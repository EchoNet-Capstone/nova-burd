#!/usr/bin/env python3
"""
PlatformIO Multi-Environment Upload Script

Sequentially builds master and slave environments, then uploads them in parallel
to multiple devices via serial ports.
"""

import argparse
import subprocess
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from typing import List, Tuple

class Colors:
    """ANSI color codes for terminal output"""
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    BOLD = '\033[1m'
    END = '\033[0m'

def print_colored(message: str, color: str = Colors.END):
    """Print colored message to terminal"""
    print(f"{color}{message}{Colors.END}")

def run_pio_command(command: List[str], description: str, verbose: bool = False) -> bool:
    """
    Run a PlatformIO command and return success status
    
    Args:
        command: List of command arguments
        description: Description for logging
        verbose: Whether to show detailed output
        
    Returns:
        bool: True if command succeeded, False otherwise
    """
    print_colored(f"🔨 {description}...", Colors.CYAN)
    
    if verbose:
        print_colored(f"Command: {' '.join(command)}", Colors.BLUE)
    
    try:
        if verbose:
            # Show real-time output in verbose mode
            result = subprocess.run(command, check=True)
        else:
            result = subprocess.run(
                command,
                capture_output=True,
                text=True,
                check=True
            )
        
        print_colored(f"✓ {description} completed successfully", Colors.GREEN)
        return True
        
    except subprocess.CalledProcessError as e:
        print_colored(f"✗ {description} failed", Colors.RED)
        if not verbose and hasattr(e, 'stderr') and e.stderr:
            print_colored(f"Error: {e.stderr.strip()}", Colors.RED)
        return False

def clean_environments(env_type: str, verbose: bool = False) -> bool:
    """
    Clean master and slave environments
    
    Args:
        env_type: Environment type (prod/dev/disassemble)
        verbose: Whether to show detailed output
        
    Returns:
        bool: True if both cleans succeeded
    """
    master_env = f"{env_type}-master"
    slave_env = f"{env_type}-slave"
    
    print_colored(f"\n{'='*50}", Colors.BOLD)
    print_colored(f"CLEANING ENVIRONMENTS FOR: {env_type.upper()}", Colors.BOLD)
    print_colored(f"{'='*50}", Colors.BOLD)
    
    # Clean master environment
    master_success = run_pio_command(
        ["pio", "run", "-e", master_env, "-t", "clean"],
        f"Cleaning {master_env}",
        verbose
    )
    
    if not master_success:
        return False
    
    # Clean slave environment
    slave_success = run_pio_command(
        ["pio", "run", "-e", slave_env, "-t", "clean"],
        f"Cleaning {slave_env}",
        verbose
    )
    
    return slave_success

def build_environments(env_type: str, verbose: bool = False) -> bool:
    """
    Build master and slave environments sequentially
    
    Args:
        env_type: Environment type (prod/dev/disassemble)
        verbose: Whether to show detailed output
        
    Returns:
        bool: True if both builds succeeded
    """
    master_env = f"{env_type}-master"
    slave_env = f"{env_type}-slave"
    
    print_colored(f"\n{'='*50}", Colors.BOLD)
    print_colored(f"BUILDING ENVIRONMENTS FOR: {env_type.upper()}", Colors.BOLD)
    print_colored(f"{'='*50}", Colors.BOLD)
    
    # Build master environment
    master_success = run_pio_command(
        ["pio", "run", "-e", master_env],
        f"Building {master_env}",
        verbose
    )
    
    if not master_success:
        return False
    
    # Build slave environment
    slave_success = run_pio_command(
        ["pio", "run", "-e", slave_env],
        f"Building {slave_env}",
        verbose
    )
    
    return slave_success

def upload_to_port(env_name: str, port: str, device_type: str, verbose: bool = False) -> Tuple[bool, str, str]:
    """
    Upload firmware to a specific port
    
    Args:
        env_name: Environment name to upload
        port: Serial port name
        device_type: "MASTER" or "SLAVE" for logging
        verbose: Whether to show detailed output
        
    Returns:
        Tuple of (success, port, device_type)
    """
    print_colored(f"📤 Starting upload: {env_name} -> {port} ({device_type})", Colors.BLUE)
    
    command = ["pio", "run", "-e", env_name, "--target", "upload", "--upload-port", port]
    
    if verbose:
        print_colored(f"Command: {' '.join(command)}", Colors.BLUE)
    
    try:
        if verbose:
            result = subprocess.run(command, check=True)
        else:
            result = subprocess.run(
                command,
                capture_output=True,
                text=True,
                check=True
            )
        
        print_colored(f"✓ Upload completed: {port} ({device_type})", Colors.GREEN)
        return True, port, device_type
        
    except subprocess.CalledProcessError as e:
        print_colored(f"✗ Upload failed: {port} ({device_type})", Colors.RED)
        if not verbose and hasattr(e, 'stderr') and e.stderr:
            print_colored(f"Error: {e.stderr.strip()}", Colors.RED)
        return False, port, device_type

def upload_parallel(env_type: str, master_port: str, slave_ports: List[str], 
                   max_workers: int = None, verbose: bool = False) -> bool:
    """
    Upload firmware to all ports in parallel
    
    Args:
        env_type: Environment type (prod/dev/disassemble)
        master_port: Port for master device
        slave_ports: List of ports for slave devices
        max_workers: Maximum number of parallel workers
        verbose: Whether to show detailed output
        
    Returns:
        bool: True if all uploads succeeded
    """
    master_env = f"{env_type}-master"
    slave_env = f"{env_type}-slave"
    
    print_colored(f"\n{'='*50}", Colors.BOLD)
    print_colored(f"UPLOADING FIRMWARE IN PARALLEL", Colors.BOLD)
    print_colored(f"{'='*50}", Colors.BOLD)
    print_colored(f"Master: {master_env} -> {master_port}", Colors.YELLOW)
    print_colored(f"Slaves: {slave_env} -> {', '.join(slave_ports)}", Colors.YELLOW)
    print_colored(f"Total devices: {1 + len(slave_ports)}", Colors.YELLOW)
    
    if max_workers:
        print_colored(f"Max parallel workers: {max_workers}", Colors.YELLOW)
    
    # Prepare upload tasks
    upload_tasks = []
    
    # Add master upload task
    upload_tasks.append((master_env, master_port, "MASTER"))
    
    # Add slave upload tasks
    for port in slave_ports:
        upload_tasks.append((slave_env, port, "SLAVE"))
    
    # Set default max_workers if not specified
    if max_workers is None:
        max_workers = len(upload_tasks)
    
    # Execute uploads in parallel
    all_successful = True
    completed_uploads = []
    failed_uploads = []
    
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        # Submit all upload tasks
        future_to_task = {
            executor.submit(upload_to_port, env, port, device_type, verbose): (env, port, device_type)
            for env, port, device_type in upload_tasks
        }
        
        # Process completed uploads
        for future in as_completed(future_to_task):
            success, port, device_type = future.result()
            
            if success:
                completed_uploads.append((port, device_type))
            else:
                failed_uploads.append((port, device_type))
                all_successful = False
    
    # Print summary
    print_colored(f"\n{'='*50}", Colors.BOLD)
    print_colored(f"UPLOAD SUMMARY", Colors.BOLD)
    print_colored(f"{'='*50}", Colors.BOLD)
    
    if completed_uploads:
        print_colored(f"✓ Successful uploads ({len(completed_uploads)}):", Colors.GREEN)
        for port, device_type in completed_uploads:
            print_colored(f"  - {port} ({device_type})", Colors.GREEN)
    
    if failed_uploads:
        print_colored(f"✗ Failed uploads ({len(failed_uploads)}):", Colors.RED)
        for port, device_type in failed_uploads:
            print_colored(f"  - {port} ({device_type})", Colors.RED)
    
    return all_successful

def create_argument_parser() -> argparse.ArgumentParser:
    """
    Create and configure the argument parser
    
    Returns:
        argparse.ArgumentParser: Configured argument parser
    """
    parser = argparse.ArgumentParser(
        description="PlatformIO Multi-Environment Upload Script",
        epilog="""
Examples:
  %(prog)s prod COM3 COM4 COM5 COM6
  %(prog)s dev /dev/ttyUSB0 /dev/ttyUSB1 /dev/ttyUSB2
  %(prog)s disassemble COM1 COM2 --verbose
        """,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "environment",
        choices=["prod", "dev", "disassemble"],
        help="Environment type to build and upload"
    )
    
    parser.add_argument(
        "master_port",
        help="Serial port for the master device (e.g., COM3, /dev/ttyUSB0)"
    )
    
    parser.add_argument(
        "slave_ports",
        nargs="+",
        help="One or more serial ports for slave devices"
    )
    
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--build-only",
        action="store_true",
        help="Only build environments, skip uploading"
    )
    
    parser.add_argument(
        "--upload-only",
        action="store_true",
        help="Skip building, only upload (assumes builds are current)"
    )
    
    parser.add_argument(
        "--max-workers",
        type=int,
        default=None,
        help="Maximum number of parallel upload workers (default: number of devices)"
    )
    
    parser.add_argument(
        "-c", "--clean",
        action="store_true",
        help="Clean environments before building"
    )
    
    return parser

def main():
    """Main function"""
    parser = create_argument_parser()
    
    try:
        args = parser.parse_args()
        
        print_colored(f"\n🚀 PlatformIO Multi-Environment Upload Script", Colors.BOLD)
        print_colored(f"Environment: {args.environment.upper()}", Colors.CYAN)
        print_colored(f"Master port: {args.master_port}", Colors.CYAN)
        print_colored(f"Slave ports: {', '.join(args.slave_ports)}", Colors.CYAN)
        
        if args.verbose:
            print_colored(f"Verbose mode: ON", Colors.CYAN)
        if args.max_workers:
            print_colored(f"Max workers: {args.max_workers}", Colors.CYAN)
        if args.clean:
            print_colored(f"Clean build: ON", Colors.CYAN)
        
        # Handle build-only mode
        if args.build_only:
            print_colored(f"Build-only mode: Skipping uploads", Colors.YELLOW)
            
            # Clean if requested
            if args.clean:
                if not clean_environments(args.environment, args.verbose):
                    print_colored(f"\n❌ Clean failed.", Colors.RED)
                    sys.exit(1)
            
            if build_environments(args.environment, args.verbose):
                print_colored(f"\n🎉 Build completed successfully!", Colors.GREEN)
                sys.exit(0)
            else:
                print_colored(f"\n❌ Build failed.", Colors.RED)
                sys.exit(1)
        
        # Handle upload-only mode
        if args.upload_only:
            print_colored(f"Upload-only mode: Skipping builds", Colors.YELLOW)
        else:
            # Clean environments if requested
            if args.clean:
                if not clean_environments(args.environment, args.verbose):
                    print_colored("\n❌ Clean failed. Aborting.", Colors.RED)
                    sys.exit(1)
            
            # Build environments sequentially
            if not build_environments(args.environment, args.verbose):
                print_colored("\n❌ Build failed. Aborting upload.", Colors.RED)
                sys.exit(1)
        
        # Upload firmware in parallel
        if upload_parallel(args.environment, args.master_port, args.slave_ports, 
                          args.max_workers, args.verbose):
            print_colored(f"\n🎉 All uploads completed successfully!", Colors.GREEN)
            sys.exit(0)
        else:
            print_colored(f"\n❌ Some uploads failed. Check the logs above.", Colors.RED)
            sys.exit(1)
            
    except KeyboardInterrupt:
        print_colored(f"\n⚠️ Operation interrupted by user", Colors.YELLOW)
        sys.exit(1)
        
    except Exception as e:
        print_colored(f"❌ Unexpected error: {e}", Colors.RED)
        sys.exit(1)

if __name__ == "__main__":
    main()