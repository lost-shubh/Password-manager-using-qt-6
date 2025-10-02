#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>
#include <string>

// Forward Declarations
QT_BEGIN_NAMESPACE
class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class QLineEdit;
class QAction;
class QStatusBar;
QT_END_NAMESPACE

class PasswordManager;
class PasswordEntry;

/** @brief Main application window displaying password entries. */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::unique_ptr<PasswordManager> manager, QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Slots for UI actions
    void addEntry();
    void editEntry();
    void deleteEntry();
    void generatePassword();
    void searchEntries();
    void clearSearch();
    // void lockManager(); // <<<--- REMOVED SLOT
    void changeMasterPassword_triggered();

private:
    // UI setup helpers
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void setupTableWidget();
    void refreshEntryTable();
    int getSelectedEntryIndex() const;
    void updateActions();


    // UI Elements
    QTableWidget *entryTableWidget = nullptr;
    QLineEdit *searchLineEdit = nullptr;
    QPushButton *searchButton = nullptr;
    QPushButton *clearSearchButton = nullptr;
    QStatusBar *statusBar = nullptr;

    // Actions
    QAction *addAction = nullptr;
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *generateAction = nullptr;
    QAction *changeMasterPasswordAction = nullptr;
    // QAction *lockAction = nullptr; // <<<--- REMOVED ACTION POINTER
    QAction *exitAction = nullptr;

    // Backend Data
    std::unique_ptr<PasswordManager> manager;
    std::vector<PasswordEntry> currentDisplayedEntries;
};

#endif // MAINWINDOW_H