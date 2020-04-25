#include "ClientForm.h"
#include "ui_ClientForm.h"
#include "SettingsDialog.h"

#include <QDebug>

const QString red_style = "border-radius: 2px;border:1px solid black;background:#ea0000; color: rgb(18, 18, 18);font-size: 25px;";
const QString green_style = "border-radius: 2px;border:1px solid black;background:#36ea36; color: rgb(18, 18, 18);font-size: 25px;";
const QString ok_style = "font-size: 29px;border-radius: 5px;border:1px solid black;background:#36ea36; color: rgb(18, 18, 18);";
const QString ng_style = "border-radius: 5px;border:1px solid black;background:#ea0000; color: rgb(18, 18, 18);font-size: 29px;";
const QString error_style = "border-radius: 5px;border:1px solid black;background:#0055ff; color: rgb(18, 18, 18);font-size: 29px;";
const QString miss_style = "border-radius: 5px;border:1px solid black;background:#ff55ff; color: rgb(18, 18, 18);font-size: 29px;";
const QString info_style = "border-radius: 5px;border:1px solid black;background:#c6ffff; color: rgb(18, 18, 18);font-size: 25px;";


ClientForm::ClientForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientForm)
{
    ui->setupUi(this);

    ui->label_ok->setStyleSheet(ok_style);
    ui->label_ng->setStyleSheet(ng_style);
    ui->label_miss->setStyleSheet(miss_style);
    ui->label_error->setStyleSheet(error_style);
    ui->label_info->setStyleSheet(info_style);
    ui->btnSwitch->setStyleSheet(info_style);
    ui->label_status->setStyleSheet(red_style);
    ui->btnClear->setStyleSheet(info_style);
}

ClientForm::~ClientForm()
{
    delete ui;
}

void ClientForm::setParam(ClientParam param)
{
    m_handle = param.handle;
    ui->label_ok->setText(QString::number(param.okNum));
    ui->label_ng->setText(QString::number(param.ngNum));
    ui->label_error->setText(QString::number(param.errorNum));
    ui->label_miss->setText(QString::number(param.missNum));
    ui->label_info->setText(param.info);
}

void ClientForm::setOk(int value)
{
    ui->label_ok->setText(QString::number(value));
}

void ClientForm::setNg(int value)
{
    ui->label_ng->setText(QString::number(value));
}

void ClientForm::setMiss(int value)
{
    ui->label_miss->setText(QString::number(value));
}

void ClientForm::setError(int value)
{
    ui->label_error->setText(QString::number(value));
}

void ClientForm::setIp(QString strIp)
{
    ui->label_info->setText(strIp);
}

void ClientForm::setClientEnabled(bool value)
{
    if(value)
    {
        ui->btnSwitch->setText(QStringLiteral("开"));
        ui->label_status->setText(QStringLiteral("在线"));
        ui->label_status->setStyleSheet(green_style);
    }
    else
    {
        ui->btnSwitch->setText(QStringLiteral("关"));
        ui->label_status->setText(QStringLiteral("离线"));
        ui->label_status->setStyleSheet(red_style);
    }
}

void ClientForm::on_btnSettings_clicked()
{
    SettingsDialog *dlg = new SettingsDialog;
    if(dlg->exec() == QDialog::Accepted)
    {

    }
}

void ClientForm::on_btnSwitch_clicked()
{
    QString info = ui->label_info->text();
    if(ui->btnSwitch->text() == QStringLiteral("开"))
    {
       // ui->btnSwitch->setText(QStringLiteral("关"));
        setClientEnabled(false);
        emit sig_switch(m_handle, info, false);
    }
    else
    {
      //  ui->btnSwitch->setText(QStringLiteral("开"));
        setClientEnabled(true);
        emit sig_switch(m_handle, info, true);
    }
}

void ClientForm::on_btnClear_clicked()
{
    ui->label_ok->setText("0");
    ui->label_ng->setText("0");
    ui->label_error->setText("0");
    ui->label_miss->setText("0");
    emit sig_clear(m_handle);
}
