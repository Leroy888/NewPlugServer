#include "EditDialog.h"
#include "ui_EditDialog.h"
#include <QTableWidgetItem>

#include <QDebug>

EditDialog::EditDialog(const QStringList &ports, const QString &header, QWidget *parent) :
    QDialog(parent), m_portList(ports), m_curRow(-1),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("端口设置"));
   // initUi(header);
}

EditDialog::~EditDialog()
{
    delete ui;
}

QStringList EditDialog::getList() const
{
    return m_portList;
}

//void EditDialog::initUi(const QString &header)
//{
//    ui->tableWidget->clear();
//    QStringList horHeader;
//    horHeader<<header;
//    ui->tableWidget->setRowCount(m_portList.length());
//    ui->tableWidget->setColumnCount(1);
//    ui->tableWidget->setAutoScroll(true);
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget->setHorizontalHeaderLabels(horHeader);
//    ui->tableWidget->verticalHeader()->setVisible(false);

//    for(int i=0; i<m_portList.length(); i++)
//    {
//        QTableWidgetItem *item = new QTableWidgetItem();
//        item->setText(m_portList.at(i));
//        ui->tableWidget->setItem(i, 0, item);
//    }
//}

void EditDialog::on_btnOk_clicked()
{
    m_portList.clear();
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QString text = ui->tableWidget->item(i, 0)->text();
        if(text != "")
        {
            m_portList.append(text);
        }
    }
    this->accept();
}

void EditDialog::on_btnCancel_clicked()
{
    this->reject();
}

void EditDialog::on_btnDelete_clicked()
{
    ui->tableWidget->removeRow(m_curRow);
}

void EditDialog::on_btnAdd_clicked()
{
    int row = ui->tableWidget->rowCount() + 1;
    ui->tableWidget->setRowCount(row);
    QTableWidgetItem *item = new QTableWidgetItem;
    ui->tableWidget->setItem(row - 1, 0, item);
}

void EditDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    m_curRow  = index.row();
    qDebug()<<"row = "<<m_curRow;
}
