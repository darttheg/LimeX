$ErrorActionPreference = 'Stop'

$Root = Split-Path $PSScriptRoot
$Deps = if ($env:LimeDepsDir) { $env:LimeDepsDir } else { Split-Path $Root }
$Release = "$Root\LimeEngine\bin\x64\Release"
$Android = "$Root\LimeEngine\LimeAndroid"
$Out = "$Root\LimeVS"

$targets = Join-Path $env:LOCALAPPDATA 'vcpkg\vcpkg.user.targets'
$vcpkg = (Select-String -Path $targets -Pattern 'Project="(.+?)\\scripts\\buildsystems').Matches[0].Groups[1].Value + '\installed\x64-windows\bin'

# LimeVS\cmd has no DLLs next to the builder
$builder = "$Root\LimeBuilder\x64\Release\LimeBuilder.exe"
if ([Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($builder)) -match '(?i)lua54\.dll') { throw 'LimeBuilder.exe imports lua54.dll; it needs to link Lua statically' }

$files = @(
	@($builder, "$Out\cmd"),
	@("$Root\LimeEngine\api\Enums.lua", "$Out\api"),
	@("$Root\LimeEngine\api\Lime.lua", "$Out\api"),
	@("$Release\LimeEngine.dll", "$Out\template\lib"),
	@("$Release\LimeEngine.dll", "$Release\lib"),
	# Your own IrrLime build, not the one that shipped with the repo
	@("$Deps\irrlicht-1.8.5\bin\Win64-VisualStudio\Irrlicht.dll", "$Out\template\lib"),
	@("$vcpkg\libssl-3-x64.dll", "$Out\template\lib"),
	@("$vcpkg\libcrypto-3-x64.dll", "$Out\template\lib")
)

if (Test-Path "$Android\bin\classes.dex") {
	$files += @("$Android\buildApk.ps1", "$Out\android"), @("$Android\androidTools.ps1", "$Out\android")
	$files += @("$Android\template\AndroidManifest.xml", "$Out\android\template"), @("$Android\template\icon.png", "$Out\android\template")
	# lib\ because bin folders are gitignored
	$files += ,@("$Android\bin\classes.dex", "$Out\android\lib")
	foreach ($abi in 'arm64-v8a', 'x86_64') { $files += , @("$Android\bin\$abi\liblime.so", "$Out\android\lib\$abi") }
}
else { Write-Host 'Skipping Android: LimeAndroid has not been built' }

foreach ($f in $files) {
	New-Item -ItemType Directory -Force $f[1] | Out-Null
	Copy-Item -Force $f[0] $f[1]
	Write-Host "  $($f[0]) -> $($f[1])"
}
