#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "tcp_server.h"
#include "ClientForm.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QSqlDatabase>
#include <QButtonGroup>
#include "ImgForm.h"
#include <QTcpSocket>
#include "SortingDialog.h"
#include <QTimer>
#include <QMutex>
#include <QPixmap>
#include "SettingsDialog.h"
#include "model/AiThread.h"
#include "com/Functions.h"
#include "model/SortThread.h"
#include <QPoint>
#include <QHash>
#include "model/ExcelSaver.h"
#include <QCloseEvent>
#include "model/HeartTimer.h"
#include "logic/Logic.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initUi();
    void initTableWidget_data();
    void initDB2();
    void readEquip();
    void readSorting(QString equip, QStringList sortList);
    void initLogic();
    void update_Tb_client(int handle);
    void update_Tb_client(const QString &strInfo, const int handle, bool value);
    void drawLeftRuler(int num);
    void drawHorizontalRuler(int num);
    void showImage(QImage image);
    void sendResultToEl(QImage img, bool value, QStringList defList, QString strRes);
    void sendResultToEl(bool value, QString strRes);
    void getResult(bool value);
    void sendImageToAi(QString &imgPath, QString &url, int handle);
    void semdImageToAi(QString &imgPath, QString &url, int handle, SortThread &sortThd);

    // 更新分选标准到总控数据库
    void updateDB();

    void updateDB2Recipe(const QString &product, const QString &sort);
    void updateDB2Sort(const QString &product, const QString &sortText, QMap<QString, QStringList> &defMap);

    //插入返修表
    void insertPathEx(QSqlDatabase &db, const QString barCode, const QString code, const QString sharePath);
    void removeImageForm(ImgForm *form);
    void appendText(const QString &text);
    void updateAiDisconnected(const QString &url);
    void setBtnsEnabled(bool value);
signals:
    void ServerRecved(qintptr, QTcpSocket*, const QByteArray &);
    void readyReadData(qintptr, QTcpSocket*);
protected:
    void closeEvent(QCloseEvent *event);
    void updateImgWidget(int column);
    void sorting();
    void init_Tb_client();
    void insertResult(QString sql);
    void showNextImage();
    void saveExcel(QString &path, QString &code);
    QString getUrl();
    QString stringListToString(QStringList list);
    void sendNextImageToAi();
protected slots:
    void slot_modifySorting(const QString &equip, const QString &node, const QString &node1, const QStringList &sortList);
    void slot_addSorting(const QString &equip, const QStringList &sortList);
private slots:
    void ClientConnected(qintptr handle, QTcpSocket *socket);
    void ClientDisconnected(qintptr handle);
    /**
     * @brief 服务端收到消息的信号
     *    若想要统一管理或做日志处理可连接此信号
     * @param 收到消息的连接句柄
     * @param 收到消息的socket指针
     * @param 收到的消息内容
     */
    void ServerRecvedSlot(qintptr handle, QTcpSocket *socket, const QByteArray &data);
    void on_btnOk_clicked();

    void on_btnQuery_clicked();

    void readData(qintptr handle, QTcpSocket *socket);
    void slot_imgFormClicked(QWidget* wid, int handle);
    void slot_imgFormClicked(QPoint &p, QImage &image, int handle, QString &path);

    void on_btnQueryDefect_clicked();

    void on_btnSort_clicked();

    void on_btnNg_clicked();

    void slot_applySorting(QStringList equipList, QStringList verHeader, QStringList sortList, QString curText, SortMap sortMap);
    void slot_applySorting(QString equip, QStringList verHeader, QString sortText, SortMap sortMap);
    void slot_deleteSorting(const QString &equip, const QString &curSort, const QStringList &sortList);

    void slot_aiResult(bool isOk, bool bTimeout, QString url, QString level, QString imgPath, int handle, QImage img, QStringList defList, QStringList posList,
                       QStringList clsList, AiDataMap areaMap, AiDataMap lenMap, QMap<QString, QStringList> posMap);//AI返回结果
    void on_btnPic_clicked();
    void slot_aiDisconnect(QString url, int handle);

    void slot_sortResult(int handle, bool isOk, QString level);
    void slot_sort_info(bool res, QString info, QString level, int handle, QString url);
    void on_btnList_clicked();

    void on_btnData_clicked();

    void on_btnSettings_clicked();

    void slot_aiJudge();

    void on_btnAiUrlSet_clicked();

    void on_tableWidget_data_doubleClicked(const QModelIndex &index);

    void on_btnImgSet_clicked();
    void slot_clientSwitch(const int handle, QString info, const bool value);

    void slot_clear(int handle);
    void slot_heartTimeout(int handle);
    void on_btnUpdate_clicked();

private:
    Ui::MainWindow *ui;

    Logic* m_logic;

    int m_row;
    int m_column;
    QMap<QString,ClientForm*> m_clientMap;
    QMap<int,QTcpSocket*> m_socketMap;

  //  QMap<int,HeartTimer*> m_timerMap;
    int m_handle;
    QGraphicsPixmapItem* m_pixmapItem;
    QGraphicsScene *m_graphScene;
    float m_fScale;
    int m_horPieces;
    int m_verPieces;
    int m_topHight;
    int m_topPixel;  //图片的上部黑边
    int m_bottomPixel;  //图片的底部部黑边
    int m_leftPixel;    //图片的左部黑边
    int m_rightPixel;   //图片的右部黑边

    QMap<int,QString> m_defMap;
    QMap<QString,QString> m_clsMap;

    QString m_savePath;
    QString m_savePath2;    //共享目录
    QImage m_curImg;    //当前选择的图片
    int m_curNgImgIndex;    //当前选择的ng图片的index
    ImgForm *m_ImgForm; // 当前选择的imgForm
    QString m_curImgPath;
    QList<ImgForm*> m_imgFormList;
    QStringList m_imgList;
    QSqlDatabase m_db;
    QSqlDatabase m_db2; //保存当前分选标准到总控数据库
    QString m_dbIp;
    QButtonGroup *m_btnGroup;
    QString m_strOrder;
    QStringList m_productList;
    QStringList m_dgList;
    QString m_wkshop;
    int m_imgModel;
    int m_ngDlgModel;

    QList<QStringList> m_stdList;   //标准
    QStringList m_horHeader;    //水平表头
    QStringList m_verHeader;    // 垂直表头
    QString m_fileName;
    QStringList m_sortList; //分选的策略
    int m_sortIndex;    //分选策略的index
    QStringList m_pcList;   //在线设备信息列表
    QStringList m_defList;  //缺陷列表
    QStringList m_posList;  //缺陷位置列表
    QStringList m_clsList;  //缺陷分类
    QList<float> m_areaList;    //缺陷面积列表
    QList<float> m_lenList;     //缺陷长度列表
    QMap<int,ClientParam> m_paramMap;   //外挂客户端参数
    int m_stdNum;
    int m_sortNum;  //统计分选的片数，达到88时，清空输出信息

    QMap<QString,QList<double>> m_imgParamMap;

//    QMap<QString,QList<QStringList>> m_stdMap;
    //分选策略
    SortMap m_stdMap;
    //SortMap是每个分选等级对应的分选策略
    QMap<QString,SortMap> m_sortMap;    //设备对应的分选策略
    QMap<QString,QStringList> m_equipSortMap;
    QMap<int,QString> m_handEquipMap;   //handle对应的设备
    QMap<QString,QStringList> m_equipPathMap;    //设备对应的图片存储的位置
    QMap<QString,bool> m_equipStatusMap;
   // QStringList m_imgPathList;  //
    QStringList m_mesDefList;

    QList<AiDataMap> m_areaMapList;
    QList<AiDataMap> m_lenMapList;
    QList<QMap<QString,QStringList>> m_posMapList;
    QList<QStringList> m_defectLists;
    QList<QStringList> m_positionLists;
    QList<QStringList> m_classLists;

    int m_testNum;
    QStringList m_testAiList;
    int m_testIndex;
    QList<QImage> m_ngImgList;
    QStringList m_ngPathList;
    QList<int> m_handleList;
    QStringList m_ngLevelList;

    bool m_isFinished;  //某一图片是否判断完成

    QMutex m_mutex;

    QStringList m_imgNameList;
    QImage m_image;
    int m_delaySecs;
    QTimer *m_aiTimer;
    bool m_aiOk;
    int m_modelIndex;

    int m_testModelIndex;

    //数据库
    DB_Param m_dbParam;
    DB_Param m_dbParam2;
    QStringList m_sqlHeader;

    QMap<QString, AiStatus> m_statusMap;    //AI端口状态列表
    QList<QMap<QString,int>>  m_unSendImgList;
    bool m_isInit;
    QMap<QString,QString> m_equipProductMap;    //每个设备对应的产品批次Map
    QMap<QString,QString> m_equipLineMap;   //设备对应的线别

    QString m_shareIP;
    QString m_shareFolderName;
};

#endif // MAINWINDOW_H
