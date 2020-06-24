#ifndef SAVER_H
#define SAVER_H

#include <QObject>
#include <QThread>
#include <QImage>
#include <QSqlDatabase>
#include "com/Functions.h"
#include "model/ExcelSaver.h"

class Saver : public QThread
{
public:
    Saver(const ClientParam &param, const QImage &img, const QSqlDatabase &db, const QSqlDatabase &db2,
          const QString &path, const QString &path2, const QString &imgName, const bool isOk, const QString &defect);

    void setParam(ExcelSaver *saver, const QString &wkshop, const QString &level);
    void setInfoParam(const QString &date, const QString &info, const QString &dute, const QString &res, const QString &line);
    void run();

private:
    void saveImage(const QString &imgName);
    void insertData(const QString &sql, const QString &sql2);
    void insertData2(QSqlDatabase &db,const QString &sql, const QString &sql2);
    void insertDataEx(const QString &sql, const QString sharePath, const QString &sql2);
    void insertPath(const QString barCode, const QString code, const QString sharePath);
    void insertPathEx(QSqlDatabase &db,const QString barCode, const QString code, const QString sharePath);
    void insertBinData();

private:
    ClientParam m_param;
    QImage m_image;
    QSqlDatabase m_db;
    QSqlDatabase m_db2;
    QString m_path;
    QString m_path2;
    QString m_imgName;
    bool m_isOk;
    QString m_defect;
    ExcelSaver *m_excelSaver;
    QString m_wkshop;
    QString m_level;
    QString m_sharePath;

    QString m_date;
    QString m_info;
    QString m_dute;
    QString m_res;
    QString m_line;
};

#endif // SAVER_H
