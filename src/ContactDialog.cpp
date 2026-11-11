#include "ContactDialog.h"
#include "Validator.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <utility>

ContactDialog::ContactDialog(
    const QString& title,
    const Contact* initialContact,
    DuplicateCheckFn isDuplicate,
    QWidget* parent
)
    : QDialog(parent),
      nameEdit(new QLineEdit(this)),
      phoneEdit(new QLineEdit(this)),
      emailEdit(new QLineEdit(this)),
      errorLabel(new QLabel(this)),
      duplicateChecker(std::move(isDuplicate)) {
    setWindowTitle(title);
    setMinimumWidth(420);

    nameEdit->setPlaceholderText("e.g. Quang Vu Nguyen");
    phoneEdit->setPlaceholderText("e.g. 015566131440 or +84 912-345-678");
    emailEdit->setPlaceholderText("e.g. name@domain.com");

    if (initialContact != nullptr) {
        nameEdit->setText(QString::fromStdString(initialContact->getName()));
        phoneEdit->setText(QString::fromStdString(initialContact->getPhone()));
        emailEdit->setText(QString::fromStdString(initialContact->getEmail()));
    }

    errorLabel->setStyleSheet("color: #d32f2f;");
    errorLabel->setWordWrap(true);
    errorLabel->hide();

    auto* formLayout = new QFormLayout();
    formLayout->addRow("Full Name:", nameEdit);
    formLayout->addRow("Phone Number:", phoneEdit);
    formLayout->addRow("Email:", emailEdit);

    auto* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel,
        this
    );
    if (auto* saveBtn = buttonBox->button(QDialogButtonBox::Save)) {
        saveBtn->setDefault(true);
    }

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ContactDialog::onSaveClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addWidget(buttonBox);
}

Contact ContactDialog::contact() const {
    return resultContact;
}

void ContactDialog::onSaveClicked() {
    const std::string name = Validator::trim(nameEdit->text().toStdString());
    const std::string phone = Validator::trim(phoneEdit->text().toStdString());
    const std::string email = Validator::trim(emailEdit->text().toStdString());

    auto showError = [this](const QString& msg, QLineEdit* focusTarget) {
        errorLabel->setText(msg);
        errorLabel->show();
        focusTarget->setFocus();
    };

    if (!Validator::isValidName(name)) {
        showError("Invalid name! Use only letters, spaces in the middle, hyphens, or apostrophes.", nameEdit);
        return;
    }
    if (!Validator::isValidPhone(phone)) {
        showError("Invalid phone! Use digits, leading +, spaces in the middle, -, or parentheses (min 3 digits).", phoneEdit);
        return;
    }
    if (!Validator::isValidEmail(email)) {
        showError("Invalid email! Must be in format: name@domain.com", emailEdit);
        return;
    }

    Contact candidate(name, phone, email);
    if (duplicateChecker && duplicateChecker(candidate)) {
        showError("A matching contact already exists in the phonebook!", nameEdit);
        return;
    }

    resultContact = std::move(candidate);
    accept();
}
