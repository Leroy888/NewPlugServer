#ifndef SORTTHREAD_H
#define SORTTHREAD_H

#include <QObject>
#include <QThread>
#include "SortingDialog.h"
#include "model/AiThread.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QImage>
#include "com/Functions.h"

class SortThread : public QThread
{
    Q_OBJECT
public:
    SortThread(const SortMap &sortMap, int handle, QString &imgPath, QStringList sortList, QStringList defList, AiDataMap &areaMap,
               AiDataMap &lenMap, QMap<QString,QStringList> posMap);

    SortThread();

    void setDefects(QStringList sortList, QStringList defList,AiDataMap &areaMap,
                    AiDataMap &lenMap, QMap<QString,QStringList> posMap);
    void setSortMap(const SortMap &sortMap);
    void setDB(const QSqlDatabase &db);
    void setCode(const QString &code);
    void setAiReslut(const bool isOk);
    void setParams(const QSqlDatabase &db, const QSqlDatabase &db2, const QString &wkshop, const ClientParam &param, const QString savePath, const QImage &img,
                  const QString &code, const bool isOk, const QStringList &dgList);
    void setParams(const SortMap &sortMap, const QSqlDatabase &db, const QSqlDatabase &db2, const ClientParam &param,
                   const QString savePath, const QString &code, const QStringList &dgList);
    void run();
    bool sorting();

    void setDateTime(const int ms);
    void setPosList(const QStringList &list);
    void setClsList(const QStringList &list);
    void setClsMap(const QMap<QString,QString> clsMap);
    void setMaps(const AiDataMap &dgMap, const AiPointMap &ptMap);
signals:
    void sig_sort_result(int handle, bool isOk, QString level);
    void sig_sort_info(bool,QString info, QString level);

protected:
    void saveSqlData();

    void saveImage();
    void insertResult(const QString &sql, const QString &sql2);
    void saveExcel(const QStringList &header, const QStringList &dataList, const QString &path);
private:
    int m_handle;
    QString m_level;
    QString m_defect;
    QStringList m_sortList;
    QStringList m_defList;
    QList<float> m_areaList;
    QList<float> m_lenList;
    QString m_imgPath;
    SortMap m_sortMap;

    AiDataMap m_areaMap;
    AiDataMap m_lenMap;
    AiDataMap m_dgMap;
    AiPointMap m_pointMap;  //坐标位置
    QMap<QString,QStringList> m_posMap;

    bool m_aiOk;    //AI判断的结果
    bool m_isOk;    //分选判断的结果
    ClientParam m_param;
    QString m_wkshop;
    QString m_savePath;
    QImage m_image;

    QString m_csvPath;


    QString m_sortInfo;

    bool m_isBreak;

    QSqlDatabase m_db;
    QSqlDatabase m_db2;
    QString m_code;
    int m_timeMs;
    QStringList m_posList;
    QStringList m_clsList;
    QStringList m_dgList;
    QStringList m_dgLevels;

    QMap<QString,QString> m_clsMap;
};

#endif // SORTTHREAD_H
