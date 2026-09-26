# Builds bin\<abi>\liblime.so and bin\classes.dex. Unstripped libs stay in build\<abi>-<config>.
# Usage: buildNative.ps1 [-Abi arm64-v8a,x86_64] [-Config Release|Debug] [-Clean] [-CleanOnly]
param(
	[string[]]$Abi = @('arm64-v8a', 'x86_64'),
	[ValidateSet('Release', 'Debug')][string]$Config = 'Release',
	[switch]$Clean,
	[switch]$CleanOnly
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'androidTools.ps1')
$here = $PSScriptRoot

# powershell -File passes "a,b" as one string
$Abi = @($Abi | ForEach-Object { $_ -split ',' } | Where-Object { $_ })

if ($Clean -or $CleanOnly) {
	foreach ($a in $Abi) {
		$build = Join-Path $here "build\$a-$Config"
		if (Test-Path $build) { Write-Host "Removing $build"; Remove-Item -Recurse -Force $build }
	}
	if ($CleanOnly) { return }
}

$tools = Find-AndroidTools -NeedNdk -NeedCMake -NeedBuildTools -NeedJava

Write-Host '== classes.dex'
$javaOut = Join-Path $here 'build\java'
if (Test-Path $javaOut) { Remove-Item -Recurse -Force $javaOut }
New-Item -ItemType Directory -Force "$javaOut\classes", "$here\bin" | Out-Null
$sources = Get-ChildItem -Recurse -Filter *.java (Join-Path $here 'java') | ForEach-Object FullName
Invoke-Native (Join-Path $tools.JavaHome 'bin\javac.exe') (@('--release', '11', '-nowarn', '-classpath', $tools.AndroidJar, '-d', "$javaOut\classes") + $sources)
$classes = Get-ChildItem -Recurse -Filter *.class "$javaOut\classes" | ForEach-Object FullName
$env:JAVA_HOME = $tools.JavaHome
Invoke-Native (Join-Path $tools.BuildTools 'd8.bat') (@('--release', '--min-api', "$MinSdk", '--lib', $tools.AndroidJar, '--output', $javaOut) + $classes)
Copy-Item "$javaOut\classes.dex" (Join-Path $here 'bin\classes.dex') -Force

foreach ($a in $Abi) {
	$build = Join-Path $here "build\$a-$Config"
	Write-Host "== $a ($Config)"
	Invoke-Native $tools.CMake @(
		'-S', $here, '-B', $build, '-G', 'Ninja',
		"-DCMAKE_MAKE_PROGRAM=$($tools.Ninja)",
		"-DCMAKE_TOOLCHAIN_FILE=$($tools.Ndk)\build\cmake\android.toolchain.cmake",
		"-DANDROID_ABI=$a",
		"-DANDROID_PLATFORM=android-$MinSdk",
		'-DANDROID_STL=c++_static',
		"-DCMAKE_BUILD_TYPE=$Config")
	Invoke-Native $tools.CMake @('--build', $build, '--target', 'lime', '--parallel')

	$out = Join-Path $here "bin\$a"
	New-Item -ItemType Directory -Force $out | Out-Null
	$strip = Join-Path $tools.Ndk 'toolchains\llvm\prebuilt\windows-x86_64\bin\llvm-strip.exe'
	Invoke-Native $strip @('--strip-unneeded', '-o', (Join-Path $out 'liblime.so'), (Join-Path $build 'liblime.so'))
	Write-Host "  -> $(Join-Path $out 'liblime.so') ($([math]::Round((Get-Item (Join-Path $out 'liblime.so')).Length / 1MB, 1)) MB)"
}
