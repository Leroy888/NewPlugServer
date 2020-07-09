#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QObject>
#include <QImage>
#include <QMap>
#include <QStringList>
#include <QList>

struct SaveData
{
    int okNum;
    int ngNum;
    int errorNum;
    int missNum;
    float ruteError;
    float ruteMiss;
    QString strCls;
    int ylNum;
    int xhNum;
    int ylXhNum;
    int otherNum;
};

struct ClientParam
{
    int handle;
    int okNum;
    int ngNum;
    int missNum;
    int errorNum;
    QString url;
    QString info;
    QString line;
    QString barCode;
};

struct AiStatus
{
    bool isBusy = false;
    bool bTimeout = false;
    int timeoutNum = 0;
};

enum IOStyle{
    ReadOnly,
    WriteOnly,
    ReadWrite,
    Appened
};

struct AiResult
{
    int clsId;  //缺陷ID
    int x1;
    int y1;
    int x2;
    int y2;
    float len;  //缺陷长度
    int dg; //对比度
};

struct AiPoint
{
    int x1;
    int y1;
    int x2;
    int y2;
};

typedef QMap<QString,QMap<QString,int>> PosMap;
typedef QMap<QString,QList<double>> AiDataMap;
typedef QMap<QString,QList<AiPoint>> AiPointMap;



struct ImgParam
{
    int upPixel;
    int bottomPixel;
    int leftPixel;
    int rightPixel;
};

class OptDefects
{
public:
    OptDefects() {}

    void clear();

    AiDataMap areaMap;      //单片电池片上某一缺陷对应的长度列表
    AiDataMap lenMap;    //单个带电池片缺陷对应的长度列表
    AiDataMap dgMap;      //灰度值
    AiPointMap pointMap;  //坐标
    QMap<QString,QStringList> posMap; //缺陷所在位置 A1、A2等
    QMap<QString,int> piecNumMap;     //电池片上某一缺陷的总数
    QMap<QString,int> numMap;         //组件上某一缺陷的总数
    QMap<QString,int> ngNumMap;       //一个组件对应的缺陷损坏的电池片数
};



class Functions
{
public:
    Functions();

    static void sleepMs(int ms);
    static void mkDir(QString strPath);
    static QString stringListToString(const QStringList &list) ;

    static void saveImage(const QImage &img, const QString &savePath, const QString &imgName, const QString &pcInfo, const QString &strRes);
};

#endif // FUNCTIONS_H
