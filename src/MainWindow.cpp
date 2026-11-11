#include "MainWindow.h"
#include "ContactDialog.h"
#include "Validator.h"

#include <QAbstractItemView>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <numeric>
#include <vector>

MainWindow::MainWindow(const std::string& dataFilePath, QWidget* parent)
    : QMainWindow(parent),
      contactList(dataFilePath),
      searchEdit(new QLineEdit(this)),
      table(new QTableWidget(this)),
      addButton(new QPushButton("Add", this)),
      editButton(new QPushButton("Edit", this)),
      deleteButton(new QPushButton("Delete", this)),
      saveButton(new QPushButton("Save", this)),
      hasUnsavedChanges(false) {
    setupUi();
    setupShortcuts();
    refreshTable();
    updateStatusSummary();
}

void MainWindow::setupUi() {
    setWindowTitle("PhoneBook Manager[*]");
    resize(780, 500);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    searchEdit->setPlaceholderText("Search by name or phone...");
    searchEdit->setClearButtonEnabled(true);

    auto* topBar = new QHBoxLayout();
    topBar->addWidget(new QLabel("Search:", this));
    topBar->addWidget(searchEdit, 1);
    topBar->addWidget(addButton);
    topBar->addWidget(editButton);
    topBar->addWidget(deleteButton);
    topBar->addWidget(saveButton);

    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Full Name", "Phone Number", "E-Mail"});
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(table);

    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::refreshTable);
    connect(table, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateActionStates);
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) {
        onEditContact();
    });
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveContacts);
}

void MainWindow::setupShortcuts() {
    auto* addShortcut = new QShortcut(QKeySequence::New, this);
    connect(addShortcut, &QShortcut::activated, this, &MainWindow::onAddContact);

    auto* saveShortcut = new QShortcut(QKeySequence::Save, this);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::onSaveContacts);

    auto* findShortcut = new QShortcut(QKeySequence::Find, this);
    connect(findShortcut, &QShortcut::activated, this, [this]() {
        searchEdit->setFocus();
        searchEdit->selectAll();
    });

    auto* deleteShortcut = new QShortcut(QKeySequence::Delete, table);
    deleteShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(deleteShortcut, &QShortcut::activated, this, &MainWindow::onDeleteContact);
}

void MainWindow::setDirty(bool dirty) {
    hasUnsavedChanges = dirty;
    setWindowModified(dirty);
}

void MainWindow::updateStatusSummary(const QString& actionMessage) {
    const int shown = table->rowCount();
    const std::size_t total = contactList.size();
    QString status = QString("Showing %1 of %2 contact(s)")
                         .arg(shown)
                         .arg(static_cast<qulonglong>(total));
    if (hasUnsavedChanges) {
        status += "  |  Unsaved changes";
    }
    if (!actionMessage.isEmpty()) {
        status += "  |  " + actionMessage;
    }
    statusBar()->showMessage(status);
}

std::optional<std::size_t> MainWindow::selectedContactIndex() const {
    const auto selected = table->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return std::nullopt;
    }
    const int row = selected.first().row();
    const QTableWidgetItem* item = table->item(row, 0);
    if (item == nullptr) {
        return std::nullopt;
    }
    return static_cast<std::size_t>(item->data(Qt::UserRole).toULongLong());
}

void MainWindow::refreshTable() {
    const std::string keyword = Validator::trim(searchEdit->text().toStdString());
    std::vector<std::size_t> indices;
    if (keyword.empty()) {
        indices.resize(contactList.size());
        std::iota(indices.begin(), indices.end(), 0);
    } else {
        indices = contactList.searchContactsAdvanced(keyword);
    }

    table->setRowCount(static_cast<int>(indices.size()));
    for (int row = 0; row < static_cast<int>(indices.size()); ++row) {
        const std::size_t realIndex = indices[static_cast<std::size_t>(row)];
        const Contact& c = contactList.getContact(realIndex);

        auto* nameItem = new QTableWidgetItem(QString::fromStdString(c.getName()));
        nameItem->setData(Qt::UserRole, static_cast<qulonglong>(realIndex));
        auto* phoneItem = new QTableWidgetItem(QString::fromStdString(c.getPhone()));
        auto* emailItem = new QTableWidgetItem(QString::fromStdString(c.getEmail()));

        table->setItem(row, 0, nameItem);
        table->setItem(row, 1, phoneItem);
        table->setItem(row, 2, emailItem);
    }

    updateActionStates();
    updateStatusSummary();
}

void MainWindow::updateActionStates() {
    const bool hasSelection = selectedContactIndex().has_value();
    editButton->setEnabled(hasSelection);
    deleteButton->setEnabled(hasSelection);
}

void MainWindow::onAddContact() {
    ContactDialog dialog(
        "Add Contact",
        nullptr,
        [this](const Contact& candidate) { return contactList.hasContact(candidate); },
        this
    );
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (contactList.addContact(dialog.contact())) {
        setDirty(true);
        refreshTable();
        updateStatusSummary("Contact added");
    } else {
        QMessageBox::warning(this, "Duplicate Contact", "A matching contact already exists in the phonebook!");
    }
}

void MainWindow::onEditContact() {
    const auto realIndex = selectedContactIndex();
    if (!realIndex) {
        return;
    }

    const Contact current = contactList.getContact(*realIndex);
    ContactDialog dialog(
        "Edit Contact",
        &current,
        [this, realIndex](const Contact& candidate) {
            return contactList.hasContact(candidate, *realIndex);
        },
        this
    );
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const Contact updated = dialog.contact();
    if (updated == current) {
        return;
    }

    if (contactList.updateContact(*realIndex, updated)) {
        setDirty(true);
        refreshTable();
        updateStatusSummary("Contact updated");
    } else {
        QMessageBox::warning(this, "Duplicate Contact", "A matching contact already exists in the phonebook!");
    }
}

void MainWindow::onDeleteContact() {
    const auto realIndex = selectedContactIndex();
    if (!realIndex) {
        return;
    }

    const Contact& target = contactList.getContact(*realIndex);
    const auto answer = QMessageBox::question(
        this,
        "Delete Contact",
        QString("Are you sure you want to delete \"%1\"?")
            .arg(QString::fromStdString(target.getName())),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    if (answer != QMessageBox::Yes) {
        return;
    }

    if (contactList.deleteContact(*realIndex)) {
        setDirty(true);
        refreshTable();
        updateStatusSummary("Contact deleted");
    }
}

bool MainWindow::onSaveContacts() {
    if (contactList.saveToFile()) {
        setDirty(false);
        updateStatusSummary("Saved to " + QString::fromStdString(contactList.getFileName()));
        return true;
    }

    QMessageBox::critical(
        this,
        "Save Failed",
        QString("Cannot save contacts to:\n%1")
            .arg(QString::fromStdString(contactList.getFileName()))
    );
    return false;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!hasUnsavedChanges) {
        event->accept();
        return;
    }

    const auto reply = QMessageBox::question(
        this,
        "Unsaved Changes",
        "You have unsaved changes. Do you want to save before exiting?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );

    if (reply == QMessageBox::Save) {
        if (onSaveContacts()) {
            event->accept();
        } else {
            event->ignore();
        }
    } else if (reply == QMessageBox::Discard) {
        event->accept();
    } else {
        event->ignore();
    }
}
