# Password Manager Using Qt6

A complete password storing system built in C++ using Qt6 for the interface, SQL for persistent storage, and AES-256 encryption for ultimate security. All user data is sealed with strong encryption and stored **only on the local disk** for maximum protection against unauthorized access.

## Features

- Modern, cross-platform GUI with Qt6
- AES-256 encryption for storing sensitive information
- All data securely stored locally (no cloud or external transfers)
- SQL database backend for reliable storage and querying
- Simple, intuitive workflow for adding, viewing, updating, and deleting password records
- Encrypted backup and restore support

## Security

- Every password entry is encrypted with AES-256 before being written to disk
- The AES-256 encryption key is protected by a master password chosen by the user
- Even if the local data file is stolen, information remains protected due to strong encryption
- No data is sent outside the user's machine—entirely local by design

## Build Instructions

This project uses CMake for cross-platform configuration and build automation.

### Prerequisites

- CMake (version 3.16 or newer recommended)
- Qt6 (all required development libraries installed)
- A C++17-compliant compiler
- SQL library (like SQLite)

### Building from Source

1. Clone this repository:
   ```bash
   git clone https://github.com/lost-shubh/password-manager-using-qt-6.git
   cd password-manager-using-qt-6
   ```
2. Create a build directory and run CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```
3. Build the project:
   ```bash
   cmake --build .
   ```
4. Run the application:
   ```bash
   ./password_manager_gui
   ```

## Usage

- Set up a master password on first launch. This is required to unlock and manage stored passwords.
- Add, view, update, and delete password records with intuitive UI controls.
- Backup or restore your encrypted password database from the application menu.

## Project Structure

- `/src` - C++ source code
- `CMakeLists.txt` - Build configuration
- `build/` - Compiled binaries and build files

## Disclaimer

- This Password Manager is for educational and personal use.
- Security depends on choosing a strong master password and keeping local files safe.

## License

MIT License
