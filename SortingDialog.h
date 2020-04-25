#ifndef SORTINGDIALOG_H
#define SORTINGDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QHash>


typedef QMap<QString,QMap<QString,QStringList>> SortMap;

namespace Ui {
class SortingDialog;
}

class SortingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SortingDialog(QMap<QString, SortMap> sortMap, const QMap<QString, QStringList> equipSortMap, const QMap<QString, QString> &eqpProdcutMap, QStringList sortList, QStringList horHeader, QStringList verHeader, QStringList pcList, const QStringList &dgList, QWidget *parent = 0);
    SortingDialog(QMap<QString,SortMap> sortMap);
    ~SortingDialog();

    void initUi();
    void initEquipTreeWidget();
    QList<QStringList> getStandard();
    QStringList getSortList();
    void setSortList(QStringList sortList);
    void setStdList(QStringList stdList);
    QMap<QString, QMap<QString, QStringList> > getStdMap();
    void setStdMap(QMap<QString, QMap<QString, QStringList> > stdMap);


private:
    void readSettings(QString fileName);
    void initTreeWidget(QString equip);
    void updateTableWidget();
    void saveCurrentStd(QString sort);
    void initDgLevels(const QStringList &dgList);
    void updateSortMap(const QString &node, const QString &node1);
    void updateSortList();

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_btnUp_clicked();

    void on_btnDown_clicked();

    void itemPressed_slot(QTreeWidgetItem *item, int column);
    void slot_onWidgetContexMenu(QPoint point);
    void slot_addActionClicked();
    void slot_delActionClicked();
    void slot_modifyActionClicked();

    void on_btnApply_clicked();

    void slot_onTableWidgetContexMenu(QPoint point);
    void slot_copyActionClicked();

    void on_tableWidget_itemPressed(QTableWidgetItem *item);

    void on_tableWidget_pressed(const QModelIndex &index);

    void on_tableWidget_clicked(const QModelIndex &index);

    void slot_horHeaderClicked(const QModelIndex &index);
    void slot_horHeaderCLicked(int index);

    void on_tw_equip_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_tw_equip_itemClicked(QTreeWidgetItem *item, int column);

    void on_btnAdd_clicked();

    void on_btnDel_clicked();

    void on_btnSave_clicked();

signals:
    void sig_applySorting(QStringList, QStringList, QStringList, QString, SortMap);
    void sig_applySorting(QString, QStringList, QString, SortMap);
    void sig_addSorting(const QString&, QStringList);
    void sig_deleteSorting(const QString&, const QString&, QStringList);
    void sig_modifySorting(const QString&, const QString&, const QString&, QStringList);


private:
    Ui::SortingDialog *ui;

    bool m_isInit;
    QMenu *m_menu;
    QAction *m_addAction;
    QAction *m_delAction;
    QAction *m_modifyAction;

    QMenu *m_tabMenu;
    QAction *m_copyAction;
    QHeaderView *m_horHeaderView;
    int m_curColumn;

    QList<QStringList> m_stdList;   //标准
    QStringList m_horHeader;    //水平表头
    QStringList m_verHeader;    // 垂直表头
    QString m_fileName;
    QStringList m_sortList; //分选的策略
    int m_sortIndex;    //分选策略的index
    QString m_sortText;
    QStringList m_pcList;   //在线的机台
    QStringList m_selectPcList;

//    QMap<QString,QList<QStringList>> m_stdMap;
    SortMap m_stdMap;
    QMap<QString,SortMap> m_sortMap;
    SortMap m_curStdMap;    //当前选择的分选策略
    QString m_curEquip;     //当前选择的设备
    QTreeWidgetItem *m_group;
    QMap<QString,QStringList> m_equipSortMap;
    QStringList m_dgLevels;
    int m_curRow;
    QMap<QString, QString> m_equipProductMap;

};

#endif // SORTINGDIALOG_H
