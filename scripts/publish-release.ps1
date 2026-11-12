param([string]$Version = '0.1.0')
$ErrorActionPreference = 'Stop'
if ($Version -notmatch '^\d+\.\d+\.\d+$') { throw 'Invalid version' }
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root
if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
    throw 'Install GitHub CLI: winget install --id GitHub.cli -e. Then open a new terminal and run gh auth login.'
}
& gh auth status
if ($LASTEXITCODE) { throw 'Run gh auth login first' }
if ((git branch --show-current) -ne 'master') { throw 'Switch to master before publishing' }
if (git status --porcelain) { throw 'Commit local changes before publishing' }
& "$PSScriptRoot\build-release.ps1" -Version $Version
& git push origin master
if ($LASTEXITCODE) { throw 'Push failed; release was not created' }
$commit = git rev-parse HEAD
$zip = Join-Path $root "build\releases\GameEngine-$Version-windows-x64.zip"
& gh release create "v$Version" $zip "$zip.sha256" --repo kek1v/GameEngine --target $commit --title "Tank Arena $Version" --notes-file (Join-Path $root 'RELEASE_NOTES.md')
if ($LASTEXITCODE) { throw 'Release publication failed; existing releases were not overwritten' }
