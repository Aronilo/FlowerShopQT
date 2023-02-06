#include "deposit_dialog.h"
#include "ui_deposit_dialog.h"

DepositDialog::DepositDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DepositDialog)
{
    ui->setupUi(this);

    ui->spinBox->setMinimum(1);
    ui->spinBox->setMaximum(1000000);
}

DepositDialog::~DepositDialog()
{
    delete ui;
}

int DepositDialog::deposit() const {
    return ui->spinBox->value();
}
