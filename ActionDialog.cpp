#include "ActionDialog.h"
#include "ui_ActionDialog.h"

ActionDialog::ActionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionDialog)
{
    ui->setupUi(this);
    ui->btnOk->setShortcut(Qt::Key_Return);
}

ActionDialog::~ActionDialog()
{
    delete ui;
}

QString ActionDialog::getNode()
{
    return ui->lineEdit_node->text();
}

void ActionDialog::setNode(QString node)
{
    ui->lineEdit_node->setText(node);
}

void ActionDialog::on_btnOk_clicked()
{
    this->accept();
}

void ActionDialog::on_btnCancel_clicked()
{
    this->reject();
}
