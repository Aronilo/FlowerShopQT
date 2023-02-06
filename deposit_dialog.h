#ifndef DEPOSIT_DIALOG_H
#define DEPOSIT_DIALOG_H

#include <QDialog>

namespace Ui {
class DepositDialog;
}

class DepositDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositDialog(QWidget *parent = nullptr);
    ~DepositDialog();

    int deposit() const;

private:
    Ui::DepositDialog *ui;
};

#endif // DEPOSIT_DIALOG_H
