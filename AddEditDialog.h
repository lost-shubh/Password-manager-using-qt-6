#ifndef ADDEDITDIALOG_H
#define ADDEDITDIALOG_H

#include <QDialog>
#include <string>

#include "PasswordEntry.h"

// Forward Declarations
QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
class QPushButton;
class QDialogButtonBox;
QT_END_NAMESPACE

/** @brief Dialog for adding or editing a password entry. */
class AddEditDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructor for Add mode
    explicit AddEditDialog(QWidget *parent = nullptr);
    // Constructor for Edit mode - Arguments swapped
    explicit AddEditDialog(const PasswordEntry* entryToEdit, QWidget *parent = nullptr);

    ~AddEditDialog() override;

    PasswordEntry getEntryData() const; // Get data from fields

private slots:
    void generateAndFillPassword(); // Generate button clicked
    void togglePasswordVisibility(bool checked); // Show/hide button toggled
    void validateData(); // Slot connected to text changes to enable/disable OK button

private:
    void setupUi(); // Common UI setup

    // UI Elements
    QLineEdit *titleLineEdit = nullptr;
    QLineEdit *usernameLineEdit = nullptr;
    QLineEdit *passwordLineEdit = nullptr;
    QLineEdit *urlLineEdit = nullptr;
    QTextEdit *notesTextEdit = nullptr;
    QPushButton *generatePasswordButton = nullptr;
    QPushButton *showPasswordButton = nullptr;
    QDialogButtonBox *buttonBox = nullptr;
    QPushButton* okButton = nullptr; // Pointer to the OK button for enabling/disabling

    bool isEditMode = false;
};

#endif // ADDEDITDIALOG_H