# Agent-Zero C++ Implementation for OpenCog Integration
# Part of the AGENT-ZERO-GENESIS project

cmake_minimum_required(VERSION 3.16)

project(AgentZeroCpp
    VERSION 0.1.0
    DESCRIPTION "C++ Agent-Zero Implementation Optimized for OpenCog Integration"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Building Agent-Zero C++ for OpenCog Integration")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Global compiler settings
add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
else()
    add_compile_options(-O3 -DNDEBUG)
endif()

# Find required OpenCog dependencies
find_package(PkgConfig REQUIRED)

# Core OpenCog dependencies (required for all modules)
# These packages must be installed from the OpenCog ecosystem
pkg_check_modules(COGUTIL cogutil)
pkg_check_modules(ATOMSPACE atomspace)  
pkg_check_modules(COGSERVER cogserver)

if(NOT COGUTIL_FOUND)
    message(FATAL_ERROR "\n"
        "OpenCog dependency 'cogutil' not found.\n"
        "Please install the OpenCog ecosystem first:\n"
        "  See: https://github.com/opencog/cogutil\n"
        "  Or use the OpenCog Dependency Build workflow\n"
        "  See: docs/OPENCOG_DEPENDENCY_BUILD.md\n")
endif()

if(NOT ATOMSPACE_FOUND)
    message(FATAL_ERROR "\n"
        "OpenCog dependency 'atomspace' not found.\n" 
        "Please install the OpenCog ecosystem first:\n"
        "  See: https://github.com/opencog/atomspace\n"
        "  Or use the OpenCog Dependency Build workflow\n")
endif()

if(NOT COGSERVER_FOUND)
    message(FATAL_ERROR "\n"
        "OpenCog dependency 'cogserver' not found.\n"
        "Please install the OpenCog ecosystem first:\n" 
        "  See: https://github.com/opencog/cogserver\n"
        "  Or use the OpenCog Dependency Build workflow\n")
endif()

# Optional OpenCog dependencies (for specific modules)
pkg_check_modules(PLN pln)
pkg_check_modules(URE ure)
pkg_check_modules(MINER miner)
pkg_check_modules(MOSES moses)
pkg_check_modules(ATTENTION attention)
pkg_check_modules(SPACETIME spacetime)
pkg_check_modules(SENSORY sensory)
pkg_check_modules(LG_ATOMESE lg-atomese)

# Report found dependencies
message(STATUS "OpenCog Dependencies Found:")
message(STATUS "  cogutil: ${COGUTIL_VERSION}")
message(STATUS "  atomspace: ${ATOMSPACE_VERSION}")
message(STATUS "  cogserver: ${COGSERVER_VERSION}")

if(PLN_FOUND)
    message(STATUS "  pln: ${PLN_VERSION}")
endif()

if(URE_FOUND)
    message(STATUS "  ure: ${URE_VERSION}")
endif()

if(ATTENTION_FOUND)
    message(STATUS "  attention: ${ATTENTION_VERSION}")
endif()

# Global include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${COGUTIL_INCLUDE_DIRS}
    ${ATOMSPACE_INCLUDE_DIRS}
    ${COGSERVER_INCLUDE_DIRS}
)

# Global link directories
link_directories(
    ${COGUTIL_LIBRARY_DIRS}
    ${ATOMSPACE_LIBRARY_DIRS}
    ${COGSERVER_LIBRARY_DIRS}
)

# Add subdirectories for each Agent-Zero module
# These are added in dependency order

# Phase 1: Foundation Layer
add_subdirectory(agentzero-core)

# Phase 2: Perception & Action (depends on core)
if(SENSORY_FOUND)
    add_subdirectory(agentzero-perception)
else()
    message(STATUS "Skipping agentzero-perception: sensory dependency not found")
endif()

# Phase 3: Knowledge & Reasoning (depends on core)
if(PLN_FOUND AND URE_FOUND)
    add_subdirectory(agentzero-knowledge)
else()
    message(STATUS "Skipping agentzero-knowledge: PLN/URE dependencies not found")
endif()

# Phase 4: Planning & Goals (depends on core)
if(SPACETIME_FOUND)
    add_subdirectory(agentzero-planning)
else()
    message(STATUS "Skipping agentzero-planning: spacetime dependency not found")
endif()

# Phase 5: Learning & Adaptation (depends on core)
if(MOSES_FOUND)
    add_subdirectory(agentzero-learning)
else()
    message(STATUS "Skipping agentzero-learning: MOSES dependency not found")
endif()

# Phase 6: Communication & NLP (depends on core)
if(LG_ATOMESE_FOUND)
    add_subdirectory(agentzero-communication)
else()
    message(STATUS "Skipping agentzero-communication: lg-atomese dependency not found")
endif()

# Phase 7: Memory & Context (depends on core)
if(ATTENTION_FOUND)
    add_subdirectory(agentzero-memory)
else()
    message(STATUS "Skipping agentzero-memory: attention dependency not found")
endif()

# Phase 8: Tool Integration (depends on core)
add_subdirectory(agentzero-tools)

# Examples and tests
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    else()
        message(STATUS "Doxygen not found, documentation will not be built")
    endif()
endif()

# Package configuration
include(GNUInstallDirs)

# Create package config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AgentZeroCppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/AgentZeroCppConfig.cmake"
    @ONLY
)

# Installation
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/AgentZeroCppConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AgentZeroCpp
)

# Status summary
message(STATUS "")
message(STATUS "Agent-Zero C++ Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build testing: ${BUILD_TESTING}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build documentation: ${BUILD_DOCS}")
message(STATUS "")
message(STATUS "This is part of the AGENT-ZERO-GENESIS project.")
message(STATUS "See AGENT-ZERO-GENESIS.md for complete roadmap and development tasks.")
message(STATUS "")