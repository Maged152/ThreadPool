cmake_minimum_required(VERSION 3.22)
project(ThreadPool VERSION 0.0.1)

# Enable CTest and options only for the main project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(PARENT_PROJECT_NAME ${PROJECT_NAME})
    option(ThreadPool_BUILD_EXAMPLES "Enable examples" ON)
endif()

option(ThreadPool_BUILD_Doc "Build documentations" ON)

# Configuration
set(version_config "${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "qlm::")
set(INSTALL_INC_DIR "include")
set(INSTALL_CFG_DIR "cmake")

# Create the library
add_library(${PROJECT_NAME} INTERFACE)

set_target_properties(${PROJECT_NAME} PROPERTIES 
                        CXX_STANDARD 20
                        CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
)

add_library(${namespace}${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Check if examples are enabled
if(ThreadPool_BUILD_EXAMPLES)
    message(STATUS "Examples for ${PROJECT_NAME} are enabled")
    add_subdirectory(examples)
endif()

# Check if documentations are enabled
if(ThreadPool_BUILD_Doc)
    message(STATUS "documentations for ${PROJECT_NAME} are enabled")
    add_subdirectory(doc)
endif()

# Install the library
install(TARGETS ${PROJECT_NAME}
    EXPORT "${TARGETS_EXPORT_NAME}"
    INCLUDES DESTINATION ${INSTALL_INC_DIR}
)

install(EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${namespace}"
    DESTINATION "${INSTALL_CFG_DIR}"
)

install(
    DIRECTORY include/
    DESTINATION ${INSTALL_INC_DIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Create the package configuration file
include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${project_config}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${project_config}"
  INSTALL_DESTINATION "${INSTALL_CFG_DIR}"
)

# Write the library version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${version_config}"
  COMPATIBILITY AnyNewerVersion
)

# Install configuration and version files
install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/${version_config}"
    "${CMAKE_CURRENT_BINARY_DIR}/${project_config}"
    DESTINATION ${INSTALL_CFG_DIR}
)