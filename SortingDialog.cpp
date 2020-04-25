#include "SortingDialog.h"
#include "ui_SortingDialog.h"
#include "com/clsSettings.h"
#include <QComboBox>
#include <QTableWidgetItem>
#include <QCursor>
#include "ActionDialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QCheckBox>
#include "ItemForm.h"
#include <QTreeWidgetItemIterator>
#include <QTreeWidgetItemIterator>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "TabSpinBox.h"
#include "PointForm.h"
#include "DoubleSpinForm.h"
#include "ComboBox.h"
#include <QMessageBox>
#include <QLabel>
#include <QIcon>

#include <QDebug>

SortingDialog::SortingDialog(QMap<QString, SortMap> sortMap, const QMap<QString, QStringList> equipSortMap, const QMap<QString,QString> &eqpProdcutMap, QStringList sortList, QStringList horHeader, QStringList verHeader, QStringList pcList, const QStringList &dgList, QWidget *parent) :
    QDialog(parent), m_equipProductMap(eqpProdcutMap),
    ui(new Ui::SortingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("分选设置"));
   // this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    m_isInit = true;
    m_sortIndex = 0;

    m_sortMap = sortMap;
    m_equipSortMap = equipSortMap;
    m_horHeader = horHeader;
    m_verHeader = verHeader;
    m_sortList = sortList;
    m_pcList = pcList;

    m_addAction = new QAction(QStringLiteral("添加"), ui->treeWidget);
    m_delAction = new QAction(QStringLiteral("删除"), ui->treeWidget);
    m_modifyAction = new QAction(QStringLiteral("修改"), ui->treeWidget);
    m_menu = new QMenu(ui->treeWidget);
    m_menu->addAction(m_addAction);
    m_menu->addAction(m_delAction);
    m_menu->addAction(m_modifyAction);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slot_onWidgetContexMenu(QPoint)));
    connect(m_addAction,SIGNAL(triggered(bool)),this,SLOT(slot_addActionClicked()));
    connect(m_delAction,SIGNAL(triggered(bool)),this,SLOT(slot_delActionClicked()));
    connect(m_modifyAction,SIGNAL(triggered(bool)),this,SLOT(slot_modifyActionClicked()));

  //  initEquipTreeWidget();
 //   ui->tw_equip->setVisible(false);

    initDgLevels(dgList);
    initTreeWidget(m_curEquip);
    initUi();
}

SortingDialog::SortingDialog(QMap<QString, SortMap> sortMap)
{

}

SortingDialog::~SortingDialog()
{
    delete ui;
}

QList<QStringList> SortingDialog::getStandard()
{
    return m_stdList;
}

QStringList SortingDialog::getSortList()
{
    return m_sortList;
}

void SortingDialog::setSortList(QStringList sortList)
{
    this->m_sortList = sortList;
}

void SortingDialog::setStdList(QStringList stdList)
{

}

QMap<QString, QMap<QString,QStringList> > SortingDialog::getStdMap()
{
    return m_stdMap;
}

void SortingDialog::setStdMap(QMap<QString, QMap<QString, QStringList> > stdMap)
{
    this->m_stdMap = stdMap;
}

void SortingDialog::initDgLevels(const QStringList &dgList)
{
    m_dgLevels.clear();
    QStringList levels;
    levels<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T";

    for(int i=0; i<dgList.length(); i++)
    {
        QString level = levels.at(i);
        m_dgLevels.append(level);
    }
}

void SortingDialog::itemPressed_slot(QTreeWidgetItem *item, int column)
{
    if(qApp->mouseButtons() == Qt::RightButton)
    {
        m_menu->exec(QCursor::pos());
    }
}

void SortingDialog::slot_onWidgetContexMenu(QPoint point)
{
    QTreeWidgetItem *item = ui->treeWidget->itemAt(point);
    int x = point.x();
    int y = point.y() + 45;
    QPoint tmpPoint(x, y);
    if(item)
    {
        m_menu->exec(ui->treeWidget->mapToGlobal(tmpPoint));
    }
}

void SortingDialog::slot_addActionClicked()
{
    ActionDialog *dlg = new ActionDialog(this);
    if(dlg->exec() ==  QDialog::Accepted)
    {
        QString node = dlg->getNode();
        m_sortList.removeAll("");
        m_sortList.append(node);

        QStringList sortList = m_equipSortMap.value(m_curEquip);
        sortList.append(node);
        m_equipSortMap.insert(m_curEquip, sortList);
        QMap<QString,QStringList> listMap;
        SortMap sortMap = m_sortMap.value(m_curEquip);
        sortMap.insert(node, listMap);
        m_sortMap.insert(m_curEquip, sortMap);
        m_stdMap.insert(node, listMap);
        emit sig_addSorting(m_curEquip, m_sortList);

        updateSortList();

        initTreeWidget(m_curEquip);
    }
}

void SortingDialog::slot_delActionClicked()
{
    if(m_sortIndex < 0)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("选择要删除的分选"), QMessageBox::Ok);
        return;
    }
    QMessageBox::StandardButton stdBtn;
    stdBtn = QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("确定删除该分选？"),QMessageBox::Ok | QMessageBox::Cancel);
    if(stdBtn == QMessageBox::Cancel)
    {
        return;
    }

    m_stdMap.remove(m_sortList.at(m_sortIndex));
    m_sortList.removeAt(m_sortIndex);
    SortMap sortMap = m_sortMap.value(m_curEquip);
    sortMap.remove(m_sortText);
    m_sortMap.insert(m_curEquip, sortMap);
    QStringList sortList = m_equipSortMap.value(m_curEquip);
    sortList.removeAt(m_sortIndex);
    m_equipSortMap.insert(m_curEquip, sortList);

    m_sortIndex = -1;
    emit sig_deleteSorting(m_curEquip, m_sortText, m_sortList);

    updateSortList();

    initTreeWidget(m_curEquip);
}

void SortingDialog::slot_modifyActionClicked()
{
    ActionDialog *dlg = new ActionDialog(this);
    if(m_sortIndex < 0)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("选择要修改的分选"), QMessageBox::Ok);
        return;
    }
    QString node = m_sortList.at(m_sortIndex);

    dlg->setNode(node);
    if(dlg->exec() == QDialog::Accepted)
    {
        QString node1 = dlg->getNode();
        if(node == node1)
            return;
        QMap<QString,QStringList> listMap = m_stdMap.value(node);
        m_stdMap.remove(node);
        m_stdMap.insert(node1, listMap);
        m_sortList.replace(m_sortIndex, node1);
        m_equipSortMap.insert(m_curEquip, m_sortList);

        emit sig_modifySorting(m_curEquip, node, node1, m_sortList);

        updateSortMap(node, node1);
        updateSortList();

        initTreeWidget(m_curEquip);
    }
}

void SortingDialog::updateSortList()
{
    QMap<QString,QString>::iterator it;
    QString product = m_equipProductMap.value(m_curEquip);
    for(it = m_equipProductMap.begin(); it != m_equipProductMap.end(); it++)
    {
        if(it.value() == product)
        {
            QString  tmpKey = it.key();
            m_equipSortMap.insert(tmpKey, m_sortList);
        }
    }
}

void SortingDialog::updateSortMap(const QString &node, const QString &node1)
{
    QMap<QString,QString>::iterator it;
    QString product = m_equipProductMap.value(m_curEquip);
    SortMap sortMap = m_sortMap.value(m_curEquip);
    QMap<QString,QStringList> tmpMap = sortMap.value(node);
    sortMap.insert(node1, tmpMap);
    sortMap.remove(node);
    for(it = m_equipProductMap.begin(); it != m_equipProductMap.end(); it++)
    {
        if(it.value() == product)
        {
            QString equip = it.key();
            m_sortMap.insert(equip, sortMap);
        }
    }
}

void SortingDialog::readSettings(QString fileName)
{
    m_sortList<<QStringLiteral("等级1")<<QStringLiteral("等级2")<<QStringLiteral("等级3")<<QStringLiteral("等级4")
             <<QStringLiteral("等级5")<<QStringLiteral("等级6");
}

void SortingDialog::initUi()
{
    ui->tableWidget->clear();

    ui->tableWidget->setRowCount(m_verHeader.length());
    ui->tableWidget->setColumnCount(m_horHeader.length());
    ui->tableWidget->setAutoScroll(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(m_horHeader);
    ui->tableWidget->setVerticalHeaderLabels(m_verHeader);
    ui->tableWidget->verticalHeader()->setVisible(false);
    m_horHeaderView = ui->tableWidget->horizontalHeader();

    m_horHeaderView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_horHeaderView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slot_onTableWidgetContexMenu(QPoint)));
    connect(m_horHeaderView,SIGNAL(sectionClicked(int)),this,SLOT(slot_horHeaderCLicked(int)));

    m_tabMenu = new QMenu(ui->tableWidget);
    m_copyAction = new QAction(QStringLiteral("整列填充"), ui->tableWidget);
    m_tabMenu->addAction(m_copyAction);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slot_onTableWidgetContexMenu(QPoint)));
    connect(m_copyAction,SIGNAL(triggered(bool)),this,SLOT(slot_copyActionClicked()));


    SortMap sortMap = m_sortMap.value(m_curEquip);
    if(sortMap.isEmpty())
    {
        return;
    }

    m_sortList = m_equipSortMap.value(m_curEquip);
    QString node = m_sortList.at(m_sortIndex);

    QMap<QString,QStringList> tmpStdMap = sortMap.value(node);
    for(int i=0; i< m_verHeader.length(); i++)
    {
        QStringList list;

        if(tmpStdMap.count() != 0 && tmpStdMap.count() > i)
        {
            list = tmpStdMap.value(m_verHeader.at(i));
        }
        qDebug()<<"list "<<list;
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(m_verHeader.at(i));
        ui->tableWidget->setItem(i, 0, item);

        for(int j=0; j< m_horHeader.length(); j++)
        {
            QString text = "";
            if(list.length() > j && list.length() != 0)
            {
                text = list.at(j);
            }

            if(j <= 1)
            {
                ComboBox *box = new ComboBox();
                if(list.length() > (j))
                {
                    text = list.at(j);
                    box->setCurrentText(text);
                }
                ui->tableWidget->setCellWidget(i, j+1, box);
            }
            else if(j == 2)
            {
                ComboBox *box = new ComboBox();
                box->clear();
                box->addItems(m_dgLevels);
                if(list.length() > j)
                {
                    text = list.at(j);
                    box->setCurrentText(text);
                }
                ui->tableWidget->setCellWidget(i, j+1, box);
            }
            else if(j < 5)
            {
                QString strMin = "0";
                QString strMax = "0";
                if(list.length() > j)
                    strMin = list.at(2*(j-1) -1 );
                if(list.length() > j + 1)
                    strMax = list.at(2*(j-1) );
                DoubleSpinForm *spinBox = new DoubleSpinForm(this);
                spinBox->setMinValue(strMin.toDouble());
                spinBox->setMaxValue(strMax.toDouble());
                ui->tableWidget->setCellWidget(i, j + 1, spinBox);
            }
            else if(j == 5)
            {
                QString p1 = "0,0";
                QString p2 = "0,0";
                if(list.length() > j+2 )
                    p1 = list.at(j + 2 );
                if(list.length() > j +3)
                    p2 = list.at(j + 3);

                PointForm *form = new PointForm(this);
                form->setPoint1(p1);
                form->setPoint2(p2);

                ui->tableWidget->setCellWidget(i, j + 1, form);
            }
            else
            {
                QSpinBox *box = new QSpinBox();
                if(list.length() > j + 3)
                    text = list.at(j + 3);
                box->setValue(text.toInt());
                ui->tableWidget->setCellWidget(i, j + 1, box);
            }
        }
    }
}

void SortingDialog::initEquipTreeWidget()
{
//    ui->tw_equip->clear();

//    QStringList equipList = m_sortMap.keys();
//    if(equipList.isEmpty())
//    {
//        return;
//    }
//    qDebug()<<"len = "<<equipList.length();
//    for(int i=0; i<equipList.length(); i++)
//    {
//        QTreeWidgetItem *group = new QTreeWidgetItem(ui->tw_equip);
//        group->setText(0, equipList.at(i));
//        group->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |  Qt::ItemIsTristate);
//        group->setCheckState(0, Qt::Checked);

//        SortMap sortMap = m_sortMap.value(equipList.at(i));
//        qDebug()<<"fuck keys = "<<sortMap.keys();
//        SortMap::iterator it;
//        for(it = sortMap.begin(); it != sortMap.end(); it++)
//        {
//            QTreeWidgetItem *item = new QTreeWidgetItem(group);
//            item->setText(0, it.key());
//            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//            item->setCheckState(0, Qt::Checked);
//        }
//    }
//    m_curEquip = equipList.at(0);
}

void SortingDialog::initTreeWidget(QString equip)
{
    ui->treeWidget->clear();

    QStringList equipList = m_sortMap.keys();
    if(equipList.isEmpty())
    {
        return;
    }
    for(int i=0; i<equipList.length(); i++)
    {
        QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);
        root->setText(0, equipList.at(i));
        root->setIcon(0, QIcon(":/images/images/device.ico"));

        QTreeWidgetItem *group = new QTreeWidgetItem(root);
        QString product = m_equipProductMap.value(equipList.at(i));
        group->setText(0, product);
        group->setIcon(0, QIcon(":/images/images/product.ico"));

        QStringList keys = m_equipSortMap.value(equipList.at(i));
        for(int j = 0; j < keys.length(); j++)
        {
            if(keys.at(j) == "")
            {
                continue;
            }
            QTreeWidgetItem *item = new QTreeWidgetItem(group);
            item->setText(0, keys.at(j));
            item->setIcon(0, QIcon(":/images/images/item.ico"));
        }
    }
    ui->treeWidget->setItemsExpandable(true);
    ui->treeWidget->expandAll();

    if(m_isInit)
    {
        m_curEquip = equipList.at(0);
        m_isInit = false;
    }
}

void SortingDialog::updateTableWidget()
{
    ui->tableWidget->clear();

    ui->tableWidget->setRowCount(m_verHeader.length());
    ui->tableWidget->setColumnCount(m_horHeader.length());
    ui->tableWidget->setAutoScroll(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(m_horHeader);
    ui->tableWidget->setVerticalHeaderLabels(m_verHeader);
}

void SortingDialog::saveCurrentStd(QString sort)
{
    QMap<QString,QStringList> defMap;  //每个缺陷对应的标准
    int row = ui->tableWidget->rowCount();
    m_verHeader.clear();
    for(int i=0; i<row; i++)
    {
        QString text = ui->tableWidget->item(i, 0)->text();
        if(text == QString(""))
        {
            continue;
        }
        m_verHeader.append(text);
        QStringList horList;
        for(int j=1; j<m_horHeader.length(); j++)
        {
            QString text = "";
            if(j <= 3)
            {
                ComboBox *item = (ComboBox*)ui->tableWidget->cellWidget(i, j);
                text = item->currentText();
                horList.append(text);
            }
            else if(j < 6)
            {
                DoubleSpinForm *box = (DoubleSpinForm*)ui->tableWidget->cellWidget(i , j);
                double min = box->getMinValue();
                double max = box->getMaxValue();

                horList.append(QString::number(min));
                horList.append(QString::number(max));
            }
            else if(j == 6)
            {
                PointForm *form = (PointForm*)ui->tableWidget->cellWidget(i, j);
                QString p1 = form->getPoint1();
                QString p2 = form->getPoint2();

                QStringList list1 = p1.split(",");
                QString tmpStr1 = list1.at(0);
                QString tmpStr2 = list1.at(1);
                if(list1.at(0) == "")
                {
                    tmpStr1 = "0";
                }
                if(list1.at(1) == "")
                {
                    tmpStr2 = "0";
                }
                p1 = tmpStr1 + "," + tmpStr2;

                list1 = p2.split(",");
                tmpStr1 = list1.at(0);
                tmpStr2 = list1.at(1);
                if(list1.at(0) == "")
                {
                    tmpStr1 = "0";
                }
                if(list1.at(1) == "")
                {
                    tmpStr2 = "0";
                }
                p2 = tmpStr1 + "," + tmpStr2;

                horList.append(p1);
                horList.append(p2);
            }
            else
            {
                QSpinBox *box = (QSpinBox*)ui->tableWidget->cellWidget(i, j);
                int value = box->value();
                text = QString::number(value);
                horList.append(text);
            }
        }
        QString key = m_verHeader.at(i);
        defMap.insert(key, horList);
    }
    SortMap sortMap = m_sortMap.value(m_curEquip);
    sortMap.insert(sort, defMap);

    //更新和当前选择的产品批次，相同的分选标准
    QMap<QString,QString>::iterator itt;
    QString product = m_equipProductMap.value(m_curEquip);
    for(itt = m_equipProductMap.begin(); itt != m_equipProductMap.end(); itt++)
    {
        if(itt.value() == product)
        {
            QString  tmpKey = itt.key();
            m_sortMap.insert(tmpKey, sortMap);
        }
    }

    m_stdMap.insert(sort, defMap);
}

void SortingDialog::on_btnOk_clicked()
{
    this->accept();
}

void SortingDialog::on_btnCancel_clicked()
{
    this->reject();
}

void SortingDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QString text = item->text(column);

    if(item->parent() != NULL && item->parent()->parent() != NULL)
    {
        m_curEquip = item->parent()->parent()->text(0);
        m_sortList = m_equipSortMap.value(m_curEquip);
        int index = m_sortList.indexOf(text);
        m_sortIndex = index;
        m_sortText = text;

        initUi();
    }
    else if(item->parent() != NULL && item->parent()->parent() == NULL)
    {
        m_curEquip = item->parent()->text(0);
        m_sortList = m_equipSortMap.value(m_curEquip);
        m_sortIndex = -1;
    }
    else
    {
        m_sortIndex = -1;
        m_curEquip = item->text(0);
        m_sortList = m_equipSortMap.value(m_curEquip);
    }
}

//上移
void SortingDialog::on_btnUp_clicked()
{
    if(m_sortIndex == 0)
        return;
    QString text = m_sortList.at(m_sortIndex);
    m_sortList.removeAt(m_sortIndex);
    m_sortIndex--;
    m_sortList.insert(m_sortIndex, text);
    m_equipSortMap.insert(m_curEquip, m_sortList);

    emit sig_addSorting(m_curEquip, m_sortList);
    initTreeWidget(m_curEquip);
}

//下移
void SortingDialog::on_btnDown_clicked()
{
    if(m_sortIndex == m_sortList.length() - 1)
        return;
    QString text = m_sortList.at(m_sortIndex);
    m_sortList.removeAt(m_sortIndex);
    m_sortIndex++;
    m_sortList.insert(m_sortIndex, text);
    m_equipSortMap.insert(m_curEquip, m_sortList);

    emit sig_addSorting(m_curEquip, m_sortList);
    initTreeWidget(m_curEquip);
}

void SortingDialog::on_btnApply_clicked()
{
    if(m_sortText == QString(""))
    {
        QMessageBox::information(this,QStringLiteral("提示"), QStringLiteral("分选等级为空，选择分选等级"), QMessageBox::Ok);
        return;
    }
    saveCurrentStd(m_sortList.at(m_sortIndex));

    emit sig_applySorting(m_curEquip, m_verHeader, m_sortText, m_stdMap);


    return;





    QTreeWidgetItemIterator it(ui->treeWidget);
    QStringList equipList;
    QStringList sortList;

    sortList.clear();
    while (*it)
    {
        if ((*it)->checkState(0) == Qt::Checked && (*it)->parent() == NULL)
        {
            QString strEquip = (*it)->text(0);
            int count = (*it)->childCount();

            QStringList textList;
            QString strSort;
            for(int i=0; i < count; i++)
            {
                QString childText = (*it)->child(i)->text(0);
                textList.append(childText);

                if(i != count - 1)
                {
                    strSort += childText + ",";
                }
                else
                {
                    strSort += childText;
                }
            }

            if(!textList.contains(m_sortText))
            {
                QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText(0, m_sortText);
                (*it)->insertChild(count, item);
                item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setCheckState(0, Qt::Checked);
                strSort = strSort + "," + m_sortText;
                SortMap sortMap = m_sortMap.value(strEquip);
                sortMap.insert(m_sortText, m_stdMap.value(m_sortText));

                m_sortMap.insert(strEquip, sortMap);
            }

           // equipList.append(strEquip);
            sortList.append(strSort);
        }
        ++it;
    }
    emit sig_applySorting(equipList, m_verHeader, sortList, m_sortText, m_stdMap);
}

void SortingDialog::slot_onTableWidgetContexMenu(QPoint point)
{
    int x = point.x();
    int y = point.y() + 15;
    QPoint tmpPoint(x, y);

  //  int row = ui->tableWidget->horizontalHeader()->rootIndex()

    m_tabMenu->exec(ui->tableWidget->mapToGlobal(tmpPoint));
}

void SortingDialog::slot_copyActionClicked()
{

}

void SortingDialog::on_tableWidget_itemPressed(QTableWidgetItem *item)
{
    if(qApp->mouseButtons() == Qt::RightButton)
    {
        m_tabMenu->exec(QCursor::pos());
    }
}

void SortingDialog::on_tableWidget_pressed(const QModelIndex &index)
{

}

void SortingDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    m_curRow = index.row();
}

void SortingDialog::slot_horHeaderClicked(const QModelIndex &index)
{

}

void SortingDialog::slot_horHeaderCLicked(int index)
{
    m_curColumn = index;
}

void SortingDialog::on_tw_equip_itemDoubleClicked(QTreeWidgetItem *item, int column)
{

}

void SortingDialog::on_tw_equip_itemClicked(QTreeWidgetItem *item, int column)
{
    QString key;
    if(item->parent() != NULL)
    {
        key = item->parent()->text(column);
    }
    else
    {
        key = item->text(column);
    }

    m_curEquip = key;
    initTreeWidget(key);
}

void SortingDialog::on_btnAdd_clicked()
{
    int row = ui->tableWidget->rowCount() + 1;
    int column = ui->tableWidget->columnCount();
    ui->tableWidget->setRowCount(row);

    QTableWidgetItem *item = new QTableWidgetItem;
    ui->tableWidget->setItem(row - 1, 0, item);
    for(int j=0; j< m_horHeader.length(); j++)
    {
        if(j <= 1)
        {
            ComboBox *box = new ComboBox();

            ui->tableWidget->setCellWidget(row-1, j+1, box);
        }
        else if(j == 2)
        {
            ComboBox *box = new ComboBox();
            box->clear();
            box->addItems(m_dgLevels);
            ui->tableWidget->setCellWidget(row-1, j+1, box);
        }
        else if(j >2 && j < 5)
        {
            QString strMin = "0";
            QString strMax = "0";

            DoubleSpinForm *spinBox = new DoubleSpinForm(this);
            spinBox->setMinValue(strMin.toDouble());
            spinBox->setMaxValue(strMax.toDouble());
            ui->tableWidget->setCellWidget(row-1, j + 1, spinBox);
        }
        else if(j == 5)
        {
            QString p1 = "0,0";
            QString p2 = "0,0";

            PointForm *form = new PointForm(this);
            form->setPoint1(p1);
            form->setPoint2(p2);

            ui->tableWidget->setCellWidget(row-1, j + 1, form);
        }
        else
        {
            QSpinBox *box = new QSpinBox();
            box->setValue(0);
            ui->tableWidget->setCellWidget(row-1, j + 1, box);
        }
    }
}

void SortingDialog::on_btnDel_clicked()
{
    ui->tableWidget->removeRow(m_curRow);
}

void SortingDialog::on_btnSave_clicked()
{
    ActionDialog *dlg = new ActionDialog();
    if(dlg->exec() == QDialog::Accepted)
    {
        if(dlg->getNode() == "")
        {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("批次不能为空"), QMessageBox::Ok);
            return;
        }
        else
        {
            QStringList pruducts = m_equipProductMap.values();
            QString node = dlg->getNode();
            if(pruducts.contains(node))
            {

            }
            else
            {
                emit sig_applySorting(m_curEquip, m_verHeader, m_sortText, m_stdMap);
            }
        }
    }
}
