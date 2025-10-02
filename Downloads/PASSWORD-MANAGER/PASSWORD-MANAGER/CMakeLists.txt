cmake_minimum_required(VERSION 3.16) # Qt 6 requires CMake 3.16+
project(PasswordManagerGUI VERSION 1.0 LANGUAGES CXX)

# --- Standard C++ Settings ---
set(CMAKE_CXX_STANDARD 17) # Qt 6 requires C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Compiler Warnings (Optional but Recommended) ---
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic) # Add more flags if desired
endif()

# --- Enable Qt 6 Features ---
set(CMAKE_AUTOMOC ON)   # Enable automatic Meta-Object Compiler processing
set(CMAKE_AUTOUIC ON)   # Enable automatic UI Compiler processing (if using .ui files later)
set(CMAKE_AUTORCC ON)   # Enable automatic Resource Compiler processing (if using .qrc files later)
set(CMAKE_INCLUDE_CURRENT_DIR ON) # Needed for AUTOMOC/AUTOUIC

# --- Find Packages ---
# Find Qt 6 core components
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)
message(STATUS "Found Qt Version: ${Qt6_VERSION}")

# Find OpenSSL (for the backend)
find_package(OpenSSL REQUIRED)
message(STATUS "Found OpenSSL Version: ${OPENSSL_VERSION}")

# --- Project Sources ---
set(SOURCES
    # Entry Point
    src/main.cpp
    # Core Logic
    src/Encryption.cpp
    src/PasswordEntry.cpp
    src/PasswordManager.cpp
    # GUI Components
    src/LoginDialog.cpp
    src/MainWindow.cpp
    src/AddEditDialog.cpp
)

# --- Executable Target ---
# Use WIN32 for GUI applications on Windows to suppress console window
add_executable(password_manager_gui WIN32 ${SOURCES})

# --- Include Directories ---
# Project sources and dependencies (Qt includes handled via target_link_libraries)
target_include_directories(password_manager_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# --- Linking ---
# Link against necessary libraries using CMake imported targets
target_link_libraries(password_manager_gui PRIVATE
    Qt6::Widgets        # Core Qt GUI library
    OpenSSL::SSL        # OpenSSL libraries
    OpenSSL::Crypto
)

# --- Installation (Optional) ---
# You would add rules here later to install the executable and necessary DLLs
# (Qt DLLs, OpenSSL DLLs, MinGW runtime DLLs) using tools like windeployqt.
# Example (very basic, needs refinement for deployment):
# install(TARGETS password_manager_gui DESTINATION bin)

message(STATUS "Target 'password_manager_gui' configured for Qt.")