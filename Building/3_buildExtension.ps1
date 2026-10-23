param([switch]$Install)
$ErrorActionPreference = 'Stop'

Set-Location "$(Split-Path $PSScriptRoot)\LimeVS"
if (-not (Test-Path node_modules)) { npm install }

npx tsc -p .
if ($LASTEXITCODE) { throw 'tsc failed' }

$pkg = Get-Content package.json -Raw | ConvertFrom-Json
$vsix = "$($pkg.name)-$($pkg.version).vsix"
npx --yes '@vscode/vsce' package --allow-missing-repository --out $vsix
if ($LASTEXITCODE) { throw 'vsce failed' }

if ($Install) { code --install-extension $vsix --force }
