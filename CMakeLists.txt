cmake_minimum_required(VERSION 3.28.6)
project(preprocessor)

set(CMAKE_CXX_STANDARD 20)

# Add mesh_files directory
include_directories("etc/mesh_files")

# Add OpenMesh and JSON libraries
find_package(OpenMesh REQUIRED)
find_package(nlohmann_json REQUIRED)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    message("x86_64")
    include_directories("OpenMesh-11.0.0-mac-intel/include")
    include_directories("/usr/local/Cellar/nlohmann-json/3.11.3/include")

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    message("arm64")
    include_directories("OpenMesh-11.0.0-mac-arm/include")
    include_directories("/usr/local/opt/nlohmann-json/include")
endif()

# Source files
file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} OpenMeshCore OpenMeshTools)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Copy the etc directory to the build directory
#add_custom_command(
#        TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E echo "Copying etc directory..."
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/etc ${CMAKE_BINARY_DIR}/etc
#)

# Create a custom target for copying the directory
#add_custom_target(copy_etc ALL
#        COMMAND ${CMAKE_COMMAND} -E echo "Copying etc directory..."
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/etc ${CMAKE_BINARY_DIR}/etc
#)
#
## Ensure the custom target is run after the main target is built
#add_dependencies(${PROJECT_NAME} copy_etc)
