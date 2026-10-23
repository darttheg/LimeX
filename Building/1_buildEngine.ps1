param([switch]$Rebuild)
$ErrorActionPreference = 'Stop'

$Root = Split-Path $PSScriptRoot
$msbuild = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -prerelease -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
$suffix = if ($Rebuild) { ':Rebuild' } else { '' }

function Build([string[]]$projects) {
	$target = ($projects | ForEach-Object { "$_$suffix" }) -join ';'
	& $msbuild "$Root\LimeX.sln" "-t:$target" -p:Configuration=Release -p:Platform=x64 -m -nologo -v:minimal -clp:ErrorsOnly
	if ($LASTEXITCODE) { throw "Build failed: $target" }
}

# LimeBuilder embeds lp\LimePlayer.exe, so player builds first
Build 'LimePlayer'
Copy-Item -Force "$Root\LimePlayer\x64\Release\LimePlayer.exe" "$Root\LimeBuilder\lp\"
Build 'LimeBuilder', 'LimeEngine', 'LimeAndroid'

$dll = "$Root\LimeEngine\bin\x64\Release\LimeEngine.dll"
Write-Host "LimeEngine.dll $([Diagnostics.FileVersionInfo]::GetVersionInfo($dll).ProductVersion)"
