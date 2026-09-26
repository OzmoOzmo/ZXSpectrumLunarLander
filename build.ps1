param(
    [string]$Source = "LLander.c",
    [string]$Compiler = "zcc",
    [string]$Output = "LLander.bin",
    [switch]$Run
)

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $scriptDir

if (-not (Test-Path $Source)) {
    Write-Error "Source file '$Source' not found."
    exit 1
}

$baseName = [System.IO.Path]::GetFileNameWithoutExtension($Output)
$compilerArgs1 = @(
    "+zx"
    "-vn"
    $Source
    "-lndos"
    "-create-app"
    "-o$baseName"
    "-lm"
)
$compilerArgs = @(
    "+zx"
    "-vn"
    "-Dspritesize=16"
    $Source
    "-o$baseName"
    "-create-app"
    "-lndos"
)
Write-Host "Compiling $Source with $Compiler $($compilerArgs -join ' ')"
& $Compiler @compilerArgs
if ($LASTEXITCODE -ne 0) {
    Write-Error "Compilation failed (exit $LASTEXITCODE)"
    exit $LASTEXITCODE
}

Write-Host "Compiled successfully."
$resolvedBin = Join-Path $scriptDir "$baseName.bin"
$resolvedTap = Join-Path $scriptDir "$baseName.tap"
Write-Host "Binary file: $resolvedBin"
Write-Host "Tap file: $resolvedTap"

if (-not (Test-Path $resolvedTap)) {
    Write-Warning "Build finished, but '$resolvedTap' was not generated."
}

if ($Run) {
    $runFile = $resolvedTap
    if (-not (Test-Path $runFile)) {
        if (Test-Path $resolvedBin) {
            Write-Warning "Tap file not found; falling back to binary file '$resolvedBin'."
            $runFile = $resolvedBin
        } else {
            Write-Error "Cannot run because neither '$resolvedTap' nor '$resolvedBin' exists."
            exit 1
        }
    }

    Write-Host "Starting Fuse: $runFile"
    try {
        Start-Process -FilePath "fuse.exe" -ArgumentList @($runFile) -WorkingDirectory $scriptDir
    } catch {
        Write-Error "Failed to launch fuse.exe. Ensure Fuse is installed and on PATH."
        exit 1
    }
}
