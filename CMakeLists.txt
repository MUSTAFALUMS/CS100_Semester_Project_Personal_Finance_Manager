cmake_minimum_required(VERSION 3.14)
project(PersonalFinanceManager VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Set source files
set(SOURCES
    src/main.cpp
    src/GUI/Window.cpp
    src/GUI/Button.cpp
    src/GUI/TextInput.cpp
    src/Core/User.cpp
    src/Core/Transaction.cpp
    src/Core/Category.cpp
    src/Core/Budget.cpp
    src/Database/DatabaseManager.cpp
)

# Set header files
set(HEADERS
    include/GUI/Window.hpp
    include/GUI/Button.hpp
    include/GUI/TextInput.hpp
    include/Core/User.hpp
    include/Core/Transaction.hpp
    include/Core/Category.hpp
    include/Core/Budget.hpp
    include/Database/DatabaseManager.hpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link SFML
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Copy SFML DLLs for Windows
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()