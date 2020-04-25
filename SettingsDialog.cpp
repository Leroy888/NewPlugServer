#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "com/clsSettings.h"
#include <QTableWidgetItem>
#include <QComboBox>
#include <QFileDialog>
#include "view/AiPortDialog.h"


#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("系统设置"));
    readSettings("./cfg/Settings.ini");
    iniUi();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::iniUi()
{

}

void SettingsDialog::updateUi()
{

}

void SettingsDialog::setDelaySecs(int secs)
{
    ui->spinBox_delay->setValue(secs);
}

int SettingsDialog::getDelaySecs()
{
    return ui->spinBox_delay->value();
}

void SettingsDialog::readSettings(QString fileName)
{
    clsSettings *set = new clsSettings(fileName);
    QString strNode = QString("System/");

}

void SettingsDialog::on_btnOk_clicked()
{
    m_dbParam.ip = ui->lineEdit_dbIp->text();
    m_dbParam.port = 3306;
    m_dbParam.name = ui->lineEdit_dbName->text();
    m_dbParam.user = ui->lineEdit_dbUser->text();
    m_dbParam.pwd = ui->lineEdit_dbPwd->text();

    m_dbParam2.ip = ui->lineEdit_dbIp_2->text();
    m_dbParam2.port = 3306;
    m_dbParam2.name = ui->lineEdit_dbName_2->text();
    m_dbParam2.user = ui->lineEdit_dbUser_2->text();
    m_dbParam2.pwd = ui->lineEdit_dbPwd_2->text();

    m_savePath2 = ui->lineEdit_path2->text();
    m_wkshop = ui->lineEdit_wkshop->text();

    this->accept();
}

void SettingsDialog::on_btnPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, QStringLiteral("选择图片保存目录"),QStringLiteral("保存目录"));
    if(path == "")
    {
        return;
    }
    m_savePath = path;
    ui->lineEdit_path->setText(m_savePath);
}

void SettingsDialog::on_btnCancel_clicked()
{
    this->reject();
}

QString SettingsDialog::getSavePath() const
{
    return m_savePath;
}

void SettingsDialog::setSavePath(const QString &savePath)
{
    m_savePath = savePath;
    ui->lineEdit_path->setText(m_savePath);
}

DB_Param SettingsDialog::getDbParam() const
{
    return m_dbParam;
}

DB_Param SettingsDialog::getDbParam2() const
{
    return m_dbParam2;
}

void SettingsDialog::setDbParam(const DB_Param &dbParam, const DB_Param &dbParam2)
{
    m_dbParam = dbParam;
    ui->lineEdit_dbIp->setText(m_dbParam.ip);
    ui->lineEdit_dbName->setText(m_dbParam.name);
    ui->lineEdit_dbUser->setText(m_dbParam.user);
    ui->lineEdit_dbPwd->setText(m_dbParam.pwd);
    m_dbParam.port = 3306;

    m_dbParam2 = dbParam2;
    ui->lineEdit_dbIp_2->setText(m_dbParam2.ip);
    ui->lineEdit_dbName_2->setText(m_dbParam2.name);
    ui->lineEdit_dbUser_2->setText(m_dbParam2.user);
    ui->lineEdit_dbPwd_2->setText(m_dbParam2.pwd);
    m_dbParam2.port = 3306;
}

int SettingsDialog::getModel()
{
    int model = ui->cmbBox_model->currentIndex();

    return model;
}

void SettingsDialog::setModel(const int &model)
{
    ui->cmbBox_model->setCurrentIndex(model);
}

//int SettingsDialog::getVerPieces()
//{
//    m_verPieces = ui->spinBox_ver->value();
//    return m_verPieces;
//}

//void SettingsDialog::setVerPieces(int verPieces)
//{
//    m_verPieces = verPieces;
//    ui->spinBox_ver->setValue(verPieces);
//}

//int SettingsDialog::getHorPieces()
//{
//    m_horPieces = ui->spinBox_hor->value();
//    return m_horPieces;
//}

//void SettingsDialog::setHorPieces(int horPieces)
//{
//    m_horPieces = horPieces;
//    ui->spinBox_hor->setValue(horPieces);
//}

void SettingsDialog::on_spinBox_up_valueChanged(int arg1)
{
    m_upPixel = arg1;
}

//int SettingsDialog::getRightPixel() const
//{
//    return m_rightPixel;
//}

//void SettingsDialog::setRightPixel(int rightPixel)
//{
//    m_rightPixel = rightPixel;
//    ui->spinBox_right->setValue(rightPixel);
//}

QString SettingsDialog::getOrder() const
{
    return ui->cmb_order->currentText();
}

QStringList SettingsDialog::getOrderList() const
{
    return m_orderList;
}

void SettingsDialog::setOrder(const QStringList &orderList, const QString &order)
{
    m_orderList = orderList;
    ui->cmb_order->addItems(orderList);
    ui->cmb_order->setCurrentText(order);
}

QStringList SettingsDialog::getDgList() const
{
    return m_dgList;
}

void SettingsDialog::setDgList(const QStringList &dgList)
{
    m_dgList = dgList;
    ui->cmb_dg->clear();
    ui->cmb_dg->addItems(dgList);
}

int SettingsDialog::getImgModel() const
{
    return ui->cmbBox_img->currentIndex();
}

void SettingsDialog::setImgModel(const int model)
{
    this->m_imgModel = model;
    ui->cmbBox_img->setCurrentIndex(model);
}

//int SettingsDialog::getLeftPixel() const
//{
//    return m_leftPixel;
//}

//void SettingsDialog::setLeftPixel(int leftPixel)
//{
//    m_leftPixel = leftPixel;
//    ui->spinBox_left->setValue(leftPixel);
//}

//int SettingsDialog::getBottomPixel() const
//{
//    return m_bottomPixel;
//}

//void SettingsDialog::setBottomPixel(int bottomPixel)
//{
//    m_bottomPixel = bottomPixel;
//    ui->spinBox_bottom->setValue(bottomPixel);
//}

//int SettingsDialog::getUpPixel() const
//{
//    return m_upPixel;
//}

//void SettingsDialog::setUpPixel(int upPixel)
//{
//    m_upPixel = upPixel;
//    ui->spinBox_up->setValue(upPixel);
//}
void SettingsDialog::on_spinBox_bottom_valueChanged(int arg1)
{
    m_bottomPixel = arg1;
}

void SettingsDialog::on_spinBox_left_valueChanged(int arg1)
{
    m_leftPixel = arg1;
}

void SettingsDialog::on_spinBox_right_valueChanged(int arg1)
{
    m_rightPixel = arg1;
}

void SettingsDialog::on_btnOrderEdit_clicked()
{
    QStringList headers;
    headers<<QStringLiteral("序号")<<QStringLiteral("工单号");
    AiPortDialog *dlg = new AiPortDialog(m_orderList, headers, QString("Item"));
    dlg->setWindowTitle(QStringLiteral("工单设置"));
    if(dlg->exec() == QDialog::Accepted)
    {
        m_orderList.clear();
        m_orderList = dlg->getList();
        ui->cmb_order->clear();
        ui->cmb_order->addItems(m_orderList);
    }
    delete dlg;
}

void SettingsDialog::on_btnDgEdit_clicked()
{
    QStringList headers;
    headers<<QStringLiteral("序号")<<QStringLiteral("灰度值");
    AiPortDialog *dlg = new AiPortDialog(m_dgList, headers, QString("LineEdit"));
    dlg->setWindowTitle(QStringLiteral("灰度值设置"));
    if(dlg->exec() == QDialog::Accepted)
    {
        m_dgList.clear();
        m_dgList = dlg->getList();
        ui->cmb_dg->clear();
        ui->cmb_dg->addItems(m_dgList);
    }
    delete dlg;
}

QString SettingsDialog::getWkshop() const
{
    return m_wkshop;
}

void SettingsDialog::setWkshop(const QString &wkshop)
{
    m_wkshop = wkshop;
    ui->lineEdit_wkshop->setText(wkshop);
}

QString SettingsDialog::getSavePath2() const
{
    return m_savePath2;
}

void SettingsDialog::setSavePath2(const QString &savePath2)
{
    m_savePath2 = savePath2;
    ui->lineEdit_path2->setText(savePath2);
}
