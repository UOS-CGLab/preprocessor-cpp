cmake_minimum_required(VERSION 3.29)
project(preprocessor)

set(CMAKE_CXX_STANDARD 20)

# add mesh_files directory
include_directories("./mesh_files")

# add openmesh
find_package(OpenMesh REQUIRED)
include_directories("OpenMesh-11.0.0-mac-arm/include")

find_package(nlohmann_json REQUIRED)
include_directories("/usr/local/opt/nlohmann-json/include")

# Source files
file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} main.cpp ${SOURCES}
        include/to_json.h)

target_link_libraries(${PROJECT_NAME} OpenMeshCore OpenMeshTools)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

include_directories(${CMAKE_SOURCE_DIR}/include)

#set_target_properties(${PROJECT_NAME} PROPERTIES
#        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/"
#)
