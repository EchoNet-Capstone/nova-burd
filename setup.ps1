# Define project directory (assumes the script is run from the project root)
$PROJECT_DIR = Get-Location

# Function to check and install PlatformIO
function Install-PlatformIO {
    Write-Output "-- Checking for PlatformIO --"

    # Check if PlatformIO is installed
    $platformio = Get-Command "platformio" -ErrorAction SilentlyContinue
    if (-not $platformio) {
        Write-Output "-- PlatformIO not found. Installing... --"
        
        # Check if pip is available
        $pip = Get-Command "pip" -ErrorAction SilentlyContinue
        if ($pip) {
            # Attempt to install PlatformIO
            pip install -U platformio
            if ($LASTEXITCODE -ne 0) {
                Write-Output "Error: Failed to install PlatformIO. Ensure Python and pip are installed."
                exit 1
            }
        } else {
            Write-Output "Error: pip is not installed. Please install Python and pip first."
            exit 1
        }
    } else {
        Write-Output "PlatformIO is already installed."
    }
}

# Pre-build tasks
function Pre-Build {
    Write-Output "=== Pre-Build Step: Preparing environment ==="

    # Clean build directory
    $buildDir = Join-Path -Path $PROJECT_DIR -ChildPath ".pio/build"
    if (Test-Path -Path $buildDir) {
        Write-Output "Cleaning build directory..."
        Remove-Item -Path $buildDir -Recurse -Force
    }
}

# Main script logic
Install-PlatformIO
Pre-Build
