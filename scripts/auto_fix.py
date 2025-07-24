#!/usr/bin/env python3
"""
Auto-fix script for CogML build issues.
This script attempts to automatically resolve common build problems.
"""

import os
import sys
import subprocess
import argparse
import json
import logging
from pathlib import Path

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class BuildAutoFixer:
    def __init__(self, repo_root, max_attempts=3):
        self.repo_root = Path(repo_root)
        self.max_attempts = max_attempts
        self.ci_artifacts_dir = self.repo_root / "ci_artifacts"
        self.ci_artifacts_dir.mkdir(exist_ok=True)
        
    def run_command(self, cmd, cwd=None, capture_output=True):
        """Run a command and return the result."""
        try:
            result = subprocess.run(
                cmd, 
                shell=True, 
                cwd=cwd or self.repo_root,
                capture_output=capture_output,
                text=True,
                timeout=300  # 5 minute timeout
            )
            return result
        except subprocess.TimeoutExpired:
            logger.error(f"Command timed out: {cmd}")
            return None
        except Exception as e:
            logger.error(f"Error running command '{cmd}': {e}")
            return None
    
    def check_dependencies(self):
        """Check if basic build dependencies are available."""
        logger.info("Checking build dependencies...")
        deps = ["cmake", "make", "gcc", "g++"]
        missing = []
        
        for dep in deps:
            result = self.run_command(f"which {dep}")
            if result and result.returncode != 0:
                missing.append(dep)
        
        if missing:
            logger.warning(f"Missing dependencies: {missing}")
            return False
        return True
    
    def fix_cmake_policy_warnings(self):
        """Fix common CMake policy warnings."""
        logger.info("Fixing CMake policy warnings...")
        
        # Find CMakeLists.txt files that might need policy updates
        cmake_files = list(self.repo_root.glob("**/CMakeLists.txt"))
        
        for cmake_file in cmake_files:
            try:
                content = cmake_file.read_text()
                
                # Add CMake policy fixes for Boost
                if "FIND_PACKAGE(Boost" in content and "CMAKE_POLICY(SET CMP0167" not in content:
                    logger.info(f"Adding Boost policy fix to {cmake_file}")
                    
                    # Find the CMAKE_MINIMUM_REQUIRED line
                    lines = content.split('\n')
                    insert_index = -1
                    
                    for i, line in enumerate(lines):
                        if "CMAKE_MINIMUM_REQUIRED" in line:
                            insert_index = i + 1
                            break
                    
                    if insert_index > 0:
                        policy_lines = [
                            "",
                            "# Fix for modern CMake Boost module policy",
                            "IF(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)",
                            "    CMAKE_POLICY(SET CMP0167 NEW)",
                            "ENDIF()",
                            ""
                        ]
                        
                        lines[insert_index:insert_index] = policy_lines
                        cmake_file.write_text('\n'.join(lines))
                        logger.info(f"Updated {cmake_file} with Boost policy fix")
                        
            except Exception as e:
                logger.error(f"Error updating {cmake_file}: {e}")
    
    def fix_cmake_config_files(self):
        """Create missing CMake config files for dependencies."""
        logger.info("Creating missing CMake config files...")
        
        # Use the dedicated script to create consistent config files
        config_script = self.repo_root / "scripts/create_cmake_configs.py"
        if config_script.exists():
            logger.info("Running CMake config creation script...")
            result = self.run_command(f"python3 {config_script}")
            if result and result.returncode == 0:
                logger.info("CMake config files created successfully")
            else:
                logger.warning("CMake config creation script failed")
        else:
            logger.warning("CMake config creation script not found, skipping")
    
    def fix_cython_warnings(self):
        """Fix Cython warning about nogil keyword placement."""
        logger.info("Fixing Cython nogil keyword warnings...")
        
        # Find .pxd files that might have nogil warnings
        pxd_files = list(self.repo_root.glob("**/*.pxd"))
        
        for pxd_file in pxd_files:
            try:
                content = pxd_file.read_text()
                
                # Fix nogil except + pattern
                if "nogil except +" in content:
                    logger.info(f"Fixing nogil keyword placement in {pxd_file}")
                    
                    # Replace nogil except + with except + nogil
                    fixed_content = content.replace("nogil except +", "except + nogil")
                    pxd_file.write_text(fixed_content)
                    
                    logger.info(f"Fixed nogil warnings in {pxd_file}")
                    
            except Exception as e:
                logger.error(f"Error fixing Cython file {pxd_file}: {e}")
    
    def fix_missing_directories(self):
        """Create missing directories that the build expects."""
        logger.info("Creating missing directories...")
        
        # Common missing directories
        missing_dirs = [
            "atomspace/lib",
            "atomspace-rocks/lib", 
            "atomspace-restful/lib",
            "ure/lib",
            "moses/lib",
            "cogserver/lib"
        ]
        
        for dir_path in missing_dirs:
            full_path = self.repo_root / dir_path
            if not full_path.exists():
                full_path.mkdir(parents=True, exist_ok=True)
                
                # Create a minimal CMakeLists.txt for lib directories
                lib_cmake = full_path / "CMakeLists.txt"
                if not lib_cmake.exists():
                    lib_cmake.write_text("# Empty lib directory for build compatibility\n")
                    logger.info(f"Created {full_path} with CMakeLists.txt")
    
    def attempt_build_fix(self, build_cmd, attempt_num):
        """Attempt to fix and run a build command."""
        logger.info(f"Build fix attempt {attempt_num}/{self.max_attempts}")
        
        if attempt_num == 1:
            # First attempt: Fix dependencies and missing directories
            self.check_dependencies()
            self.fix_missing_directories()
            self.fix_cython_warnings()
            
        elif attempt_num == 2:
            # Second attempt: Fix CMake policies and config files
            self.fix_cmake_policy_warnings()
            self.fix_cmake_config_files()
            
        elif attempt_num == 3:
            # Third attempt: Clean build
            logger.info("Performing clean build...")
            clean_cmd = "rm -rf build && mkdir -p build"
            self.run_command(clean_cmd)
        
        # Try the build command
        logger.info(f"Running build command: {build_cmd}")
        result = self.run_command(build_cmd, capture_output=False)
        
        if result and result.returncode == 0:
            logger.info("Build successful!")
            self.create_success_report(attempt_num)
            return True
        else:
            logger.warning(f"Build attempt {attempt_num} failed")
            return False
    
    def create_success_report(self, attempt_num):
        """Create a success report."""
        report = {
            "status": "success",
            "attempts": attempt_num,
            "timestamp": subprocess.check_output(["date", "-u", "+%Y-%m-%dT%H:%M:%SZ"]).decode().strip(),
            "fixes_applied": [
                "Dependencies checked",
                "Missing directories created", 
                "Cython warnings fixed",
                "CMake policies updated" if attempt_num >= 2 else None,
                "CMake config files created" if attempt_num >= 2 else None,
                "Clean build performed" if attempt_num >= 3 else None
            ]
        }
        
        report_file = self.ci_artifacts_dir / "success_report.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        logger.info(f"Success report created: {report_file}")
    
    def create_escalation_report(self):
        """Create an escalation report when auto-fix fails."""
        report = {
            "status": "escalation_required",
            "attempts": self.max_attempts,
            "timestamp": subprocess.check_output(["date", "-u", "+%Y-%m-%dT%H:%M:%SZ"]).decode().strip(),
            "message": "Auto-fix could not resolve the build issues. Manual intervention required.",
            "suggested_actions": [
                "Review build logs for specific error messages",
                "Check for missing system dependencies",
                "Verify CMake configuration",
                "Examine compiler compatibility"
            ]
        }
        
        report_file = self.ci_artifacts_dir / "escalation_report.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        logger.info(f"Escalation report created: {report_file}")

def main():
    parser = argparse.ArgumentParser(description="Auto-fix CogML build issues")
    parser.add_argument("--build-cmd", required=True, help="Build command to execute")
    parser.add_argument("--max-attempts", type=int, default=3, help="Maximum fix attempts")
    parser.add_argument("--repo-root", default=".", help="Repository root directory")
    
    args = parser.parse_args()
    
    fixer = BuildAutoFixer(args.repo_root, args.max_attempts)
    
    for attempt in range(1, args.max_attempts + 1):
        if fixer.attempt_build_fix(args.build_cmd, attempt):
            sys.exit(0)
    
    # All attempts failed
    logger.error("All build fix attempts failed")
    fixer.create_escalation_report()
    sys.exit(1)

if __name__ == "__main__":
    main()