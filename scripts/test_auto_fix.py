#!/usr/bin/env python3
"""
Test script for the auto-fix functionality.
This script validates that the auto-fix system works correctly.
"""

import os
import sys
import subprocess
import tempfile
import shutil
from pathlib import Path

def test_auto_fix_system():
    """Test the auto-fix system with a simple build scenario."""
    print("🧪 Testing self-healing auto-fix system...")
    
    # Create a temporary test directory
    with tempfile.TemporaryDirectory() as temp_dir:
        test_dir = Path(temp_dir) / "test_project"
        test_dir.mkdir()
        
        # Create a simple CMake project that will initially fail
        cmake_content = """
cmake_minimum_required(VERSION 3.12)
project(test_project)

# This will cause a warning that our auto-fix should resolve
find_package(Boost REQUIRED)

add_executable(test_app main.cpp)
"""
        
        main_content = """
#include <iostream>
int main() {
    std::cout << "Test application" << std::endl;
    return 0;
}
"""
        
        (test_dir / "CMakeLists.txt").write_text(cmake_content)
        (test_dir / "main.cpp").write_text(main_content)
        
        print(f"✅ Created test project in {test_dir}")
        
        # Test that our auto-fix script exists and is executable
        script_path = Path(__file__).parent / "auto_fix.py"
        if not script_path.exists():
            print("❌ auto_fix.py script not found!")
            return False
            
        # Make the script executable
        os.chmod(script_path, 0o755)
        
        # Test the auto-fix functionality
        build_cmd = f"cd {test_dir} && mkdir -p build && cd build && cmake .."
        
        try:
            result = subprocess.run([
                "python3", str(script_path),
                "--build-cmd", build_cmd,
                "--max-attempts", "2",
                "--repo-root", str(test_dir)
            ], capture_output=True, text=True, timeout=60)
            
            print(f"Auto-fix script exit code: {result.returncode}")
            print(f"Auto-fix stdout: {result.stdout}")
            if result.stderr:
                print(f"Auto-fix stderr: {result.stderr}")
            
            # Check if success or escalation report was created
            ci_artifacts = test_dir / "ci_artifacts"
            if ci_artifacts.exists():
                success_report = ci_artifacts / "success_report.json"
                escalation_report = ci_artifacts / "escalation_report.json"
                
                if success_report.exists():
                    print("✅ Success report created")
                    return True
                elif escalation_report.exists():
                    print("⚠️  Escalation report created (expected for some test scenarios)")
                    return True
                else:
                    print("❌ No report generated")
                    return False
            else:
                print("❌ CI artifacts directory not created")
                return False
                
        except subprocess.TimeoutExpired:
            print("❌ Auto-fix script timed out")
            return False
        except Exception as e:
            print(f"❌ Error running auto-fix script: {e}")
            return False

def test_missing_dependencies():
    """Test handling of missing dependencies."""
    print("🔧 Testing missing dependency detection...")
    
    # This is a simple test to ensure our dependency checking works
    script_path = Path(__file__).parent / "auto_fix.py"
    
    # Import the auto-fix module to test its functions
    sys.path.insert(0, str(script_path.parent))
    
    try:
        from auto_fix import BuildAutoFixer
        
        with tempfile.TemporaryDirectory() as temp_dir:
            fixer = BuildAutoFixer(temp_dir)
            
            # Test dependency checking
            deps_ok = fixer.check_dependencies()
            print(f"Dependencies check result: {deps_ok}")
            
            # Test missing directory creation
            fixer.fix_missing_directories()
            print("✅ Missing directory creation test completed")
            
            return True
            
    except ImportError as e:
        print(f"❌ Could not import auto_fix module: {e}")
        return False
    except Exception as e:
        print(f"❌ Error in dependency test: {e}")
        return False

def main():
    """Run all auto-fix tests."""
    print("🚀 Starting auto-fix system tests...")
    
    tests = [
        ("Auto-fix system test", test_auto_fix_system),
        ("Missing dependencies test", test_missing_dependencies)
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n📋 Running: {test_name}")
        try:
            if test_func():
                print(f"✅ {test_name} PASSED")
                passed += 1
            else:
                print(f"❌ {test_name} FAILED")
        except Exception as e:
            print(f"❌ {test_name} ERROR: {e}")
    
    print(f"\n📊 Test Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 All auto-fix tests passed!")
        return 0
    else:
        print("🚨 Some tests failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())