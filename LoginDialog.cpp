#include "LoginDialog.h"
#include "PasswordManager.h" 

// Qt Includes
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtGui/QIcon>
#include <QtCore/QString>

// Standard Library Includes
#include <string>
#include <memory>
#include <stdexcept>
#include <fstream>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Password Manager - Login");
    // setWindowIcon(QIcon(":/path/to/icon.png"));

    passwordLabel = new QLabel("Enter Master Password:", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);

    statusLabel = new QLabel("", this);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setStyleSheet("#statusLabel { color: gray; }");

    loginButton = new QPushButton("Unlock / Initialize", this);
    cancelButton = new QPushButton("Cancel", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(statusLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(loginButton);
    mainLayout->addLayout(buttonLayout);

    std::ifstream fileCheck(filename);
    if (!fileCheck.good()) {
        statusLabel->setText("Store file not found. New store will be created.");
        statusLabel->setStyleSheet("#statusLabel { color: blue; }");
    }
    fileCheck.close();

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);

    passwordEdit->setFocus();
    setMinimumWidth(350);
}

// Destructor definition moved here to ensure full PasswordManager type is known
// when unique_ptr destructor code is generated.
LoginDialog::~LoginDialog() = default;

void LoginDialog::attemptLogin() {
    std::string password = passwordEdit->text().toStdString();
    if (password.empty()) {
        QMessageBox::warning(this, "Login Failed", "Master password cannot be empty.");
        return;
    }
    try {
        passwordManager = std::make_unique<PasswordManager>(password, filename);
        accept();
    } catch (const std::exception& e) {
        passwordManager.reset();
        QMessageBox::critical(this, "Login/Initialization Failed", QString::fromStdString(e.what()));
        passwordEdit->selectAll();
        passwordEdit->setFocus();
    }
}

std::unique_ptr<PasswordManager> LoginDialog::releasePasswordManager() {
    return std::move(passwordManager);
}