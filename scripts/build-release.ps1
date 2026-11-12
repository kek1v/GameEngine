param([string]$Version = '0.1.0')
$ErrorActionPreference = 'Stop'
if ($Version -notmatch '^\d+\.\d+\.\d+$') { throw 'Version must have the form 0.1.0' }
$root = Split-Path $PSScriptRoot -Parent
$cmake = (Get-Command cmake -ErrorAction SilentlyContinue).Source
if (-not $cmake) { $cmake = 'C:\Program Files\CMake\bin\cmake.exe' }
$build = Join-Path $root 'build\release-package'
& $cmake -S $root -B $build -G 'Visual Studio 17 2022' -A x64 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
if ($LASTEXITCODE) { throw 'CMake configuration failed' }
& $cmake --build $build --config Release --parallel 1 -- /nr:false
if ($LASTEXITCODE) { throw 'Build failed' }
& (Join-Path (Split-Path $cmake) 'ctest.exe') --test-dir $build -C Release --output-on-failure
if ($LASTEXITCODE) { throw 'Tests failed' }
$name = "GameEngine-$Version-windows-x64"
$output = Join-Path $root 'build\releases'
# Unique staging folder prevents stale files from entering a later package.
$stage = Join-Path $output ([guid]::NewGuid().ToString())
$package = Join-Path $stage $name
$null = New-Item -ItemType Directory -Path (Join-Path $package 'licenses') -Force
Copy-Item -LiteralPath (Join-Path $build 'bin\Release\GameEngine.exe') -Destination $package
Copy-Item -LiteralPath (Join-Path $root 'res') -Destination $package -Recurse
Copy-Item -LiteralPath (Join-Path $root 'README.md') -Destination $package
Copy-Item -LiteralPath (Join-Path $root 'external\glfw\LICENSE.md') -Destination (Join-Path $package 'licenses\GLFW.txt')
Copy-Item -LiteralPath (Join-Path $root 'external\glm\copying.txt') -Destination (Join-Path $package 'licenses\GLM.txt')
Get-Content (Join-Path $root 'src\Resources\stb_image.h') -Tail 42 | Set-Content (Join-Path $package 'licenses\stb_image.txt')
# Preserve the upstream notices shipped in these vendored headers.
Copy-Item -LiteralPath (Join-Path $root 'external\rapidjson\include\rapidjson\rapidjson.h') -Destination (Join-Path $package 'licenses\rapidjson.h')
Copy-Item -LiteralPath (Join-Path $root 'external\glad\include\KHR\khrplatform.h') -Destination (Join-Path $package 'licenses\khrplatform.h')
$archive = Join-Path $output "$name.zip"
Compress-Archive -LiteralPath $package -DestinationPath $archive -Force
$hash = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $name.zip" | Set-Content -LiteralPath "$archive.sha256" -Encoding ascii
Write-Output "Release archive: $archive"
Write-Output "SHA256: $hash"
