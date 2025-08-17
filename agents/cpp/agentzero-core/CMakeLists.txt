# Agent-Zero Core Module CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

project(AgentZeroCore
    VERSION 0.1.0
    DESCRIPTION "Agent-Zero Core Orchestration Engine for OpenCog Integration"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required OpenCog packages
find_package(PkgConfig REQUIRED)

# Find cogutil
pkg_check_modules(COGUTIL REQUIRED cogutil)
if (COGUTIL_FOUND)
    message(STATUS "Found cogutil version ${COGUTIL_VERSION}")
    include_directories(${COGUTIL_INCLUDE_DIRS})
    link_directories(${COGUTIL_LIBRARY_DIRS})
endif()

# Find atomspace  
pkg_check_modules(ATOMSPACE REQUIRED atomspace)
if (ATOMSPACE_FOUND)
    message(STATUS "Found atomspace version ${ATOMSPACE_VERSION}")
    include_directories(${ATOMSPACE_INCLUDE_DIRS})
    link_directories(${ATOMSPACE_LIBRARY_DIRS})
endif()

# Find cogserver
pkg_check_modules(COGSERVER REQUIRED cogserver)
if (COGSERVER_FOUND)
    message(STATUS "Found cogserver version ${COGSERVER_VERSION}")
    include_directories(${COGSERVER_INCLUDE_DIRS})
    link_directories(${COGSERVER_LIBRARY_DIRS})
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(AGENTZERO_CORE_SOURCES
    src/AgentZeroCore.cpp
    src/CognitiveLoop.cpp  
    src/TaskManager.cpp
    src/KnowledgeIntegrator.cpp
)

# Header files
set(AGENTZERO_CORE_HEADERS
    include/opencog/agentzero/AgentZeroCore.h
    include/opencog/agentzero/CognitiveLoop.h
    include/opencog/agentzero/TaskManager.h
    include/opencog/agentzero/KnowledgeIntegrator.h
)

# Create library (when source files exist)
if(AGENTZERO_CORE_SOURCES)
    add_library(agentzero-core SHARED
        ${AGENTZERO_CORE_SOURCES}
        ${AGENTZERO_CORE_HEADERS}
    )

    target_link_libraries(agentzero-core
        ${COGUTIL_LIBRARIES}
        ${ATOMSPACE_LIBRARIES}
        ${COGSERVER_LIBRARIES}
    )

    target_include_directories(agentzero-core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    # Installation
    install(TARGETS agentzero-core
        EXPORT AgentZeroCoreTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )

    install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
    )
endif()

# Tests (when implemented)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Status message
message(STATUS "Agent-Zero Core module configuration complete")
message(STATUS "Source files implemented - ready for build and testing")