#include "ImageSetDialog.h"
#include "ui_ImageSetDialog.h"
#include <QTableWidgetItem>
#include "ComboBox.h"
#include <QLineEdit>
#include <QIntValidator>
#include "model/LineEdit.h"

#include <QDebug>

ImageSetDialog::ImageSetDialog(QMap<QString, QList<double> > &imgParamMap, QMap<QString,QString> &eqpProductMap, const QStringList &orderList, QWidget *parent) :
    QDialog(parent), m_imgParamMap(imgParamMap), m_equipProductMap(eqpProductMap), m_orderList(orderList),
    ui(new Ui::ImageSetDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("图像参数设置"));
    initUi();
}

ImageSetDialog::~ImageSetDialog()
{
    delete ui;
}

void ImageSetDialog::initUi()
{
    QStringList headers;
    headers<<QStringLiteral("设备号")<<QStringLiteral("产品批次")<<QStringLiteral("垂直标尺")<<QStringLiteral("水平片数")
          <<QStringLiteral("垂直片数")<<QStringLiteral("顶部黑边像素")<<QStringLiteral("底部黑边像素")
         <<QStringLiteral("左边黑边像素")<<QStringLiteral("右边黑边像素");
    ui->tableWidget->setRowCount(m_imgParamMap.count());
    ui->tableWidget->setColumnCount(headers.length());
    ui->tableWidget->setAutoScroll(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->verticalHeader()->setVisible(false);

    QMap<QString,QList<double>>::iterator it;
    int i = 0;
    for(it = m_imgParamMap.begin(); it != m_imgParamMap.end(); it++)
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(it.key());
        ui->tableWidget->setItem(i, 0, item);
        QList<double> list = it.value();
        qDebug()<<"list "<<list;
        for(int j=0; j<ui->tableWidget->columnCount() - 1; j++)
        {
            if(j == 0)
            {
                ComboBox *cmbBox = new ComboBox;
                cmbBox->clear();
                cmbBox->addItems(m_orderList);
                QString product = m_equipProductMap.value(it.key());
//                int index = 0;
//                if(m_orderList.length() > j)
//                {
//                    index = list.at(j);
//                }
                cmbBox->setCurrentText(product);
                ui->tableWidget->setCellWidget(i, j+1, cmbBox);
            }
            else if(j == 1)
            {
                ComboBox *cmbBox = new ComboBox;
                cmbBox->clear();
                QStringList items;
                items<<QStringLiteral("顺序")<<QStringLiteral("逆序");
                cmbBox->addItems(items);
                int index = 0;
                if(list.length() > j-1)
                {
                    index = list.at(j-1);
                }
                cmbBox->setCurrentText(items.at(index));
                ui->tableWidget->setCellWidget(i, j+1, cmbBox);
                qDebug()<<QString("row : %1, column : %2").arg(QString::number(i)).arg(QString::number(j+1));
                qDebug()<<QStringLiteral("标尺顺序");
            }
            else
            {
                LineEdit *lineEdit = new LineEdit;
                lineEdit->setText("0");
                if(list.length() > j-1)
                {
                    lineEdit->setText(QString::number(list.at(j-1)));
                }
                ui->tableWidget->setCellWidget(i, j+1, lineEdit);
            }
        }
        i++;
    }
}

QMap<QString,QList<double>> ImageSetDialog::getImageParamMap() const
{
    return m_imgParamMap;
}

QMap<QString, QString> ImageSetDialog::getEquipProductMap() const
{
    return m_equipProductMap;
}

void ImageSetDialog::on_btnOk_clicked()
{
    m_imgParamMap.clear();
    m_equipProductMap.clear();

    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QString key = ui->tableWidget->item(i, 0)->text();
        if(key != "")
        {
            QList<double> list;
            QString product;
            for(int j=1; j<ui->tableWidget->columnCount(); j++)
            {
                if(j == 1)
                {
                    ComboBox *cmbBox = (ComboBox*)ui->tableWidget->cellWidget(i, j);
                    product = cmbBox->currentText();
                }
                else if(j == 2)
                {
                    ComboBox *cmbBox = (ComboBox*)ui->tableWidget->cellWidget(i, j);
                    int index = cmbBox->currentIndex();
                    list.append((double)index);
                }
                else
                {
                    LineEdit *lineEdit = (LineEdit*)ui->tableWidget->cellWidget(i, j);
                    QString text = lineEdit->text();
                    double value = 0;
                    if(text != "")
                    {
                        value = text.toDouble();
                    }
                    list.append(value);
                }
            }
            m_equipProductMap.insert(key, product);
            m_imgParamMap.insert(key, list);
        }
    }

    this->accept();
}

void ImageSetDialog::on_btnCancel_clicked()
{
    this->reject();
}

void ImageSetDialog::on_btnAdd_clicked()
{
    int row = ui->tableWidget->rowCount() + 1;
    ui->tableWidget->setRowCount(row);
    QTableWidgetItem *item = new QTableWidgetItem;
    ui->tableWidget->setItem(row - 1, 0, item);
}

void ImageSetDialog::on_btnDelete_clicked()
{
    ui->tableWidget->removeRow(m_curRow);
}

void ImageSetDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    m_curRow = index.row();
}
