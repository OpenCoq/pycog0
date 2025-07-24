#!/usr/bin/env python3
"""
Create CMake config files for OpenCog components.
This script creates consistent CMake config files that can be used
by dependent modules to find installed libraries.
"""

import os
import sys
from pathlib import Path

def create_cmake_config(name, version, libraries=None, dependencies=None):
    """Create CMake config files for a component."""
    config_dir = Path(f"/usr/local/lib/cmake/{name}")
    config_dir.mkdir(parents=True, exist_ok=True)
    
    if libraries is None:
        libraries = [name.lower()]
    
    if dependencies is None:
        dependencies = []
    
    # Create the main config file
    config_content = f'''# {name}Config.cmake - Config file for {name}

# Set version information
set(PACKAGE_VERSION "{version}")
set({name}_VERSION "{version}")
set({name.upper()}_VERSION "{version}")

# Version compatibility check
set(PACKAGE_VERSION_EXACT FALSE)
set(PACKAGE_VERSION_COMPATIBLE TRUE)
set(PACKAGE_VERSION_UNSUITABLE FALSE)

# Set basic variables
set({name.upper()}_FOUND TRUE)
set({name}_FOUND TRUE)

# Set include directories
set({name.upper()}_INCLUDE_DIRS "/usr/local/include")
set({name}_INCLUDE_DIRS "/usr/local/include")

# Set library directories  
set({name.upper()}_LIBRARY_DIRS "/usr/local/lib/opencog")
set({name}_LIBRARY_DIRS "/usr/local/lib/opencog")

# Find libraries
'''
    
    # Add library finding logic
    for lib in libraries:
        config_content += f'''find_library({name.upper()}_{lib.upper()}_LIBRARY
    NAMES {lib}
    PATHS /usr/local/lib/opencog
    NO_DEFAULT_PATH
)
'''
    
    # Set library variables
    if len(libraries) == 1:
        config_content += f'''
set({name.upper()}_LIBRARIES ${{{name.upper()}_{libraries[0].upper()}_LIBRARY}})
set({name}_LIBRARIES ${{{name.upper()}_{libraries[0].upper()}_LIBRARY}})
'''
    else:
        lib_vars = " ".join([f"${{{name.upper()}_{lib.upper()}_LIBRARY}}" for lib in libraries])
        config_content += f'''
set({name.upper()}_LIBRARIES {lib_vars})
set({name}_LIBRARIES {lib_vars})
'''
    
    # Add dependencies
    for dep in dependencies:
        config_content += f'''find_package({dep} REQUIRED)
'''
    
    config_content += f'''
# Set data directory
set({name.upper()}_DATA_DIR "/usr/local/share/opencog")
set({name}_DATA_DIR "/usr/local/share/opencog")

# Mark as found
set({name.upper()}_FOUND TRUE)
set({name}_FOUND TRUE)

message(STATUS "Found {name}: ${{{name.upper()}_LIBRARIES}}")
'''
    
    # Write config file
    config_file = config_dir / f"{name}Config.cmake"
    config_file.write_text(config_content)
    
    # Create version file
    version_content = f'''# {name}ConfigVersion.cmake - Version file for {name}

set(PACKAGE_VERSION "{version}")

# Check whether the requested PACKAGE_FIND_VERSION is compatible
if("${{PACKAGE_VERSION}}" VERSION_LESS "${{PACKAGE_FIND_VERSION}}")
  set(PACKAGE_VERSION_COMPATIBLE FALSE)
else()
  set(PACKAGE_VERSION_COMPATIBLE TRUE)
  if ("${{PACKAGE_VERSION}}" VERSION_EQUAL "${{PACKAGE_FIND_VERSION}}")
    set(PACKAGE_VERSION_EXACT TRUE)
  endif()
endif()
'''
    
    version_file = config_dir / f"{name}ConfigVersion.cmake"
    version_file.write_text(version_content)
    
    print(f"Created CMake config for {name} version {version}")

def main():
    """Create CMake configs for common OpenCog components."""
    
    # AtomSpace with its multiple libraries
    create_cmake_config(
        "AtomSpace", 
        "5.0.3",
        libraries=["atom_types", "value", "atombase", "atomcore", "atomflow", "truthvalue", "clearbox", "execution", "pattern", "query-engine", "smob", "atomspace"],
        dependencies=["CogUtil"]
    )
    
    # URE
    create_cmake_config("URE", "1.0.0", dependencies=["AtomSpace"])
    
    # Moses  
    create_cmake_config("MOSES", "3.6.8", dependencies=["CogUtil"])
    
    # CogServer
    create_cmake_config("CogServer", "0.1.4", libraries=["server", "network"], dependencies=["AtomSpace"])
    
    # AtomSpaceStorage
    create_cmake_config("AtomSpaceStorage", "1.0.0", libraries=["persist", "sexpr", "csv"], dependencies=["AtomSpace"])
    
    # AtomSpaceRocks
    create_cmake_config("AtomSpaceRocks", "1.3.0", libraries=["persist-rocks"], dependencies=["AtomSpace", "AtomSpaceStorage"])
    
    print("All CMake configs created successfully")

if __name__ == "__main__":
    main()