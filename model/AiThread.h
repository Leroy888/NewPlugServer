#ifndef AITHREAD_H
#define AITHREAD_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <QMap>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTimer>
#include "SortThread.h"
#include "com/Functions.h"
#include <QSqlDatabase>


class AiThread : public QThread
{
    Q_OBJECT
public:
    AiThread(QString &imgPath, QString strUrl, int horNum, int verNum, int handle);
    ~AiThread();

public:
    void run();
  //  static AiThread* getInstance();
    void setParam(QImage &img, QString strUrl, int horNum, int verNum);
    void setParams(SortMap &sortMap,  QSqlDatabase &db, const QSqlDatabase &db2, const QString &wkshop,
                   QStringList sortList,  ClientParam &param,  QString savePath,  QString &code,
                   const QStringList &dgList);

    void readAiModel(QString fileName);
    void setAiModel(QMap<int, QString> &defMap, QMap<QString,QString> &clsMap);
    void setPixels(const int top, const int bottom, const int left, const int right);

    bool waitForConnect();

    void sleepMs(int ms);
    void setImageParam(const int horPic, const int verPic, const int topPix, const int btmPix, const int leftPix, const int rightPix, const int imgModel);
private:
        void drawImage(QList<AiResult> resList);
        void sort();
        bool sorting();
protected slots:
    void slot_waitTimeout();
    void slot_readFinished();

    void slot_sortResult(int handle,bool isOk, QString level);
    void slot_sort_info(bool res, QString info, QString level);

signals:
    void signal_waitTimeout();
    void sig_aiResult(bool isOk, bool bTimeout, QString url, QString level, QString imgPath, int hanlde, QImage img,
                      QStringList defList, QStringList posList, QStringList clsList, AiDataMap areaMap, AiDataMap lenMap,
                      QMap<QString,QStringList>);
    void sig_aiDisConnect(QString, int);
    void sig_aiInfo(QString);

    void sig_sortResult(int handle,bool isOk, QString level);
    void sig_sort_info(bool, QString info, QString level, int, QString);

protected:
    AiPoint getAiPoint(const AiResult &aiRes, int widPic, int heightPic);
private:
  //  static AiThread *m_aiThd;
    QString m_iniFile;
    QImage m_img;
    QString m_imgPath;
    QString m_strUrl;
    QString m_path; //图片保存路径
    QMap<int,QString> m_Maps;
    QMap<QString,QString> m_clsMap;
    QNetworkReply *m_replay;
  //  QNetworkAccessManager *m_manager;
    int m_waitTimeMs;
    bool m_isTimeout;
    int m_horNum;
    int m_verNum;
    int m_imgModel;

    int m_topPixel;  //图片的上部黑边
    int m_bottomPixel;  //图片的底部部黑边
    int m_leftPixel;    //图片的左部黑边
    int m_rightPixel;   //图片的右部黑边

    QString m_wkshop;

    ImgParam m_imgParam;

    QString m_sortInfo;
    QString m_level;

    bool m_isOk;
    QStringList m_clsList;
    QStringList m_defectList;
    QStringList m_posList;
    QStringList m_piecPosList;  //缺陷所在的
    QStringList m_addrList;     //缺陷所在区域列表

    QMap<QString,QMap<QString,int>> m_countPieceMap;  //单个电池片上的相应的缺陷个数
    QMap<QString,int> m_countMap; //整个组件上相应的缺陷个数 QString 1:缺陷, QString 2:位置（A1、A2）, int:某一缺陷在某一位置的个数
//    QList<float> m_areaList;    //面积
//    QList<float> m_lenList;

    QTimer *m_timer;
    QTimer *testTimer;
    int m_isStop;
    int m_handle;
    QStringList m_sortList;
    QStringList m_dgList;

    SortMap m_sortMap;
    QSqlDatabase m_db;
    QSqlDatabase m_db2;
    QString m_code;
    ClientParam m_param;
    QString m_savePath;

    QMap<QString,QList<double>> m_piecAreaMap;  //单片电池片上 某一缺陷对应的面积列表
    AiDataMap m_areaMap;      //单片电池片上某一缺陷对应的长度列表
    AiDataMap m_lenMap;    //单个带电池片缺陷对应的长度列表
    AiDataMap m_dgMap;      //灰度值
    AiPointMap m_pointMap;  //坐标
    QMap<QString,QStringList> m_posMap; //缺陷所在位置 A1、A2等
    QMap<QString,int> m_piecNumMap;     //电池片上某一缺陷的总数
    QMap<QString,int> m_numMap;         //组件上某一缺陷的总数
    QMap<QString,int> m_ngNumMap;       //一个组件对应的缺陷损坏的电池片数

};

#endif // AITHREAD_H
