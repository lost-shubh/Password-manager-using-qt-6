#include "AddEditDialog.h"
#include "PasswordManager.h"
#include "Encryption.h"     

// Qt Includes
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox> // <<<--- ADDED INCLUDE for confirmation dialog
#include <QtCore/QString>

// Standard Includes
#include <string>
#include <random>
#include <vector>   // Required for password char set construction if improved
#include <stdexcept>// Required for password generation exceptions if added

// Common UI setup
void AddEditDialog::setupUi() {
    titleLineEdit = new QLineEdit(this);
    usernameLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    urlLineEdit = new QLineEdit(this);
    urlLineEdit->setPlaceholderText("Optional: https://example.com");
    notesTextEdit = new QTextEdit(this);
    notesTextEdit->setAcceptRichText(false);
    notesTextEdit->setPlaceholderText("Optional notes");

    generatePasswordButton = new QPushButton("Generate", this);
    showPasswordButton = new QPushButton("Show", this);
    showPasswordButton->setCheckable(true);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    okButton = buttonBox->button(QDialogButtonBox::Ok);

    // Layouts
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Title:", titleLineEdit);
    formLayout->addRow("Username:", usernameLineEdit);
    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget(passwordLineEdit, 1);
    passwordLayout->addWidget(showPasswordButton);
    passwordLayout->addWidget(generatePasswordButton);
    formLayout->addRow("Password:", passwordLayout);
    formLayout->addRow("URL:", urlLineEdit);
    formLayout->addRow("Notes:", notesTextEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    // Connections
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(generatePasswordButton, &QPushButton::clicked, this, &AddEditDialog::generateAndFillPassword);
    connect(showPasswordButton, &QPushButton::toggled, this, &AddEditDialog::togglePasswordVisibility);
    connect(titleLineEdit, &QLineEdit::textChanged, this, &AddEditDialog::validateData);
    connect(passwordLineEdit, &QLineEdit::textChanged, this, &AddEditDialog::validateData);

    validateData(); // Initial validation check
}

// Constructor for Add mode
AddEditDialog::AddEditDialog(QWidget *parent)
    : QDialog(parent), isEditMode(false)
{
    setupUi();
    setWindowTitle("Add New Entry");
    titleLineEdit->setFocus();
}

// Constructor for Edit mode
AddEditDialog::AddEditDialog(const PasswordEntry* entryToEdit, QWidget *parent)
     : QDialog(parent), isEditMode(true)
{
    setupUi();
    setWindowTitle("Edit Entry");
    if (entryToEdit) { // Pre-fill fields
        titleLineEdit->setText(QString::fromStdString(entryToEdit->title));
        usernameLineEdit->setText(QString::fromStdString(entryToEdit->username));
        passwordLineEdit->setText(QString::fromStdString(entryToEdit->password));
        urlLineEdit->setText(QString::fromStdString(entryToEdit->url));
        notesTextEdit->setPlainText(QString::fromStdString(entryToEdit->notes)); // Use setPlainText
    }
    validateData(); // Validate pre-filled data
    titleLineEdit->setFocus();
}

AddEditDialog::~AddEditDialog() = default;

// Gathers data from fields into a PasswordEntry object
PasswordEntry AddEditDialog::getEntryData() const {
    PasswordEntry entry;
    entry.title = titleLineEdit->text().toStdString();
    entry.username = usernameLineEdit->text().toStdString();
    entry.password = passwordLineEdit->text().toStdString();
    entry.url = urlLineEdit->text().toStdString();
    entry.notes = notesTextEdit->toPlainText().toStdString();
    return entry;
}

// Generates and fills password *after confirming overwrite if necessary*
void AddEditDialog::generateAndFillPassword() {
    // --- Confirmation Logic Added ---
    QString currentPassword = passwordLineEdit->text();
    if (!currentPassword.isEmpty()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, // Parent
                                      "Confirm Overwrite", // Title
                                      "The password field is not empty.\n"
                                      "Generating a new password will replace the current content.\n\n"
                                      "Do you wish to continue?", // Message
                                      QMessageBox::Yes | QMessageBox::No, // Buttons
                                      QMessageBox::No); // Default button is No

        if (reply == QMessageBox::No) {
            return; // User clicked No, so do nothing
        }
        // User clicked Yes, proceed...
    }
    // --- End Confirmation Logic ---


    // --- Password Generation (simple internal version) ---
    // TODO: Consider adding options (length, chars) via a small popup or controls
    // TODO: Could call manager->generateRandomPassword if manager reference was passed
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{}|;:,.<>?";
    int length = 16; // Default length
    std::string password = "";
    if (chars.empty()) { // Basic check
        QMessageBox::critical(this, "Error", "Character set for password generation is empty!");
        return;
    }
    password.reserve(static_cast<size_t>(length));
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, static_cast<int>(chars.size()) - 1);
    for (int i = 0; i < length; ++i) {
        password += chars[static_cast<size_t>(distribution(generator))];
    }
    // --- End Password Generation ---

    // Update the password field
    passwordLineEdit->setText(QString::fromStdString(password));

    // Optional: Notify user password was generated
    // QMessageBox::information(this, "Password Generated", "New password placed in field.");
}

// Toggles password field visibility
void AddEditDialog::togglePasswordVisibility(bool checked) {
    if (checked) {
        passwordLineEdit->setEchoMode(QLineEdit::Normal);
        showPasswordButton->setText("Hide");
    } else {
        passwordLineEdit->setEchoMode(QLineEdit::Password);
        showPasswordButton->setText("Show");
    }
}

// Enables/disables the OK button based on required fields (Title and Password)
void AddEditDialog::validateData() {
    bool titleOk = !titleLineEdit->text().isEmpty();
    bool passwordOk = !passwordLineEdit->text().isEmpty();
    if(okButton) { // Check if pointer is valid before using
        okButton->setEnabled(titleOk && passwordOk);
    }
}