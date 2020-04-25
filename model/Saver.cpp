#include "Saver.h"
#include <QDateTime>
#include <QTime>
#include <QSqlQuery>
#include <QDir>
#include <QMutex>


#include <QDebug>

Saver::Saver(const ClientParam &param, const QImage &img, const QSqlDatabase &db, const QSqlDatabase &db2,
             const QString &path, const QString &path2, const QString &imgName, const bool isOk, const QString &defect)
    :m_param(param), m_image(img), m_db(db), m_db2(db2), m_path(path), m_path2(path2),
      m_imgName(imgName), m_isOk(isOk), m_defect(defect)
{

}

void Saver::setParam(ExcelSaver *saver, const QString &wkshop, const QString &level)
{
    m_excelSaver = saver;
    m_wkshop = wkshop;
    m_level = level;
}

void Saver::run()
{
    QString info = m_param.info;
    QString line = m_param.line;
    QDateTime curDate = QDateTime::currentDateTime();
    QString strDate = curDate.toString("yyyy-MM-dd");
    QTime curTime = QTime::currentTime();
    QString dute = QStringLiteral("白班");
    if(curTime.hour() < 8 || curTime.hour() > 19)
    {
        dute = QStringLiteral("晚班");
    }
    if(curTime.hour() < 8)
    {
        strDate = curDate.addDays(-1).toString("yyyy-MM-dd");
    }
    QString strRes = "OK";

    if(!m_isOk)
    {
        strRes = "NG";
    }

    QString path = m_path + "/" + strDate + "/" + info + "/" + dute + "/" + strRes + "/";
    QString path2 = m_path2 + strDate + "/" + info + "/" + dute + "/" + strRes + "/";
    QString csvPath = m_path + "/" + strDate + "/" + info + "/" + dute + "/";

    QDir dir(path);
    if(!dir.exists())
    {
        Functions::mkDir(path);
    }

    QString imgName = path + m_imgName;
    saveImage(imgName);

    QString wpPath;
    if(m_isOk)
    {
        QString strWP = QStringLiteral("误判");
        wpPath = m_path + "/" + strDate + "/" + info + "/" + dute + "/" + strWP + "/";
        Functions::sleepMs(20);
        QDir wpdir(wpPath);
        if(!wpdir.exists())
        {
            Functions::mkDir(wpPath);
        }
        QString wpImgName = wpPath + m_imgName;
        saveImage(wpImgName);
    }

    int ss = curDate.currentSecsSinceEpoch();

    QString strAiRes = "NG";
    QString code = m_imgName.remove(m_imgName.length() - 4, 4);
    QString sql = QString("insert into tab_result values ('%1','%2','%3','%4','%5','%6',%7,'%8','%9')").arg(code)
            .arg(m_wkshop).arg(line).arg(strAiRes).arg(strRes).arg(m_defect).arg(ss).arg(path).arg(m_level);
    QString sql2 = QString("insert into tab_result values ('%1','%2','%3','%4','%5','%6',%7,'%8','%9')").arg(code)
            .arg(m_wkshop).arg(line).arg(strAiRes).arg(strRes).arg(m_defect).arg(ss).arg(path2).arg(m_level);
qDebug()<<"insert sql "<<sql;
    insertData(sql, sql2);

    QStringList header;
    header<<QStringLiteral("条码")<<QStringLiteral("车间")<<QStringLiteral("产线")<<QStringLiteral("AI结果")
         <<QStringLiteral("复判结果")<<QStringLiteral("缺陷信息")<<QStringLiteral("测试时间")
        <<QStringLiteral("保存路径")<<QStringLiteral("分选等级");
    QStringList dataList;
    dataList<<code<<m_wkshop<<line<<strAiRes<<strRes<<m_defect<<curDate.toString("yyyy-MM-dd hh:mm:ss")
           <<path<<m_level;

    qDebug()<<"dateTime "<<curDate.toString("yyyy-MM-dd hh:mm:ss");
    m_excelSaver->setParam(header, dataList, Appened, csvPath + QStringLiteral("/统计表.csv"));
    m_excelSaver->save();
}

void Saver::saveImage(const QString &imgName)
{
    m_image.save(imgName);
}

void Saver::insertData(const QString &sql, const QString &sql2)
{
    QMutex mutex;
    mutex.lock();

    QSqlQuery query(m_db);
    query.exec(sql);
    query.clear();

    QSqlQuery query2(m_db2);
    query2.exec(sql2);
    query2.clear();

    mutex.unlock();
}
