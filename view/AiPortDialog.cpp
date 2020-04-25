#include "AiPortDialog.h"
#include "ui_AiPortDialog.h"
#include <QTableWidgetItem>
#include "model/LineEdit.h"

#include <QDebug>

AiPortDialog::AiPortDialog(const QStringList &ports, const QStringList &header, const QString &type, QWidget *parent) :
    QDialog(parent), m_portList(ports), m_curRow(-1), m_type(type),
    ui(new Ui::AiPortDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("端口设置"));
    initUi(header, type);
}

AiPortDialog::~AiPortDialog()
{
    delete ui;
}

QStringList AiPortDialog::getList() const
{
    return m_portList;
}

void AiPortDialog::initUi(const QStringList &header, const QString &type)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(m_portList.length());
    ui->tableWidget->setColumnCount(header.length());
    ui->tableWidget->setAutoScroll(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->verticalHeader()->setVisible(false);

    if(type == QString("LineEdit"))
    {
        QStringList list;
        list<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q";
        for(int i=0; i<m_portList.length(); i++)
        {
            for(int j=0; j<header.length(); j++)
            {
                LineEdit *item = new LineEdit();
                if(j == 0)
                {
                    item->setText(list.at(i));
                }
                else
                {
                    item->setText(m_portList.at(i));
                }
                ui->tableWidget->setCellWidget(i, j, item);
            }
        }
    }
    else
    {
        for(int i=0; i<m_portList.length(); i++)
        {
            for(int j=0; j< header.length(); j++)
            {
                QTableWidgetItem *item = new QTableWidgetItem();
                if(j == 0)
                {
                    item->setText(QString::number(i+1));
                }
                else
                {
                    item->setText(m_portList.at(i));
                }
                ui->tableWidget->setItem(i, j, item);
            }
        }
    }

}

void AiPortDialog::on_btnOk_clicked()
{
    m_portList.clear();
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QString text = ui->tableWidget->item(i, 1)->text();
        if(text != "")
        {
            m_portList.append(text);
        }
    }
    this->accept();
}

void AiPortDialog::on_btnCancel_clicked()
{
    this->reject();
}

void AiPortDialog::on_btnDelete_clicked()
{
    ui->tableWidget->removeRow(m_curRow);
}

void AiPortDialog::on_btnAdd_clicked()
{
    int row = ui->tableWidget->rowCount() + 1;
    ui->tableWidget->setRowCount(row);
    if(m_type == QString("LineEdit"))
    {
        QStringList list;
        list<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q";
        LineEdit *item = new LineEdit(true);
        item->setEnabled(false);
        item->setText(list.at(row - 1));
        ui->tableWidget->setCellWidget(row-1, 0, item);

        LineEdit *item2 = new LineEdit;
        item2->setText("0");
        ui->tableWidget->setCellWidget(row-1, 1, item2);
    }
    else
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString::number(row));
        ui->tableWidget->setItem(row - 1, 0, item);

        QTableWidgetItem *item2 = new QTableWidgetItem;
        ui->tableWidget->setItem(row - 1, 0, item2);
    }

}

void AiPortDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    m_curRow  = index.row();
    qDebug()<<"row = "<<m_curRow;
}
