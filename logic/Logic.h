#ifndef LOGIC_H
#define LOGIC_H

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
//#include "mainwindow.h"

class MainWindow;

class Logic : public QObject
{
    Q_OBJECT

public:
    explicit Logic(MainWindow* window, QMap<QString, ClientForm *> &clientMap);
    ~Logic();

//    virtual void sendImageToAI(const ClientParam &param);
//    virtual void showImage(const int index, const QImage &image);
    virtual void parseData(const int handle, ClientParam &param, const QMap<QString,QString> &lineMap,
                           const QByteArray &data);

    bool judge(bool value);

    QMap<QString, QList<double> > getImgParamMap() const;

signals:
    void sig_updateTb_client(QString, int, bool);
    void sig_clientSwitch(int, QString, bool);

    void sig_showImage(const QImage &image);
    void sig_btnsDisabled(bool);

public slots:
    void slot_connected(qintptr handle, QTcpSocket* socket);
    void slot_disconnected(qintptr handle);
    void slot_readData();
    virtual void slot_aiResult(bool isOk, bool bTimeout, QString url, QString level, QString imgPath, int handle, QImage img, QStringList defList, QStringList posList,
                                           QStringList clsList, AiDataMap areaMap, AiDataMap lenMap, QMap<QString,QStringList> posMap);

    void slot_aiDisconnect(QString url, int handle);
    void slot_sortResult(int handle, bool isOk, QString level);
protected:
    QString getUrl();

protected:
    TcpServer *m_tcpServer;
    MainWindow *m_window;

    int m_row;
    int m_column;
    QMap<QString,ClientForm*> m_clientMap;
    QMap<int,QTcpSocket*> m_socketMap;

  //  QMap<int,HeartTimer*> m_timerMap;
    int m_handle;
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
    QMap<QString,int> m_imgIndexMap;
    QMap<QString,QImage*> m_imgMap;

    ExcelSaver *m_excelSaver;
  //  QTimer *m_heartTimer;

    QString m_shareIP;
    QString m_shareFolderName;

    void sendImageToAi(QString &imgPath, QString &url, int handle, int index);
    void sendNextImageToAi();
    void showNextImage();
    void updateImgWidget(int column);
    void readSettings();
    void initDB();
};

#endif // LOGIC_H
