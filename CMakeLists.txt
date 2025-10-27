cmake_minimum_required(VERSION 3.16)
project(StudentPicker VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    Sql
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/global.cpp
    src/core/logger.cpp
    src/core/userPreference.cpp
    src/core/DatabaseManager.cpp
    src/core/CSVReader.cpp
    src/core/XLSXReader.cpp
    src/core/ImageProcessor.cpp
    src/gui/MainWindow.cpp
    src/gui/StudentTableModel.cpp
)

# Header files
set(HEADERS
    src/core/global.hpp
    src/core/logger.hpp
    src/core/userPreference.hpp
    src/core/DatabaseManager.hpp
    src/core/CSVReader.hpp
    src/core/XLSXReader.hpp
    src/core/ImageProcessor.hpp
    src/gui/MainWindow.hpp
    src/gui/StudentTableModel.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    Qt6::Sql
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/gui
)

# Platform specific settings
if(WIN32)
    # Windows specific
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        WIN32_EXECUTABLE ON
    )
    
    # Copy Qt DLLs to output directory (for Windows)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::Core>
        $<TARGET_FILE:Qt6::Gui>
        $<TARGET_FILE:Qt6::Widgets>
        $<TARGET_FILE:Qt6::Sql>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    
elseif(APPLE)
    # macOS specific
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        MACOSX_BUNDLE ON
    )
    
elseif(UNIX)
    # Linux specific
    # No additional settings needed
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in DEBUG mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_DEBUG)
else()
    message(STATUS "Building in RELEASE mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_DEBUG_OUTPUT)
endif()

# Print configuration info
message(STATUS "==================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "==================================")
