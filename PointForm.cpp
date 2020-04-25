#include "PointForm.h"
#include "ui_PointForm.h"

PointForm::PointForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PointForm)
{
    ui->setupUi(this);
}

PointForm::~PointForm()
{
    delete ui;
}

void PointForm::setPoint1(QString p)
{
    QStringList list = p.split(",");
    if(list.length() != 2)
    {
       ui->lineEdit_x1->setText("0");
       ui->lineEdit_y1->setText("0");
    }
    else
    {
        ui->lineEdit_x1->setText(list.at(0));
        ui->lineEdit_y1->setText(list.at(1));
    }
}

QString PointForm::getPoint1()
{
    QString px = "0";
    QString py = "0";
    if(ui->lineEdit_x1->text() != "")
    {
        px = ui->lineEdit_x1->text();
    }
    if(ui->lineEdit_y1->text() != "")
    {
        py = ui->lineEdit_y1->text();
    }

    QString strP = px + "," + py;

    return strP;
}

void PointForm::setPoint2(QString p)
{
    QStringList list = p.split(",");
    if(list.length() != 2)
    {
       ui->lineEdit_x2->setText("0");
       ui->lineEdit_y2->setText("0");
    }
    else
    {
        ui->lineEdit_x2->setText(list.at(0));
        ui->lineEdit_y2->setText(list.at(1));
    }
}

QString PointForm::getPoint2()
{
    QString px = "0";
    QString py = "0";
    if(ui->lineEdit_x2->text() != "")
    {
        px = ui->lineEdit_x2->text();
    }
    if(ui->lineEdit_y2->text() != "")
    {
        py = ui->lineEdit_y2->text();
    }

    QString strP = px + "," + py;

    return strP;
}
