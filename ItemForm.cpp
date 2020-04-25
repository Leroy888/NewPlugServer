#include "ItemForm.h"
#include "ui_ItemForm.h"

ItemForm::ItemForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemForm)
{
    ui->setupUi(this);
}

ItemForm::~ItemForm()
{
    delete ui;
}

void ItemForm::setPoint1(QPoint value)
{
    QString x = QString::number(value.x());
    QString y = QString::number(value.y());
    QString text = x + "," + y;
    ui->lineEdit_p1->setText(text);
}

QString ItemForm::getPoint2()
{
    QString text = ui->lineEdit_p2->text();
    if(text == "")
    {
        text = "0,0";
    }
    return text;
}

void ItemForm::setPoint2(QPoint value)
{
    QString x = QString::number(value.x());
    QString y = QString::number(value.y());
    QString text = x + "," + y;
    ui->lineEdit_p2->setText(text);
}

QString ItemForm::getPoint1()
{
    QString text = ui->lineEdit_p1->text();
    if(text == "")
    {
        text = "0,0";
    }

    return text;
}
