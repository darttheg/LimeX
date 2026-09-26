# Builds Release x64 and refreshes what the LimeVS extension ships (cmd, api, template).
# Usage: updateResources.ps1 [-NoBuild]
param([switch]$NoBuild)

$ErrorActionPreference = 'Stop'

$Root = $PSScriptRoot
$Deps = if ($env:LimeDepsDir) { $env:LimeDepsDir } else { Split-Path $Root }
$PlayerExe = Join-Path $Root 'LimePlayer\x64\Release\LimePlayer.exe'
$EmbeddedPlayer = Join-Path $Root 'LimeBuilder\lp\LimePlayer.exe'
$BuilderExe = Join-Path $Root 'LimeBuilder\x64\Release\LimeBuilder.exe'
$EngineDll = Join-Path $Root 'LimeEngine\bin\x64\Release\LimeEngine.dll'
$Template = Join-Path $Root 'LimeVS\template'

function Find-MSBuild {
	$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
	if (-not (Test-Path $vswhere)) { throw 'vswhere.exe not found' }
	$found = & $vswhere -latest -prerelease -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
	if (-not $found) { throw 'MSBuild not found' }
	return $found
}

# OpenSSL comes from the vcpkg install the MSBuild integration points at
function Find-VcpkgBin {
	$targets = Join-Path $env:LOCALAPPDATA 'vcpkg\vcpkg.user.targets'
	if (Test-Path $targets) {
		$m = Select-String -Path $targets -Pattern 'Project="(.+?)\\scripts\\buildsystems' | Select-Object -First 1
		if ($m) { return Join-Path $m.Matches[0].Groups[1].Value 'installed\x64-windows\bin' }
	}
	throw 'vcpkg integration not found'
}

function Invoke-MSBuild([string]$target) {
	Write-Host "Building $target"
	$ErrorActionPreference = 'Continue'
	& $script:MSBuild (Join-Path $Root 'LimeX.sln') "-t:$target" -p:Configuration=Release -p:Platform=x64 -m -nologo -v:minimal -clp:ErrorsOnly
	if ($LASTEXITCODE -ne 0) { throw "MSBuild failed for $target" }
}

function Test-SameFile([string]$a, [string]$b) {
	if (-not (Test-Path $b)) { return $false }
	return (Get-FileHash $a).Hash -eq (Get-FileHash $b).Hash
}

function Update-File([string]$src, [string]$destDir, [string]$name) {
	if (-not (Test-Path $src)) { throw "Missing $src" }
	if (-not $name) { $name = Split-Path $src -Leaf }
	New-Item -ItemType Directory -Force $destDir | Out-Null
	$dest = Join-Path $destDir $name
	$rel = $dest.Substring($Root.Length + 1)
	if (Test-SameFile $src $dest) { Write-Host "  unchanged  $rel"; return }
	Copy-Item -Force $src $dest
	Write-Host "  updated    $rel"
}

if (-not $NoBuild) {
	$script:MSBuild = Find-MSBuild
	# LimeBuilder embeds lp\LimePlayer.exe, so the player has to be built and copied first
	Invoke-MSBuild 'LimePlayer'
	Update-File $PlayerExe (Split-Path $EmbeddedPlayer)
	Invoke-MSBuild 'LimeBuilder;LimeEngine;LimeAndroid'
}
elseif (-not (Test-SameFile $PlayerExe $EmbeddedPlayer)) {
	Write-Warning 'LimeBuilder\lp\LimePlayer.exe is out of date; run without -NoBuild'
}

# LimeVS\cmd has no DLLs next to the builder
$builderText = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($BuilderExe))
if ($builderText -match '(?i)lua54\.dll') { throw 'LimeBuilder.exe imports lua54.dll; it needs to link Lua statically' }

Write-Host 'Updating LimeVS'
Update-File $BuilderExe (Join-Path $Root 'LimeVS\cmd')
foreach ($api in 'Enums.lua', 'Lime.lua') {
	Update-File (Join-Path $Root "LimeEngine\api\$api") (Join-Path $Root 'LimeVS\api')
}

$lib = Join-Path $Template 'lib'
$vcpkgBin = Find-VcpkgBin
Update-File $EngineDll $lib
# Your own IrrLime build, not the one that shipped with the repo
Update-File (Join-Path $Deps 'irrlicht-1.8.5\bin\Win64-VisualStudio\Irrlicht.dll') $lib
Update-File (Join-Path $vcpkgBin 'libssl-3-x64.dll') $lib
Update-File (Join-Path $vcpkgBin 'libcrypto-3-x64.dll') $lib

Update-File $EngineDll (Join-Path $Root 'LimeEngine\bin\x64\Release\lib')

$androidSrc = Join-Path $Root 'LimeEngine\LimeAndroid'
$androidOut = Join-Path $Root 'LimeVS\android'
if (Test-Path (Join-Path $androidSrc 'bin\classes.dex')) {
	Write-Host 'Updating LimeVS\android'
	Update-File (Join-Path $androidSrc 'buildApk.ps1') $androidOut
	Update-File (Join-Path $androidSrc 'androidTools.ps1') $androidOut
	Update-File (Join-Path $androidSrc 'template\AndroidManifest.xml') (Join-Path $androidOut 'template')
	Update-File (Join-Path $androidSrc 'template\icon.png') (Join-Path $androidOut 'template')
	# lib\ because bin folders are gitignored
	Update-File (Join-Path $androidSrc 'bin\classes.dex') (Join-Path $androidOut 'lib')
	foreach ($abi in 'arm64-v8a', 'x86_64') {
		Update-File (Join-Path $androidSrc "bin\$abi\liblime.so") (Join-Path $androidOut "lib\$abi")
	}
}
else {
	Write-Host 'Skipping Android: LimeAndroid has not been built'
}

Write-Host "`nLimeVS resources are up to date."
