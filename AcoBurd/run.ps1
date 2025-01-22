# Define the project directory (current working directory)
$PROJECT_DIR = Get-Location

# Run the setup script to ensure PlatformIO is installed and environment is prepared
Write-Output "=== Running Setup Script ==="
& .\setup.ps1

# Define the COM port to use
# come fix later ?
$PORT = "COM3"

# Build the project using PlatformIO
Write-Output "=== Building the Project ==="
platformio run --target upload --upload-port $PORT

# Check if the build succeeded
if ($LASTEXITCODE -eq 0) {
    Write-Output "=== Build and upload completed successfully ==="
} else {
    Write-Output "=== Build failed ==="
    exit 1
}
