#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "mainwindow.h"


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_btnOK_clicked()
{
    this->accept();
}

QString LoginDialog::getModel() const
{
    return ui->cmbModel->currentText();
}
