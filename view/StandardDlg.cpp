#include "StandardDlg.h"
#include "ui_StandardDlg.h"
#include <QCheckBox>
#include "DoubleSpinForm.h"
#include "PointForm.h"
#include <QSpinBox>
#include "ComboBox.h"

StandardDlg::StandardDlg(const QStringList &horHeaders, const QStringList &verHeader, const QMap<QString, QStringList> &sortMap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StandardDlg)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("分选标准"));
    initUi(horHeaders, verHeader, sortMap);
}

StandardDlg::~StandardDlg()
{
    delete ui;
}

void StandardDlg::initUi(const QStringList &horHeader, const QStringList &verHeader, const QMap<QString, QStringList> &sortMap)
{
    ui->tableWidget->clear();

    ui->tableWidget->setRowCount(verHeader.length());
    ui->tableWidget->setColumnCount(horHeader.length());
    ui->tableWidget->setAutoScroll(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(horHeader);
    ui->tableWidget->setVerticalHeaderLabels(verHeader);
    ui->tableWidget->verticalHeader()->setVisible(false);

   // m_horHeaderView = ui->tableWidget->horizontalHeader();
    for(int i=0; i< verHeader.length(); i++)
    {
        QStringList list;
        if(sortMap.count() != 0 && sortMap.count() > i)
        {
            list = sortMap.value(verHeader.at(i));
        }
        for(int j=0; j< verHeader.length(); j++)
        {
            QString text = "";
            if(list.length() > j && list.length() != 0)
            {
                text = list.at(j);
            }

            if(j == 0)
            {
                QCheckBox *box = new QCheckBox();
                bool isChecked = false;

                if(text == "true")
                {
                    isChecked = true;
                }
                box->setChecked(isChecked);
                box->setText(verHeader.at(i));
                ui->tableWidget->setCellWidget(i, 0, box);
            }
            else if(j < 3)
            {
                QString strMin = "0";
                QString strMax = "0";
                if(list.length() > 2*j - 1)
                    strMin = list.at(2*j - 1);
                if(list.length() > 2*j)
                    strMax = list.at(2*j);
                DoubleSpinForm *spinBox = new DoubleSpinForm(this);
                spinBox->setMinValue(strMin.toDouble());
                spinBox->setMaxValue(strMax.toDouble());
                ui->tableWidget->setCellWidget(i, j, spinBox);
            }
            else if(j == 3)
            {
                QString p1 = "0,0";
                QString p2 = "0,0";
                if(list.length() > 2*j - 1)
                    p1 = list.at(2*j - 1);
                if(list.length() > 2*j - 1)
                    p2 = list.at(2*j);

                PointForm *form = new PointForm(this);
                form->setPoint1(p1);
                form->setPoint2(p2);

                ui->tableWidget->setCellWidget(i, j, form);
            }
            else if(j < horHeader.length() - 1)
            {
                QSpinBox *box = new QSpinBox();
                if(list.length() > j + 3)
                    text = list.at(j + 3);
                box->setValue(text.toInt());
                ui->tableWidget->setCellWidget(i, j, box);
            }
            else
            {
                ComboBox *box = new ComboBox();
                if(list.length() > (j + 3))
                {
                    text = list.last();
                    box->setCurrentText(text);
                }
                ui->tableWidget->setCellWidget(i, j, box);
            }
        }
    }
}

void StandardDlg::on_btnCancel_clicked()
{
    this->reject();
}

void StandardDlg::on_btnOk_clicked()
{
    this->accept();
}
