#include "MainWindow.h"
#include "AddEditDialog.h"
#include "PasswordManager.h"
#include "PasswordEntry.h"

// Qt Includes
#include <QtWidgets/QApplication>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QInputDialog>
#include <QtGui/QAction>
#include <QtGui/QKeySequence>
#include <QtGui/QCloseEvent>
#include <QtGui/QClipboard>
#include <QtCore/QString>
#include <QtCore/QVariant>

// Standard Includes
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>

MainWindow::MainWindow(std::unique_ptr<PasswordManager> mgr, QWidget *parent)
    : QMainWindow(parent), manager(std::move(mgr))
{
    setWindowTitle("Password Manager");
    setMinimumSize(700, 450);

    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
    setupTableWidget();

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Search Title/Username/URL...");
    searchButton = new QPushButton("Search", this);
    clearSearchButton = new QPushButton("Clear", this);
    searchLayout->addWidget(new QLabel("Search:", this));
    searchLayout->addWidget(searchLineEdit, 1);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(clearSearchButton);
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(entryTableWidget);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchEntries);
    connect(clearSearchButton, &QPushButton::clicked, this, &MainWindow::clearSearch);
    connect(searchLineEdit, &QLineEdit::returnPressed, this, &MainWindow::searchEntries);
    connect(entryTableWidget, &QTableWidget::itemDoubleClicked, this, &MainWindow::editEntry);
    connect(entryTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateActions);

    searchEntries(); // Initial load
    updateActions();
    statusBar->showMessage("Ready.", 2000);
}

MainWindow::~MainWindow() {
    try { if (manager) { manager->saveChanges(); } }
    catch (const std::exception& e) { std::cerr << "Error saving changes during destruction: " << e.what() << std::endl; }
}

void MainWindow::setupTableWidget() {
    // ... (setupTableWidget code remains the same) ...
    entryTableWidget = new QTableWidget(0, 4, this);
    entryTableWidget->setHorizontalHeaderLabels({"Title", "Username", "Password", "URL"});
    entryTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    entryTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    entryTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    entryTableWidget->verticalHeader()->setVisible(false);
    entryTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    entryTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    entryTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    entryTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void MainWindow::refreshEntryTable() {
    // ... (refreshEntryTable code remains the same) ...
    entryTableWidget->setSortingEnabled(false);
    entryTableWidget->setRowCount(0);
    entryTableWidget->setRowCount(static_cast<int>(currentDisplayedEntries.size()));
    const auto& allMgrEntries = manager->getAllEntries();
    for (size_t i = 0; i < currentDisplayedEntries.size(); ++i) {
        const auto& entry = currentDisplayedEntries[i];
        QTableWidgetItem *titleItem = new QTableWidgetItem(QString::fromStdString(entry.title));
        size_t managerIndex = std::string::npos;
         for(size_t mgrIdx = 0; mgrIdx < allMgrEntries.size(); ++mgrIdx) {
            if (allMgrEntries[mgrIdx].title == entry.title && allMgrEntries[mgrIdx].username == entry.username && allMgrEntries[mgrIdx].url == entry.url) {
                 managerIndex = mgrIdx; break;
            }
         }
         if (managerIndex != std::string::npos) { titleItem->setData(Qt::UserRole, QVariant::fromValue(managerIndex)); }
         else { std::cerr << "Warning: Displayed entry not found in manager list!" << std::endl; titleItem->setData(Qt::UserRole, QVariant::fromValue(-1LL)); }
        QTableWidgetItem *usernameItem = new QTableWidgetItem(QString::fromStdString(entry.username));
        QTableWidgetItem *passwordItem = new QTableWidgetItem("********");
        QTableWidgetItem *urlItem = new QTableWidgetItem(QString::fromStdString(entry.url));
        Qt::ItemFlags readOnlyFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        titleItem->setFlags(readOnlyFlags); usernameItem->setFlags(readOnlyFlags); passwordItem->setFlags(readOnlyFlags); urlItem->setFlags(readOnlyFlags);
        entryTableWidget->setItem(static_cast<int>(i), 0, titleItem); entryTableWidget->setItem(static_cast<int>(i), 1, usernameItem);
        entryTableWidget->setItem(static_cast<int>(i), 2, passwordItem); entryTableWidget->setItem(static_cast<int>(i), 3, urlItem);
    }
    entryTableWidget->setSortingEnabled(true); updateActions();
}

int MainWindow::getSelectedEntryIndex() const {
    // ... (getSelectedEntryIndex code remains the same) ...
    QList<QTableWidgetItem*> selectedItems = entryTableWidget->selectedItems();
    if (selectedItems.isEmpty()) { return -1; }
    QTableWidgetItem* firstItem = entryTableWidget->item(selectedItems.first()->row(), 0);
    if (!firstItem) return -1;
    bool ok;
    qlonglong storedIndex = firstItem->data(Qt::UserRole).toLongLong(&ok);
    if (ok && storedIndex >= 0 && static_cast<size_t>(storedIndex) < manager->getAllEntries().size()) { return static_cast<int>(storedIndex); }
    return -1;
}

void MainWindow::createActions() {
    addAction = new QAction("&Add Entry...", this); addAction->setShortcuts(QKeySequence::New); connect(addAction, &QAction::triggered, this, &MainWindow::addEntry);
    editAction = new QAction("&Edit Entry...", this); connect(editAction, &QAction::triggered, this, &MainWindow::editEntry);
    deleteAction = new QAction("&Delete Entry", this); deleteAction->setShortcuts(QKeySequence::Delete); connect(deleteAction, &QAction::triggered, this, &MainWindow::deleteEntry);
    generateAction = new QAction("&Generate Password...", this); connect(generateAction, &QAction::triggered, this, &MainWindow::generatePassword);
    changeMasterPasswordAction = new QAction("&Change Master Password...", this); connect(changeMasterPasswordAction, &QAction::triggered, this, &MainWindow::changeMasterPassword_triggered);
    // lockAction = new QAction("&Lock", this); connect(lockAction, &QAction::triggered, this, &MainWindow::lockManager); // <<<--- REMOVED
    exitAction = new QAction("E&xit", this); exitAction->setShortcuts(QKeySequence::Quit); connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu("&File");
    // fileMenu->addAction(lockAction); // <<<--- REMOVED
    // fileMenu->addSeparator(); // <<<--- REMOVED (optional)
    fileMenu->addAction(exitAction);

    QMenu *entryMenu = menuBar()->addMenu("&Entries");
    entryMenu->addAction(addAction); entryMenu->addAction(editAction); entryMenu->addAction(deleteAction);

    QMenu *toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction(generateAction); toolsMenu->addAction(changeMasterPasswordAction);
}

void MainWindow::createToolBar() {
    QToolBar *mainToolBar = addToolBar("Main");
    mainToolBar->addAction(addAction); mainToolBar->addAction(editAction); mainToolBar->addAction(deleteAction);
    mainToolBar->addSeparator(); mainToolBar->addAction(generateAction);
    // mainToolBar->addSeparator(); // <<<--- REMOVED (optional)
    // mainToolBar->addAction(lockAction); // <<<--- REMOVED
}

void MainWindow::createStatusBar() { statusBar = new QStatusBar(this); setStatusBar(statusBar); }

// --- Action Slots Implementation ---

void MainWindow::addEntry() { /* ... add entry logic (no changes) ... */
    AddEditDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        PasswordEntry newEntry = dialog.getEntryData();
        try { if (manager->addEntry(newEntry)) { searchEntries(); statusBar->showMessage("Entry added.", 3000); } else { QMessageBox::warning(this, "Add Failed", "Manager rejected entry."); }
        } catch (const std::exception& e) { QMessageBox::critical(this, "Add Error", QString("Error: %1").arg(e.what())); }
    }
}
void MainWindow::editEntry() { /* ... edit entry logic (no changes) ... */
    int managerIndex = getSelectedEntryIndex(); if (managerIndex < 0) { QMessageBox::warning(this, "Edit Entry", "Please select an entry."); return; }
    const PasswordEntry& existingEntry = manager->getAllEntries()[static_cast<size_t>(managerIndex)];
    AddEditDialog dialog(&existingEntry, this);
    if (dialog.exec() == QDialog::Accepted) {
        PasswordEntry updatedEntry = dialog.getEntryData();
        try { if (manager->updateEntry(static_cast<size_t>(managerIndex), updatedEntry)) { searchEntries(); statusBar->showMessage("Entry updated.", 3000); } else { QMessageBox::warning(this, "Update Failed", "Manager rejected update."); }
        } catch (const std::exception& e) { QMessageBox::critical(this, "Update Error", QString("Error: %1").arg(e.what())); }
    }
}
void MainWindow::deleteEntry() { /* ... delete entry logic (no changes) ... */
    int managerIndex = getSelectedEntryIndex(); if (managerIndex < 0) { QMessageBox::warning(this, "Delete Entry", "Please select an entry."); return; }
    const PasswordEntry& entryToDelete = manager->getAllEntries()[static_cast<size_t>(managerIndex)];
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", QString("Delete entry '%1'?").arg(QString::fromStdString(entryToDelete.title)), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        try { if (manager->deleteEntry(static_cast<size_t>(managerIndex))) { searchEntries(); statusBar->showMessage("Entry deleted.", 3000); } else { QMessageBox::warning(this, "Delete Failed", "Manager failed to delete."); }
        } catch (const std::exception& e) { QMessageBox::critical(this, "Delete Error", QString("Error: %1").arg(e.what())); }
    }
}
void MainWindow::generatePassword() { /* ... generate password logic (no changes) ... */
    try { std::string password = manager->generateRandomPassword(); QApplication::clipboard()->setText(QString::fromStdString(password)); QMessageBox::information(this, "Generated Password", QString("Generated Password:\n%1\n\n(Copied to clipboard)").arg(QString::fromStdString(password)));
    } catch (const std::exception& e) { QMessageBox::critical(this, "Generation Error", QString("Error: %1").arg(e.what())); }
}

// void MainWindow::lockManager() { close(); } // <<<--- REMOVED SLOT IMPLEMENTATION

void MainWindow::changeMasterPassword_triggered() { /* ... change master password logic (no changes) ... */
    bool ok1, ok2;
    QString newPass1 = QInputDialog::getText(this, "Change Master Password", "Enter NEW master password:", QLineEdit::Password, QString(), &ok1);
    if (!ok1 || newPass1.isEmpty()) { statusBar->showMessage("Master password change cancelled.", 2000); return; }
    QString newPass2 = QInputDialog::getText(this, "Confirm Master Password", "Confirm NEW master password:", QLineEdit::Password, QString(), &ok2);
    if (!ok2) { statusBar->showMessage("Master password change cancelled.", 2000); return; }
    if (newPass1 != newPass2) { QMessageBox::warning(this, "Password Mismatch", "Passwords do not match."); return; }
    try { manager->changeMasterPassword(newPass1.toStdString()); QMessageBox::information(this, "Success", "Master password updated."); statusBar->showMessage("Master password changed.", 3000); }
    catch (const std::exception& e) { QMessageBox::critical(this, "Change Failed", QString("Error: %1").arg(e.what())); }
}

void MainWindow::searchEntries() { /* ... search entries logic (no changes) ... */
    std::string searchTerm = searchLineEdit->text().toStdString();
    try {
        if (searchTerm.empty()) { currentDisplayedEntries = manager->getAllEntries(); } else { currentDisplayedEntries = manager->findEntries(searchTerm); }
        refreshEntryTable();
        if (!searchTerm.empty()) { statusBar->showMessage(QString("Found %1 entries matching '%2'.").arg(currentDisplayedEntries.size()).arg(QString::fromStdString(searchTerm)), 3000); }
        else { statusBar->showMessage(QString("Showing all %1 entries.").arg(currentDisplayedEntries.size()), 2000); }
    } catch (const std::exception& e) { QMessageBox::critical(this, "Search Error", QString("Error during search: %1").arg(e.what())); }
}
void MainWindow::clearSearch() { searchLineEdit->clear(); searchEntries(); }
void MainWindow::updateActions() { bool itemSelected = getSelectedEntryIndex() >= 0; editAction->setEnabled(itemSelected); deleteAction->setEnabled(itemSelected); }
void MainWindow::closeEvent(QCloseEvent *event) { /* ... close event logic (no changes) ... */
    try { manager->saveChanges(); event->accept(); }
    catch (const std::exception& e) { QMessageBox::StandardButton r = QMessageBox::critical(this, "Save Error", QString("Failed to save: %1\nClose anyway?").arg(e.what()), QMessageBox::Yes | QMessageBox::No); if (r == QMessageBox::Yes) event->accept(); else event->ignore(); }
}