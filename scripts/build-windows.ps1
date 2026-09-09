[CmdletBinding()]
param(
    [string]$QtRoot = "C:\Qt\Qt5.14.2\5.14.2\msvc2017_64",
    [string]$VtkRoot = "C:\VTK",
    [string]$Hdf5Root = "C:\dev",
    [string]$EigenRoot = "C:\Users\13568\Desktop\PassWing\3rdLib\Eigen3",
    [switch]$Clean,
    [switch]$EnableWebEngine,
    [switch]$NoZip
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))
$buildDir = Join-Path $repoRoot "out\build\windows-msvc"
$packageDir = Join-Path $repoRoot "out\package\PassWing-Windows-x64"
$zipPath = Join-Path $repoRoot "out\package\PassWing-Windows-x64.zip"

function Assert-PathExists {
    param([string]$Path, [string]$Description)

    if (-not (Test-Path -LiteralPath $Path)) {
        throw "$Description not found: $Path"
    }
}

function Remove-WorkspaceDirectory {
    param([string]$Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        return
    }

    $workspacePrefix = $repoRoot.TrimEnd('\') + '\'
    $resolvedTarget = [IO.Path]::GetFullPath($Path).TrimEnd('\') + '\'
    if (-not $resolvedTarget.StartsWith($workspacePrefix, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to remove a directory outside the repository: $resolvedTarget"
    }

    Remove-Item -LiteralPath $Path -Recurse -Force
}

function Invoke-Checked {
    param(
        [string]$Program,
        [string[]]$Arguments
    )

    Write-Host "> $Program $($Arguments -join ' ')" -ForegroundColor DarkGray
    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code ${LASTEXITCODE}: $Program"
    }
}

function Copy-RuntimeFiles {
    param([string]$SourceDirectory, [string]$DestinationDirectory)

    $files = Get-ChildItem -LiteralPath $SourceDirectory -Filter "*.dll" -File
    if ($files.Count -eq 0) {
        throw "No runtime DLLs found in: $SourceDirectory"
    }
    Copy-Item -LiteralPath $files.FullName -Destination $DestinationDirectory -Force
}

function Copy-OptionalRuntimeData {
    param([string]$Name)

    $source = Join-Path $repoRoot $Name
    if (Test-Path -LiteralPath $source) {
        Copy-Item -LiteralPath $source -Destination $packageDir -Recurse -Force
    }
}

$cmakeCommand = Get-Command cmake.exe -ErrorAction Stop
$cmakeExe = $cmakeCommand.Source
$windeployqtExe = Join-Path $QtRoot "bin\windeployqt.exe"
$qtConfig = Join-Path $QtRoot "lib\cmake\Qt5\Qt5Config.cmake"
$vtkConfig = Join-Path $VtkRoot "lib\cmake\vtk-9.3\vtk-config.cmake"
$hdf5Config = Join-Path $Hdf5Root "cmake\hdf5-config.cmake"
$eigenConfigDir = Join-Path $EigenRoot "share\eigen3\cmake"
$eigenConfig = Join-Path $eigenConfigDir "Eigen3Config.cmake"

Assert-PathExists $qtConfig "Qt 5 CMake package"
Assert-PathExists $vtkConfig "VTK CMake package"
Assert-PathExists $hdf5Config "HDF5 CMake package"
Assert-PathExists $eigenConfig "Eigen CMake package"
Assert-PathExists $windeployqtExe "windeployqt"

$vswhereExe = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
Assert-PathExists $vswhereExe "Visual Studio locator"
$visualStudioRoot = (& $vswhereExe -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath).Trim()
if (-not $visualStudioRoot) {
    throw "Visual Studio with the MSVC C++ toolchain was not found."
}

if ($visualStudioRoot -match "2022") {
    $generator = "Visual Studio 17 2022"
} elseif ($visualStudioRoot -match "2019") {
    $generator = "Visual Studio 16 2019"
} else {
    throw "Unsupported Visual Studio installation: $visualStudioRoot"
}

if ($Clean) {
    Write-Host "Cleaning build directory..." -ForegroundColor Cyan
    Remove-WorkspaceDirectory $buildDir
}

$webEngineValue = if ($EnableWebEngine) { "ON" } else { "OFF" }
$prefixPath = @($QtRoot, $VtkRoot, $Hdf5Root, $EigenRoot) -join ";"
$configureArguments = @(
    "-S", $repoRoot,
    "-B", $buildDir,
    "-G", $generator,
    "-A", "x64",
    "-DCMAKE_PREFIX_PATH=$prefixPath",
    "-DEigen3_DIR=$eigenConfigDir",
    "-DPASSWING_ENABLE_WEBENGINE=$webEngineValue"
)

Write-Host "Configuring PassWing ($generator, x64)..." -ForegroundColor Cyan
Invoke-Checked $cmakeExe $configureArguments

Write-Host "Building PassWing (Release)..." -ForegroundColor Cyan
Invoke-Checked $cmakeExe @("--build", $buildDir, "--config", "Release", "--parallel")

$builtExe = Join-Path $buildDir "Release\PassWing.exe"
Assert-PathExists $builtExe "PassWing executable"

Write-Host "Creating clean package directory..." -ForegroundColor Cyan
Remove-WorkspaceDirectory $packageDir
New-Item -ItemType Directory -Path $packageDir -Force | Out-Null
Copy-Item -LiteralPath $builtExe -Destination $packageDir -Force

Write-Host "Deploying Qt runtime..." -ForegroundColor Cyan
$packagedExe = Join-Path $packageDir "PassWing.exe"
Invoke-Checked $windeployqtExe @(
    "--release",
    "--compiler-runtime",
    "--no-translations",
    "--dir", $packageDir,
    $packagedExe
)

Write-Host "Copying VTK and HDF5 runtime libraries..." -ForegroundColor Cyan
Copy-RuntimeFiles (Join-Path $VtkRoot "bin") $packageDir
Copy-RuntimeFiles (Join-Path $Hdf5Root "bin") $packageDir

Write-Host "Copying runtime data..." -ForegroundColor Cyan
foreach ($name in @("setting", "resoure", "theoreticalFramework", "libaries", "help", "Profili.mdb")) {
    Copy-OptionalRuntimeData $name
}

if (-not $NoZip) {
    Write-Host "Creating ZIP archive..." -ForegroundColor Cyan
    if (Test-Path -LiteralPath $zipPath) {
        Remove-Item -LiteralPath $zipPath -Force
    }
    Compress-Archive -LiteralPath $packageDir -DestinationPath $zipPath -CompressionLevel Optimal
}

$packageSize = (Get-ChildItem -LiteralPath $packageDir -File -Recurse | Measure-Object -Property Length -Sum).Sum
Write-Host ""
Write-Host "Windows package completed successfully." -ForegroundColor Green
Write-Host "Executable: $packagedExe"
Write-Host ("Package size: {0:N1} MB" -f ($packageSize / 1MB))
if (-not $NoZip) {
    Write-Host "Archive: $zipPath"
}
