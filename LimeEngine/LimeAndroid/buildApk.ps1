# Packages a Lime project as an APK. Settings come from <project>\android.json.
# Usage: buildApk.ps1 -Project <dir> [-Out <file.apk>] [-Abi x86_64] [-Run | -RunOnly] [-Device <serial>] [-Keystore <file> -KeyAlias <a> -StorePass <p> -KeyPass <p>]
param(
	[Parameter(Mandatory = $true)][string]$Project,
	[string]$Out,
	[string]$PackageName,
	[string]$AppName,
	[string]$VersionName,
	[int]$VersionCode = 0,
	[ValidateSet('', 'landscape', 'portrait', 'fullSensor', 'sensorLandscape', 'sensorPortrait', 'unspecified')][string]$Orientation = '',
	[ValidateSet('', 'escape', 'exit')][string]$BackButton = '',
	[string[]]$Abi,
	[string]$NativeLibDir,
	[string]$Builder,
	[string]$Keystore,
	[string]$KeyAlias = 'androiddebugkey',
	[string]$StorePass = 'android',
	[string]$KeyPass = 'android',
	[string]$Device,
	[switch]$RunOnly,
	[switch]$Run
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'androidTools.ps1')
Add-Type -AssemblyName System.Drawing

# bin\ from buildNative.ps1, lib\ when shipped with LimeVS
if (-not $NativeLibDir) {
	$NativeLibDir = @((Join-Path $PSScriptRoot 'lib'), (Join-Path $PSScriptRoot 'bin')) |
		Where-Object { Test-Path (Join-Path $_ 'classes.dex') } | Select-Object -First 1
	if (-not $NativeLibDir) { throw 'No Android engine next to buildApk.ps1 (bin\ or lib\ with classes.dex). Build LimeAndroid first.' }
}

$Project = (Resolve-Path $Project).Path.TrimEnd('\')
$tools = Find-AndroidTools -NeedBuildTools -NeedJava
$env:JAVA_HOME = $tools.JavaHome
$env:PATH = "$($tools.JavaHome)\bin;$env:PATH"

$config = @{}
$configFile = Join-Path $Project 'android.json'
if (Test-Path $configFile) {
	(Get-Content -Raw $configFile | ConvertFrom-Json).PSObject.Properties | ForEach-Object { $config[$_.Name] = $_.Value }
}
$folderName = Split-Path $Project -Leaf
if (-not $AppName) { $AppName = if ($config.name) { $config.name } else { $folderName } }
if (-not $PackageName) {
	$PackageName = if ($config.package) { $config.package } else {
		$id = ($folderName.ToLower() -replace '[^a-z0-9_]', '')
		if ($id -notmatch '^[a-z]') { $id = "game$id" }
		"com.lime.$id"
	}
}
if ($PackageName -notmatch '^[a-zA-Z][a-zA-Z0-9_]*(\.[a-zA-Z][a-zA-Z0-9_]*)+$') { throw "Invalid package name '$PackageName' (expected something like com.example.game)." }
if (-not $VersionName) { $VersionName = if ($config.version) { [string]$config.version } else { '1.0' } }
if ($VersionCode -le 0) { $VersionCode = if ($config.versionCode) { [int]$config.versionCode } else { 1 } }
if (-not $Orientation) { $Orientation = if ($config.orientation) { $config.orientation } else { 'landscape' } }
if (-not $BackButton) { $BackButton = if ($config.backButton) { $config.backButton } else { 'escape' } }
if ($BackButton -notin 'escape', 'exit') { throw "backButton must be 'escape' or 'exit', not '$BackButton'." }
if (-not $Out) { $Out = Join-Path $Project ("bin-android\" + ($AppName -replace '[\\/:*?"<>|]', '_') + '.apk') }

function Start-OnDevice {
	$adb = Join-Path $tools.Sdk 'platform-tools\adb.exe'
	if (-not $Device) {
		$devices = @(& $adb devices | Select-Object -Skip 1 | Where-Object { $_ -match '\tdevice$' })
		if (-not $devices) { throw 'No Android device or emulator connected (adb devices is empty).' }
		$script:Device = ($devices[0] -split "`t")[0]
	}
	$target = @('-s', $Device)
	Write-Host "Installing on $Device"
	Invoke-Native $adb ($target + @('install', '-r', $Out))
	Invoke-Native $adb ($target + @('shell', 'am', 'force-stop', $PackageName))
	Invoke-Native $adb ($target + @('logcat', '-c'))
	Invoke-Native $adb ($target + @('shell', 'am', 'start', '-n', "$PackageName/org.lime.LimeActivity"))
	Write-Host 'Showing the game log (Ctrl+C to stop)'
	& $adb @target logcat -v time -s Lime:V Irrlicht:V AndroidRuntime:E DEBUG:F
}

if ($RunOnly) {
	if (-not (Test-Path $Out)) { throw "No APK at $Out. Build it first." }
	Start-OnDevice
	return
}

# powershell -File passes "a,b" as one string
$Abi = @($Abi | ForEach-Object { $_ -split ',' } | Where-Object { $_ })
if (-not $Abi) { $Abi = Get-ChildItem -Directory $NativeLibDir | Where-Object { Test-Path (Join-Path $_.FullName 'liblime.so') } | ForEach-Object Name }
if (-not $Abi) { throw "No liblime.so under $NativeLibDir." }

if (-not $Builder) {
	# LimeVS\cmd, or the repo's Release build
	$Builder = @((Join-Path $PSScriptRoot '..\cmd\LimeBuilder.exe'), (Join-Path $PSScriptRoot '..\..\LimeBuilder\x64\Release\LimeBuilder.exe')) |
		Where-Object { Test-Path $_ } | Select-Object -First 1
	if (-not $Builder) { throw 'LimeBuilder.exe not found; pass -Builder.' }
}

Write-Host "Packaging $AppName ($PackageName $VersionName, $($Abi -join ', '))"

# 'stored' files go in uncompressed (native libs, media that's already compressed), 'deflated' ones compressed.
# jar adds them because aapt2 writes backslashes into asset paths on Windows.
$stage = Join-Path $Project '.android-build'
if (Test-Path $stage) { Remove-Item -Recurse -Force $stage }
$stored = "$stage\stored"
$deflated = "$stage\deflated"
New-Item -ItemType Directory -Force "$stored\assets\lime", "$deflated\assets\lime", "$stage\res\mipmap-xxxhdpi" | Out-Null
$storedExtensions = @('.png', '.jpg', '.jpeg', '.webp', '.mp3', '.ogg', '.wav', '.flac', '.zip', '.limepkg')
function Get-StageRoot([string]$fileName) {
	if ($storedExtensions -contains [IO.Path]::GetExtension($fileName).ToLower()) { return $stored }
	return $deflated
}

Invoke-Native $Builder @($Project, "$stored\assets\lime", '--package-only', '--platform', 'Android')

# Same ignore rules as Lime: Package
$ignore = [System.Collections.Generic.List[string]]@('.limepkg', '.ico', '.exp', '.lib', '.pdb', '.log', '.luarc.json',
	'.vscode', '.ignore', 'bin', '.git', '.gitignore', '.gitmodules', '.gitattributes',
	'.exe', '.dll', 'bin-android', '.android-build', 'android.json')
$ignoreFile = Join-Path $Project '.ignore'
if (Test-Path $ignoreFile) {
	Get-Content $ignoreFile | ForEach-Object { $_.Trim() } | Where-Object { $_ -and -not $_.StartsWith('#') } | ForEach-Object { $ignore.Add($_) }
}
function Test-Ignored([string]$name) {
	foreach ($entry in $ignore) {
		if ($entry.StartsWith('.')) { if ($name.EndsWith($entry)) { return $true } }
		elseif ($name -eq $entry) { return $true }
	}
	return $false
}

$files = [System.Collections.Generic.List[string]]::new()
function Add-GameFiles([string]$dir, [string]$rel) {
	foreach ($item in Get-ChildItem -Force $dir) {
		if (Test-Ignored $item.Name) { continue }
		$itemRel = if ($rel) { "$rel/$($item.Name)" } else { $item.Name }
		if ($item.PSIsContainer) {
			Add-GameFiles $item.FullName $itemRel
		} else {
			$dest = Join-Path "$(Get-StageRoot $item.Name)\assets\game" ($itemRel -replace '/', '\')
			New-Item -ItemType Directory -Force (Split-Path $dest) | Out-Null
			Copy-Item $item.FullName $dest
			$files.Add($itemRel)
		}
	}
}
Add-GameFiles $Project ''

# First line is the build id, AndroidMain re-extracts the game files when it changes
$buildId = '{0:yyyyMMddHHmmss}-{1}' -f (Get-Date), [guid]::NewGuid().ToString('N').Substring(0, 8)
[IO.File]::WriteAllLines("$deflated\assets\lime\manifest.txt", [string[]](@($buildId) + $files), (New-Object Text.UTF8Encoding $false))
Write-Host "  $($files.Count) game files"

$escape = { param($s) [Security.SecurityElement]::Escape($s) }
$manifest = (Get-Content -Raw (Join-Path $PSScriptRoot 'template\AndroidManifest.xml')).
	Replace('{{PACKAGE}}', $PackageName).Replace('{{LABEL}}', (& $escape $AppName)).
	Replace('{{VERSION_CODE}}', [string]$VersionCode).Replace('{{VERSION_NAME}}', (& $escape $VersionName)).
	Replace('{{ORIENTATION}}', $Orientation).Replace('{{BACK_CALLBACK}}', $(if ($BackButton -eq 'exit') { 'true' } else { 'false' }))
[IO.File]::WriteAllText("$stage\AndroidManifest.xml", $manifest, (New-Object Text.UTF8Encoding $false))

$iconSource = @((Join-Path $Project 'icon.png'), (Join-Path $Project 'icon.ico'), (Join-Path $PSScriptRoot 'template\icon.png')) |
	Where-Object { Test-Path $_ } | Select-Object -First 1
$image = if ($iconSource.EndsWith('.ico')) {
	$icon = New-Object Drawing.Icon($iconSource, 512, 512)
	$bmp = $icon.ToBitmap(); $icon.Dispose(); $bmp
} else { [Drawing.Image]::FromFile($iconSource) }
$launcher = New-Object Drawing.Bitmap(192, 192)
$g = [Drawing.Graphics]::FromImage($launcher)
$g.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
$g.DrawImage($image, 0, 0, 192, 192)
$g.Dispose(); $image.Dispose()
$launcher.Save("$stage\res\mipmap-xxxhdpi\ic_launcher.png", [Drawing.Imaging.ImageFormat]::Png)
$launcher.Dispose()

$aapt2 = Join-Path $tools.BuildTools 'aapt2.exe'
Invoke-Native $aapt2 @('compile', '--dir', "$stage\res", '-o', "$stage\res.zip")
$unsigned = "$stage\unsigned.apk"
Invoke-Native $aapt2 @('link', '-o', $unsigned, '-I', $tools.AndroidJar, '--manifest', "$stage\AndroidManifest.xml",
	'--min-sdk-version', "$MinSdk", '--target-sdk-version', "$TargetSdk", "$stage\res.zip")

Copy-Item (Join-Path $NativeLibDir 'classes.dex') "$deflated\classes.dex"
foreach ($a in $Abi) {
	New-Item -ItemType Directory -Force "$stored\lib\$a" | Out-Null
	Copy-Item (Join-Path $NativeLibDir "$a\liblime.so") "$stored\lib\$a\liblime.so"
}
$jar = Join-Path $tools.JavaHome 'bin\jar.exe'
Invoke-Native $jar @('--update', "--file=$unsigned", '--no-manifest', '-C', $deflated, 'assets', '-C', $deflated, 'classes.dex')
Invoke-Native $jar @('--update', "--file=$unsigned", '--no-manifest', '--no-compress', '-C', $stored, 'assets', '-C', $stored, 'lib')

# -P 16: 16 KB page alignment for the native libs
$aligned = "$stage\aligned.apk"
Invoke-Native (Join-Path $tools.BuildTools 'zipalign.exe') @('-f', '-P', '16', '4', $unsigned, $aligned)

if (-not $Keystore) {
	$Keystore = Join-Path $env:USERPROFILE '.android\debug.keystore'
	if (-not (Test-Path $Keystore)) {
		Write-Host "Creating debug keystore $Keystore"
		New-Item -ItemType Directory -Force (Split-Path $Keystore) | Out-Null
		Invoke-Native (Join-Path $tools.JavaHome 'bin\keytool.exe') @('-genkeypair', '-keystore', $Keystore,
			'-storepass', 'android', '-alias', 'androiddebugkey', '-keypass', 'android', '-keyalg', 'RSA',
			'-keysize', '2048', '-validity', '10000', '-dname', 'CN=Android Debug,O=Android,C=US')
	}
}

New-Item -ItemType Directory -Force (Split-Path $Out) | Out-Null
Invoke-Native (Join-Path $tools.BuildTools 'apksigner.bat') @('sign', '--ks', $Keystore, '--ks-key-alias', $KeyAlias,
	'--ks-pass', "pass:$StorePass", '--key-pass', "pass:$KeyPass", '--out', $Out, $aligned)
Invoke-Native (Join-Path $tools.BuildTools 'apksigner.bat') @('verify', $Out)

Remove-Item -Recurse -Force $stage
Write-Host "Created $Out ($([math]::Round((Get-Item $Out).Length / 1MB, 1)) MB)"

if ($Run) { Start-OnDevice }
