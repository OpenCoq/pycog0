#
# OpenCog Organization Repository - Unified CMake Build System
#
# This CMakeLists.txt provides a unified build system for the entire OpenCog
# ecosystem. It identifies available components and provides a centralized
# way to build them, either individually or collectively.
#
# Components in this repository are designed to be built independently,
# each with their own dependencies. This unified build system provides:
# - Discovery of available components  
# - Individual component build targets
# - Collective build coordination
# - Clear reporting of what can be built
#
# Usage:
#   mkdir build && cd build
#   cmake ..                           # Configure and discover components
#   make help                          # See available targets
#   make <component>                   # Build specific component
#   make all-components               # Attempt to build all available components
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

# Python venv seems to need this
IF(CMAKE_VERSION VERSION_GREATER 3.31)
   CMAKE_POLICY(SET CMP0177 NEW)
ENDIF(CMAKE_VERSION VERSION_GREATER 3.31)

# Fix for modern CMake Boost module policy
IF(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    CMAKE_POLICY(SET CMP0167 OLD)
ENDIF()

PROJECT(opencog-org)

# ----------------------------------------------------------
# Build configuration
#

# Default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Global definitions
ADD_DEFINITIONS(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
                -DPROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}")

# ----------------------------------------------------------
# Component discovery and target creation
#

# Initialize tracking lists
SET(AVAILABLE_COMPONENTS "")
SET(MISSING_COMPONENTS "")

# Function to check if a component is available and create targets for it
FUNCTION(REGISTER_OPENCOG_COMPONENT component_name component_path layer)
    SET(component_full_path "${CMAKE_SOURCE_DIR}/${component_path}")
    
    IF(EXISTS "${component_full_path}/CMakeLists.txt")
        LIST(APPEND AVAILABLE_COMPONENTS "${component_name}")
        SET(AVAILABLE_COMPONENTS ${AVAILABLE_COMPONENTS} PARENT_SCOPE)
        
        # Create a target for building this component individually
        ADD_CUSTOM_TARGET(${component_name}
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/${component_name}-build
            COMMENT "Building ${component_name} component"
            VERBATIM
        )
        
        # Create a target for configuring this component
        ADD_CUSTOM_TARGET(configure-${component_name}
            COMMAND ${CMAKE_COMMAND} -S ${component_full_path} -B ${CMAKE_BINARY_DIR}/${component_name}-build
            COMMENT "Configuring ${component_name} component"
            VERBATIM
        )
        
        # Create dependency between build and configure
        ADD_DEPENDENCIES(${component_name} configure-${component_name})
        
        MESSAGE(STATUS "✓ Registered component: ${component_name} (${layer} layer)")
    ELSE()
        LIST(APPEND MISSING_COMPONENTS "${component_name}")
        SET(MISSING_COMPONENTS ${MISSING_COMPONENTS} PARENT_SCOPE)
        MESSAGE(STATUS "⚠ Component not available: ${component_name} (no CMakeLists.txt)")
    ENDIF()
ENDFUNCTION()

# ----------------------------------------------------------
# Register components by dependency layer
#

MESSAGE(STATUS "")
MESSAGE(STATUS "=============================================================")
MESSAGE(STATUS "OpenCog Organization Repository - Component Discovery")
MESSAGE(STATUS "=============================================================")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Foundation Layer Components ---")
REGISTER_OPENCOG_COMPONENT("cogutil" "cogutil" "Foundation")
REGISTER_OPENCOG_COMPONENT("moses" "moses" "Foundation") 
REGISTER_OPENCOG_COMPONENT("blender_api_msgs" "blender_api_msgs" "Foundation")
REGISTER_OPENCOG_COMPONENT("external-tools" "external-tools" "Foundation")
REGISTER_OPENCOG_COMPONENT("ocpkg" "ocpkg" "Foundation")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Core Layer Components ---")
REGISTER_OPENCOG_COMPONENT("atomspace" "atomspace" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-rocks" "atomspace-rocks" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-pgres" "atomspace-pgres" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-ipfs" "atomspace-ipfs" "Core") 
REGISTER_OPENCOG_COMPONENT("atomspace-websockets" "atomspace-websockets" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-restful" "atomspace-restful" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-bridge" "atomspace-bridge" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-metta" "atomspace-metta" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-rpc" "atomspace-rpc" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-cog" "atomspace-cog" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-agents" "atomspace-agents" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-dht" "atomspace-dht" "Core")
REGISTER_OPENCOG_COMPONENT("atomspace-storage" "atomspace-storage" "Core")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Logic Layer Components ---")
REGISTER_OPENCOG_COMPONENT("unify" "unify" "Logic")
REGISTER_OPENCOG_COMPONENT("ure" "ure" "Logic")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Cognitive Layer Components ---")
REGISTER_OPENCOG_COMPONENT("cogserver" "cogserver" "Cognitive")
REGISTER_OPENCOG_COMPONENT("attention" "attention" "Cognitive")
REGISTER_OPENCOG_COMPONENT("spacetime" "spacetime" "Cognitive")
REGISTER_OPENCOG_COMPONENT("pattern-index" "pattern-index" "Cognitive")
REGISTER_OPENCOG_COMPONENT("dimensional-embedding" "dimensional-embedding" "Cognitive")
REGISTER_OPENCOG_COMPONENT("profile" "profile" "Cognitive")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Advanced Layer Components ---")
REGISTER_OPENCOG_COMPONENT("pln" "pln" "Advanced")
REGISTER_OPENCOG_COMPONENT("miner" "miner" "Advanced")
REGISTER_OPENCOG_COMPONENT("asmoses" "asmoses" "Advanced")
REGISTER_OPENCOG_COMPONENT("benchmark" "benchmark" "Advanced")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Learning Layer Components ---")
REGISTER_OPENCOG_COMPONENT("learn" "learn" "Learning")
REGISTER_OPENCOG_COMPONENT("generate" "generate" "Learning")
REGISTER_OPENCOG_COMPONENT("language-learning" "language-learning" "Learning")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Language Layer Components ---")
REGISTER_OPENCOG_COMPONENT("lg-atomese" "lg-atomese" "Language")
REGISTER_OPENCOG_COMPONENT("relex" "relex" "Language")
REGISTER_OPENCOG_COMPONENT("link-grammar" "link-grammar" "Language")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Robotics Layer Components ---")
REGISTER_OPENCOG_COMPONENT("vision" "vision" "Robotics")
REGISTER_OPENCOG_COMPONENT("perception" "perception" "Robotics") 
REGISTER_OPENCOG_COMPONENT("sensory" "sensory" "Robotics")
REGISTER_OPENCOG_COMPONENT("ros-behavior-scripting" "ros-behavior-scripting" "Robotics")
REGISTER_OPENCOG_COMPONENT("robots_config" "robots_config" "Robotics")
REGISTER_OPENCOG_COMPONENT("pau2motors" "pau2motors" "Robotics")
REGISTER_OPENCOG_COMPONENT("motor" "motor" "Robotics")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Integration Layer Components ---")
REGISTER_OPENCOG_COMPONENT("opencog" "opencog" "Integration")
REGISTER_OPENCOG_COMPONENT("TinyCog" "TinyCog" "Integration")

MESSAGE(STATUS "")
MESSAGE(STATUS "--- Discovering Specialized Components ---")
REGISTER_OPENCOG_COMPONENT("visualization" "visualization" "Specialized")
REGISTER_OPENCOG_COMPONENT("cheminformatics" "cheminformatics" "Specialized")
REGISTER_OPENCOG_COMPONENT("agi-bio" "agi-bio" "Specialized")
REGISTER_OPENCOG_COMPONENT("ghost_bridge" "ghost_bridge" "Specialized")
REGISTER_OPENCOG_COMPONENT("matrix" "matrix" "Specialized")
REGISTER_OPENCOG_COMPONENT("python-attic" "python-attic" "Specialized")
REGISTER_OPENCOG_COMPONENT("atomese-simd" "atomese-simd" "Specialized")

# ----------------------------------------------------------
# Collective build targets
#

# Target to configure all available components
ADD_CUSTOM_TARGET(configure-all
    COMMENT "Configuring all available components..."
)

# Target to build all available components  
ADD_CUSTOM_TARGET(all-components
    COMMENT "Building all available components..."
)

# Add dependencies for collective targets
FOREACH(component ${AVAILABLE_COMPONENTS})
    ADD_DEPENDENCIES(configure-all configure-${component})
    ADD_DEPENDENCIES(all-components ${component})
ENDFOREACH()

# Layer-specific targets
FOREACH(layer Foundation Core Logic Cognitive Advanced Learning Language Robotics Integration Specialized)
    ADD_CUSTOM_TARGET(${layer}-layer
        COMMENT "Building all ${layer} layer components..."
    )
ENDFOREACH()

# ----------------------------------------------------------
# Documentation and utility targets
#

# Create a help file at configure time
SET(HELP_FILE "${CMAKE_BINARY_DIR}/help.txt")
FILE(WRITE ${HELP_FILE} "Available OpenCog components:\n")
FILE(APPEND ${HELP_FILE} "Individual components:\n")

FOREACH(component ${AVAILABLE_COMPONENTS})
    FILE(APPEND ${HELP_FILE} "  make ${component}\n")
ENDFOREACH()

FILE(APPEND ${HELP_FILE} "\nCollective targets:\n")
FILE(APPEND ${HELP_FILE} "  make configure-all   # Configure all components\n")
FILE(APPEND ${HELP_FILE} "  make all-components  # Build all components\n")
FILE(APPEND ${HELP_FILE} "  make Foundation-layer  # Build foundation components\n")
FILE(APPEND ${HELP_FILE} "  make Core-layer      # Build core components\n")
FILE(APPEND ${HELP_FILE} "  make list-components # Show this help\n")

# Create a target that shows available build targets
ADD_CUSTOM_TARGET(list-components
    COMMAND ${CMAKE_COMMAND} -E cat ${HELP_FILE}
    VERBATIM
)

# ----------------------------------------------------------
# Build Summary
#

# Function to print a list with proper formatting
FUNCTION(PRINT_COMPONENT_LIST list_name component_list)
    LIST(LENGTH ${component_list} list_length)
    IF(list_length GREATER 0)
        MESSAGE(STATUS "${list_name} (${list_length}):")
        FOREACH(component ${${component_list}})
            MESSAGE(STATUS "  - ${component}")
        ENDFOREACH()
    ELSE()
        MESSAGE(STATUS "${list_name}: None")
    ENDIF()
ENDFUNCTION()

MESSAGE(STATUS "")
MESSAGE(STATUS "=============================================================")
MESSAGE(STATUS "OpenCog Unified Build System - Discovery Summary")
MESSAGE(STATUS "=============================================================")

PRINT_COMPONENT_LIST("Available Components" AVAILABLE_COMPONENTS)
MESSAGE(STATUS "")
PRINT_COMPONENT_LIST("Missing Components (no CMakeLists.txt)" MISSING_COMPONENTS)

LIST(LENGTH AVAILABLE_COMPONENTS num_available)
LIST(LENGTH MISSING_COMPONENTS num_missing)
MATH(EXPR total_components "${num_available} + ${num_missing}")

MESSAGE(STATUS "")
MESSAGE(STATUS "Discovery Statistics:")
MESSAGE(STATUS "  Total components checked: ${total_components}")
MESSAGE(STATUS "  Available for building: ${num_available}")
MESSAGE(STATUS "  Missing CMakeLists.txt: ${num_missing}")

IF(num_available GREATER 0)
    MATH(EXPR availability_rate "${num_available} * 100 / ${total_components}")
    MESSAGE(STATUS "  Availability rate: ${availability_rate}% (${num_available}/${total_components})")
ENDIF()

MESSAGE(STATUS "")
MESSAGE(STATUS "Quick Start:")
MESSAGE(STATUS "  make list-components   # Show all available build targets")
MESSAGE(STATUS "  make <component>       # Build individual component") 
MESSAGE(STATUS "  make configure-all     # Configure all components")
MESSAGE(STATUS "  make all-components    # Build all available components")
MESSAGE(STATUS "  make Foundation-layer  # Build foundation layer")
MESSAGE(STATUS "")
MESSAGE(STATUS "Note: Individual components are built in their own build")
MESSAGE(STATUS "directories to avoid conflicts and dependency issues.")
MESSAGE(STATUS "=============================================================")