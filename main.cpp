#include "LoginDialog.h"
#include "MainWindow.h"
#include "PasswordManager.h" 

// Qt Includes
#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory> 

// Standard Includes
#include <memory>
#include <stdexcept>
#include <iostream>

int main(int argc, char *argv[])
{
    // Create the Qt Application instance
    QApplication app(argc, argv);
    app.setApplicationName("PasswordManagerGUI");
    app.setOrganizationName("YourOrg"); // Optional

    // Optional: Set a style for a more modern look (like Fusion)
    // app.setStyle(QStyleFactory::create("Fusion"));

    std::unique_ptr<PasswordManager> manager = nullptr; // Will hold the manager instance

    // Loop: Show Login -> If OK -> Show Main -> If Lock -> Show Login again?
    // Simple version: Login once, then show main window. Exit if login cancelled.
    bool loginSuccessful = false;
    while(!loginSuccessful)
    {
        LoginDialog loginDialog;
        // loginDialog.setWindowIcon(QIcon(":/icons/appicon.png")); // Set icon if desired

        if (loginDialog.exec() == QDialog::Accepted) {
            manager = loginDialog.releasePasswordManager(); // Get manager instance
            if (manager) {
                loginSuccessful = true; // Break loop
            } else {
                // Should not happen if dialog logic is correct
                QMessageBox::critical(nullptr, "Fatal Error", "Login accepted, but password manager is invalid!");
                return 1;
            }
        } else {
            // User cancelled the login dialog
            return 0; // Exit application gracefully
        }
    }

    // If loop exited, login was successful
    MainWindow mainWindow(std::move(manager)); // Pass ownership to main window
    // mainWindow.setWindowIcon(QIcon(":/icons/appicon.png"));
    mainWindow.show(); // Show the main window

    // Start the Qt application event loop
    return app.exec();
}