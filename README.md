# Password Manager

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
- No data is sent outside the user’s machine—entirely local by design

## Getting Started

### Prerequisites

- Qt6 framework
- C++17 or above
- SQL (e.g., SQLite) development environment/libraries

### Building

1. Clone the repository.
2. Open the project file with Qt Creator or configure your build toolchain for C++ with Qt6.
3. Make sure all required dependencies (Qt6, SQL library) are installed.
4. Build and run the project from your IDE or terminal.

### Usage

- Set up a master password on first launch. This is required to unlock and manage stored passwords.
- Add, view, update, and delete password records with intuitive UI controls.
- Backup or restore your encrypted password database from the application menu.

## Project Structure

- `/src` - C++ source code
- `/ui`  - Qt6 interface files
- `/db`  - SQL scripts and schema

## Encryption Details

- AES-256 is used for all sensitive data (site, login, password, notes)
- The master password is not stored; only its cryptographic hash and salt are saved
- Key derivation uses a secure method to prevent brute-force attacks
Since CMake is used to build your project, mention this clearly in the README and include a build section with specific CMake instructions. Here are the recommended additions and changes:

***

## Build Instructions

This project uses CMake for cross-platform configuration and build automation.

### Prerequisites

- CMake (version 3.16 or newer recommended)
- Qt6 (all required development libraries installed)
- A C++17-compliant compiler
- SQL library (like SQLite)
- Git (for cloning, optional)

### Building from Source

1. Clone this repository:
   ```bash
   git clone <your_repo_url>
   cd password-manager
   ```
2. Create a build directory and run CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```
   - You may need to set up the `CMAKE_PREFIX_PATH` to let CMake find your Qt6 installation, e.g.:
     ```bash
     cmake -DCMAKE_PREFIX_PATH=/path/to/Qt6/lib/cmake ..
     ```
3. Build the project:
   ```bash
   cmake --build .
   ```
4. Run the application:
   ```bash
   ./password-manager
   ```

For more details on building Qt6 CMake projects, see official [Qt documentation].[3]

***

Add this section to your README, ensuring users and collaborators can build your password manager easily using CMake and Qt6, regardless of OS.To accurately reflect that CMake is used for building, add a dedicated section to the README. Here’s how to integrate it:

***

## Building with CMake

This project uses CMake for cross-platform builds and configuration.

### Steps

1. Ensure CMake (≥ 3.16) and Qt6 (plus SQL libraries) are installed.
2. Clone the repository and open a terminal in the project directory.
   git clone <your_repo_url>
   cd password-manager

3. Create a build directory and run:
   ```sh
   cmake -B build -S .
   cmake --build build
   ```
   If Qt6 isn’t auto-detected, set its path:
   ```sh
   cmake -B build -S . -DCMAKE_PREFIX_PATH=/path/to/Qt6/lib/cmake
   ```
   mkdir build
   cd build
   cmake ..

4. Run the resulting application from the `build` directory.


## Disclaimer

- This Password Manager is for educational and personal use.
- Security depends on choosing a strong master password and keeping local files safe.

## License

MIT License (or include your chosen open-source license text)

***

This README provides a concise and informative overview that will serve users, collaborators, and reviewers alike, clearly highlighting the security-centric nature and local-first storage design of your project.[4][6][2]

[1](https://www.manageengine.com/products/passwordmanagerpro/readme.html)
[2](https://dev.to/zand/a-comprehensive-and-user-friendly-project-readmemd-template-2ei8)
[3](https://test-projectcalamp.readme.io/docs/password-manager-interface-1)
[4](https://sourceforge.net/projects/password-pusher.mirror/files/v1.61.1/README.md/download)
[5](https://www.youtube.com/watch?v=KQjf9get6PE)
[6](https://forum.djangoproject.com/t/passmanagerweb-a-password-manager-built-with-django/35665)
[7](https://dev.to/lito/self-hosted-password-manager-laravel-8-php-8-mysql-8-3bd7)
[8](https://test-projectcalamp.readme.io/docs/calamp-device-password-manager-phase-21)
[9](https://www.reddit.com/r/reactjs/comments/1fzl5fr/my_fullstack_password_manager_project_inspired_by/)
[10](https://readme.com/resources/1password-shell-plugin)
