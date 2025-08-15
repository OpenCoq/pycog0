cmake_minimum_required(VERSION 3.16)
project(agents VERSION 0.1.0 DESCRIPTION "OpenCog Agents Project")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Configuring OpenCog Agents Project")

# Add C++ Agent-Zero implementation
add_subdirectory(cpp)

# Future: Add other agent implementations here
# add_subdirectory(python)  # For Python-based agents
# add_subdirectory(scheme)  # For pure Scheme agents

message(STATUS "OpenCog Agents configuration complete")