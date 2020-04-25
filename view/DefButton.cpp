#include "DefButton.h"
#include "ui_DefButton.h"

#include <QDebug>

DefButton::DefButton(const int index, QWidget *parent) :
    QWidget(parent), m_index(index),
    ui(new Ui::DefButton)
{
    ui->setupUi(this);
    ui->pushButton->setStyleSheet("background: rgb(108, 108, 108); font: 75 12pt");
}

DefButton::~DefButton()
{
    delete ui;
}

void DefButton::setText(const QString &text)
{
    ui->pushButton->setText(text);
}

void DefButton::setChecked(bool value)
{
    ui->pushButton->setChecked(value);
    if(value)
    {
        ui->pushButton->setStyleSheet("background: red");
    }
    else
    {
        ui->pushButton->setStyleSheet("background: rgb(108, 108, 108); font: 75 12pt");
    }

}

bool DefButton::isChecked()
{
    if(ui->pushButton->isChecked())
    {
        return true;
    }
    return false;
}

QString DefButton::text() const
{
    return ui->pushButton->text();
}

void DefButton::on_pushButton_clicked()
{
    if(ui->pushButton->isChecked())
    {
        QString text = ui->pushButton->text();
        ui->pushButton->setStyleSheet("background-color: rgb(255, 0, 127); font: 75 12pt");
        emit sig_onBtnClicked(true, m_index, text);
        qDebug()<<"true "<<m_index;
    }
    else
    {
        emit sig_onBtnClicked(false, m_index, QString(""));
        ui->pushButton->setStyleSheet("background: rgb(108, 108, 108); font: 75 12pt");
        qDebug()<<"false "<<m_index;
    }
}
