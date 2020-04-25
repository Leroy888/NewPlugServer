#include "PosButton.h"
#include "ui_PosButton.h"

PosButton::PosButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PosButton)
{
    ui->setupUi(this);
    ui->pushButton->setStyleSheet("background: rgb(108, 108, 108); font: 75 8pt");
}

PosButton::~PosButton()
{
    delete ui;
}

void PosButton::setChecked(const bool value)
{
    ui->pushButton->setChecked(value);
}

void PosButton::setText(const QString &text)
{
    ui->pushButton->setText(text);
}

void PosButton::setSheetStyle(const QString &style)
{
    ui->pushButton->setStyleSheet(style);
}

void PosButton::on_pushButton_clicked()
{
    if(ui->pushButton->isChecked())
    {
        QString text = ui->pushButton->text();
        ui->pushButton->setStyleSheet("background-color: rgb(255, 0, 127); font: 35 8pt");
       // ui->pushButton->setStyleSheet("font: 75 13pt");
        emit sig_onBtnClicked(text);
    }
    else
    {
        emit sig_onBtnClicked(QString(""));
        ui->pushButton->setStyleSheet("background: rgb(108, 108, 108); font: 35 8pt");
    }
}
