#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <string>
#include <memory>

#include "PasswordManager.h" 

// Forward Declarations for Qt types are still okay
QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

// class PasswordManager; // Can remove forward declaration now

/** @brief Dialog for master password entry and manager initialization. */
class LoginDialog : public QDialog
{
    Q_OBJECT // Enable signals/slots

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override; // Keep declaration here

    // Method to retrieve the successfully initialized manager
    std::unique_ptr<PasswordManager> releasePasswordManager();

private slots:
    void attemptLogin(); // Slot to handle login button click / enter press

private:
    // UI elements
    QLabel *passwordLabel = nullptr;
    QLineEdit *passwordEdit = nullptr;
    QPushButton *loginButton = nullptr;
    QPushButton *cancelButton = nullptr;
    QLabel *statusLabel = nullptr;

    // Data
    const std::string filename = "passwords.dat"; // Default filename
    std::unique_ptr<PasswordManager> passwordManager = nullptr; // To hold the instance
};

#endif // LOGINDIALOG_H