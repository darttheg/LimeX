# Finds the Android SDK, NDK, CMake, Ninja and Java.
# Paths come from local.properties (sdk.dir, ndk.dir, java.home, cmake.dir, ninja.dir), then ANDROID_HOME/JAVA_HOME/PATH.

$MinSdk = 24
$TargetSdk = 36

function Invoke-Native([string]$exe, [string[]]$argv) {
	# Use the exit code; PowerShell 5 treats native stderr as errors
	$ErrorActionPreference = 'Continue'
	& $exe @argv
	if ($LASTEXITCODE -ne 0) { throw "$([IO.Path]::GetFileName($exe)) failed with exit code $LASTEXITCODE" }
}

function Get-LocalProperties {
	$props = @{}
	$file = Join-Path $PSScriptRoot 'local.properties'
	if (Test-Path $file) {
		foreach ($line in Get-Content $file) {
			if ($line -match '^\s*([^#=]+?)\s*=\s*(.*?)\s*$') { $props[$Matches[1]] = $Matches[2] -replace '\\\\', '\' -replace '\\:', ':' }
		}
	}
	return $props
}

function Get-NewestChild([string]$dir) {
	if (-not (Test-Path $dir)) { return $null }
	Get-ChildItem -Directory $dir |
		Where-Object { $_.Name -match '^\d+(\.\d+)*' } |
		Sort-Object { $v = $_.Name -replace '[^\d.].*$', ''; [version]($v + '.0' * [math]::Max(0, 2 - ($v -split '\.').Count + 1)) } |
		Select-Object -Last 1
}

function Find-Exe([string]$name, [string[]]$dirs) {
	foreach ($d in $dirs) {
		if ($d -and (Test-Path (Join-Path $d $name))) { return (Join-Path $d $name) }
	}
	$cmd = Get-Command $name -ErrorAction SilentlyContinue
	if ($cmd) { return $cmd.Source }
	return $null
}

function Find-AndroidTools([switch]$NeedNdk, [switch]$NeedCMake, [switch]$NeedBuildTools, [switch]$NeedJava) {
	$local = Get-LocalProperties
	$sdk = @($local['sdk.dir'], $env:ANDROID_HOME, $env:ANDROID_SDK_ROOT, (Join-Path $env:LOCALAPPDATA 'Android\Sdk')) |
		Where-Object { $_ -and (Test-Path $_) } | Select-Object -First 1
	if (-not $sdk) { throw 'Android SDK not found. Set lime.android.sdkPath in VS Code settings, sdk.dir in local.properties, or ANDROID_HOME.' }
	$tools = @{ Sdk = $sdk }

	if ($NeedNdk) {
		$ndk = if ($local['ndk.dir']) { $local['ndk.dir'] } else { (Get-NewestChild (Join-Path $sdk 'ndk')).FullName }
		if (-not $ndk -or -not (Test-Path $ndk)) { throw "No NDK in $sdk\ndk. Set lime.android.sdkPath in VS Code settings, sdk.dir in local.properties, or ANDROID_HOME." }
		$tools.Ndk = $ndk
	}
	if ($NeedCMake) {
		$sdkCMake = Get-NewestChild (Join-Path $sdk 'cmake')
		$dirs = @($local['cmake.dir'], $(if ($sdkCMake) { Join-Path $sdkCMake.FullName 'bin' }))
		$tools.CMake = Find-Exe 'cmake.exe' $dirs
		$tools.Ninja = Find-Exe 'ninja.exe' (@($local['ninja.dir']) + $dirs)
		if (-not $tools.CMake) { throw 'CMake not found. Set cmake.dir in local.properties or put cmake on PATH.' }
		if (-not $tools.Ninja) { throw 'Ninja not found. Put ninja on PATH or install the SDK CMake package.' }
	}
	if ($NeedBuildTools) {
		$bt = Get-NewestChild (Join-Path $sdk 'build-tools')
		if (-not $bt) { throw "No build-tools in $sdk\build-tools. Set lime.android.sdkPath in VS Code settings, sdk.dir in local.properties, or ANDROID_HOME." }
		$tools.BuildTools = $bt.FullName
		$platform = Get-ChildItem -Directory (Join-Path $sdk 'platforms') -ErrorAction SilentlyContinue |
			Where-Object { Test-Path (Join-Path $_.FullName 'android.jar') } |
			Sort-Object { [double](($_.Name -replace '^android-', '') -replace '[^\d.]', '') } | Select-Object -Last 1
		if (-not $platform) { throw "No SDK platform in $sdk\platforms. Set lime.android.sdkPath in VS Code settings, sdk.dir in local.properties, or ANDROID_HOME." }
		$tools.AndroidJar = Join-Path $platform.FullName 'android.jar'
	}
	if ($NeedJava) {
		$javaHome = @($local['java.home'], $env:JAVA_HOME, 'C:\Program Files\Android\Android Studio\jbr') |
			Where-Object { $_ -and (Test-Path (Join-Path $_ 'bin\java.exe')) } | Select-Object -First 1
		if (-not $javaHome) { throw 'Java not found. Set lime.android.javaHome in VS Code settings, java.home in local.properties, or JAVA_HOME.' }
		$tools.JavaHome = $javaHome
	}
	return $tools
}
