#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ContactList.h"
#include <QMainWindow>
#include <cstddef>
#include <optional>
#include <string>

class QCloseEvent;
class QLineEdit;
class QPushButton;
class QTableWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const std::string& dataFilePath, QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    bool onSaveContacts();
    void refreshTable();
    void updateActionStates();

private:
    void setupUi();
    void setupShortcuts();
    void setDirty(bool dirty);
    void updateStatusSummary(const QString& actionMessage = QString());
    std::optional<std::size_t> selectedContactIndex() const;

    ContactList contactList;
    QLineEdit* searchEdit;
    QTableWidget* table;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QPushButton* saveButton;
    bool hasUnsavedChanges;
};

#endif
