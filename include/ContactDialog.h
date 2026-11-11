#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include "Contact.h"
#include <QDialog>
#include <functional>

class QLabel;
class QLineEdit;

class ContactDialog : public QDialog {
    Q_OBJECT

public:
    using DuplicateCheckFn = std::function<bool(const Contact&)>;

    explicit ContactDialog(
        const QString& title,
        const Contact* initialContact = nullptr,
        DuplicateCheckFn isDuplicate = {},
        QWidget* parent = nullptr
    );

    Contact contact() const;

private slots:
    void onSaveClicked();

private:
    QLineEdit* nameEdit;
    QLineEdit* phoneEdit;
    QLineEdit* emailEdit;
    QLabel* errorLabel;
    DuplicateCheckFn duplicateChecker;
    Contact resultContact;
};

#endif
