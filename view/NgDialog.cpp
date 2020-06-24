#include "NgDialog.h"
#include "ui_NgDialog.h"


#include <QDebug>

NgDialog::NgDialog(const bool isTurn, const int horPic, const int verPic, const QStringList &defList, QWidget *parent) :
    QDialog(parent), m_isTurn(isTurn), m_horPic(horPic), m_verPic(verPic), m_defList(defList), m_isChecked(false), m_defIndex(-1),
    ui(new Ui::NgDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("缺陷选择"));
    m_isOk = false;
    m_level = "";
    initUi();
}

NgDialog::~NgDialog()
{
    delete ui;
}

bool NgDialog::getIsOk() const
{
    return m_isOk;
}

QStringList NgDialog::getPosList() const
{
    return m_posList;
}

QStringList NgDialog::getDefList() const
{
    return m_mesDefList;
}

QString NgDialog::getLevel() const
{
    return m_level;
}

void NgDialog::initUi()
{
    ui->label->setText("");
    for(int i=0; i<m_horPic; i++)
    {
        ui->gridLayout_pos->setColumnStretch(i, 1);
    }
    for(int i=0; i<m_verPic; i++)
    {
        ui->gridLayout_pos->setRowStretch(i, 1);
    }
    QStringList leftMark;
    leftMark<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K";

    int maxWid = 39;
    if(m_horPic > 50)
    {
        maxWid = 18;
    }

    for(int i=0; i<m_verPic; i++)
    {
        if(m_isTurn)
        {
            for(int j=0; j<m_horPic; j++)
            {
                PosButton *btn = new PosButton();
                btn->setMaximumWidth(maxWid);
                QString text = leftMark.at(m_verPic - i - 1) + QString::number(j + 1);
                btn->setText(text);
                m_posBtnList.append(btn);
                connect(btn,SIGNAL(sig_onBtnClicked(QString)),this,SLOT(slot_onPosBtnClicked(QString)));
                ui->gridLayout_pos->addWidget(btn, i, j);
            }
        }
        else
        {
            for(int j=0; j<m_horPic; j++)
            {
                PosButton *btn = new PosButton();
                btn->setMaximumWidth(maxWid);
                QString text = leftMark.at(i) + QString::number(j + 1);
                btn->setText(text);
                m_posBtnList.append(btn);
                connect(btn,SIGNAL(sig_onBtnClicked(QString)),this,SLOT(slot_onPosBtnClicked(QString)));
                ui->gridLayout_pos->addWidget(btn, i, j);
            }
        }
    }

    ui->widget_pos->setEnabled(false);

    int row = m_defList.length() / 6 ;
    if(m_defList.length() % 6 != 0)
    {
        row++;
    }

    for(int i=0; i<row + 1; i++)
    {
        ui->gridLayout_def->setRowStretch(i, 1);
    }
    for(int i=0; i<6; i++)
    {
        ui->gridLayout_def->setColumnStretch(i, 1);
    }

    for(int i=0; i<m_defList.length(); i++)
    {
        int r = i / 6;
        int c = i % 6;
        DefButton *btn = new DefButton(i);
        btn->setText(m_defList.at(i));
        m_defBtnList.append(btn);
        connect(btn,SIGNAL(sig_onBtnClicked(bool, int, QString)),this,SLOT(slot_onDefBtnClicked(bool, int, QString)));
        ui->gridLayout_def->addWidget(btn, r, c);
    }
}

void NgDialog::on_btnReset_clicked()
{
    m_defBtnList.at(m_defIndex)->setChecked(false);
    slot_onDefBtnClicked(false, m_defIndex, "");
    m_defIndex = -1;
    m_mesDefList.clear();
    m_posList.clear();
    ui->label->setText(QString(""));
}

void NgDialog::on_btnCancel_clicked()
{
    m_isOk = true;
    this->reject();
}

void NgDialog::on_btnOk_clicked()
{
    this->accept();
}

void NgDialog::slot_onDefBtnClicked(bool value, int index, QString text)
{
    ui->widget_pos->setEnabled(value);

    if(m_defIndex < 0)
    {
        m_defIndex = index;
        for(int i=0; i<m_posBtnList.length(); i++)
        {
            m_posBtnList.at(i)->setSheetStyle(QString("background: rgb(108, 108, 108); font: 75 8pt"));
        }
        return;
    }
    if(m_defIndex != index)
    {
        m_defBtnList.at(m_defIndex)->setChecked(false);
        m_defIndex = index;
    }
    for(int i=0; i<m_posBtnList.length(); i++)
    {
        m_posBtnList.at(i)->setSheetStyle(QString("background: rgb(108, 108, 108); font: 75 8pt"));
        m_posBtnList.at(i)->setChecked(false);
    }
}

void NgDialog::slot_onPosBtnClicked(QString text)
{
    if(text == QString(""))
    {
        return;
    }
    QString strDef = m_defBtnList.at(m_defIndex)->text();
    QString strText = ui->label->text();
    if(strText != QString(""))
    {
        ui->label->setText(strText + ";" + QString("[%1:%2]").arg(strDef).arg(text));
    }
    else
    {
        ui->label->setText(QString("[%1:%2]").arg(strDef).arg(text));
    }

    m_posList.append(text);
    m_mesDefList.append(strDef);
}

void NgDialog::on_btnQA_clicked()
{
    m_level = "QA";
    ui->btnQA->setEnabled(false);
    ui->btnQB->setEnabled(true);
}

void NgDialog::on_btnQB_clicked()
{
    m_level = "QB";
    ui->btnQB->setEnabled(false);
    ui->btnQA->setEnabled(true);
}
