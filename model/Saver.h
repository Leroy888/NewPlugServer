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
    void run();

private:
    void saveImage(const QString &imgName);
    void insertData(const QString &sql, const QString &sql2);

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
};

#endif // SAVER_H
