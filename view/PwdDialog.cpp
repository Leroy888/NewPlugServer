#include "PwdDialog.h"
#include "ui_PwdDialog.h"
#include <QDebug>

PwdDialog::PwdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PwdDialog)
{
    ui->setupUi(this);
    ui->btnOk->setShortcut(Qt::Key_Return);
    this->setWindowTitle(QStringLiteral("输入密码"));

}

PwdDialog::~PwdDialog()
{
    delete ui;
}

QString PwdDialog::getPwd() const
{
    return ui->lineEdit->text();
}

void PwdDialog::on_btnOk_clicked()
{
    qDebug()<<"onBtnOk clicked";
    this->accept();
}

void PwdDialog::on_btnCancel_clicked()
{
    qDebug()<<"onBtnCancel clicked";
    this->reject();
}
