#include "Saver.h"
#include <QDateTime>
#include <QTime>
#include <QSqlQuery>
#include <QDir>
#include <QMutex>
#include <QBuffer>
#include <QSqlError>
#include <com/clsSettings.h>


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

void Saver::setInfoParam(const QString &date, const QString &info, const QString &dute, const QString &res, const QString &line)
{
    m_date = date;
    m_info = info;
    m_dute = dute;
    m_res = res;
    m_line = line;
}

void Saver::run()
{
//    QString info = m_param.info;
//    QString line = m_param.line;
    QDateTime curDate = QDateTime::currentDateTime();
//    QString strDate = curDate.toString("yyyy-MM-dd");
//    QTime curTime = QTime::currentTime();
//    QString dute = QStringLiteral("白班");
//    if(curTime.hour() < 8 || curTime.hour() > 19)
//    {
//        dute = QStringLiteral("晚班");
//    }
//    if(curTime.hour() < 8)
//    {
//        strDate = curDate.addDays(-1).toString("yyyy-MM-dd");
//    }
//    QString strRes = "OK";

//    if(!m_isOk)
//    {
//        strRes = "NG";
//    }

    QString path = m_path + "/" + m_date + "/" + m_info + "/" + m_dute + "/" + m_res + "/";
    QString path2 = m_path2 + m_date + "/" + m_info + "/" + m_dute + "/" + m_res + "/";
    QString csvPath = m_path + "/" + m_date + "/" + m_info + "/" + m_dute + "/";

    if(m_level != "" && (!m_isOk))
    {
        path = path + m_level + "/";
        path2 = path2 + m_level + "/";
    }

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
        wpPath = m_path + "/" + m_date + "/" + m_info + "/" + m_dute + "/" + strWP + "/";
        Functions::sleepMs(20);
        QDir wpdir(wpPath);
        if(!wpdir.exists())
        {
            Functions::mkDir(wpPath);
        }
        QString wpImgName = wpPath + m_imgName;
        saveImage(wpImgName);
    }

    clsSettings *settings = new clsSettings("./cfg/Settings.ini");
    QString ip,name,user,pwd;
    QString strNode = QString("Database/");
    settings->readSetting(strNode + "dbIp", ip);
    settings->readSetting(strNode + "dbName", name);
    settings->readSetting(strNode + "dbUser", user);
    settings->readSetting(strNode + "dbPwd", pwd);

//    QString shareIP,shareFolderName;
//    strNode = QString("SharePath/");
//    settings->readSetting(strNode + "shareIP", shareIP);
//    settings->readSetting(strNode + "shareFolderName", shareFolderName);
//    delete settings;
//    settings = NULL;

    QDateTime now = QDateTime::currentDateTime();
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL",now.toString("yyMMdd-HHmmss-zzz"));
    int port = 3306;
    db.setHostName(ip);
    db.setDatabaseName(name);
    db.setUserName(user);
    db.setPort(port);
    db.setPassword(pwd);
    if(!db.open())
    {
        qDebug() << "Saver::run() db open failed.";
        return;
    }
    //db.exec("set NAMES UTF8");

//    //将NG更新到repair表
//    if(!m_isOk)
//    {
//        if(shareIP.isEmpty())
//        {
//            qDebug() << "Saver::run === shareIP is empty.";
//            shareIP="optplug_ip";
//        }
//        if(shareFolderName.isEmpty())
//        {
//            qDebug() << "Saver::run === shareFolderName is empty.";
//            shareIP="optplug_sharename";
//        }

//        //QString sharePath = QString("//10.17.0.146") + "/" + QString("img") + "/"+ strDate + "/" + info + "/" + dute + "/" + strRes + "/" + m_imgName;
//        QString sharePath = QString("//") + shareIP + "/" + shareFolderName + "/"+ strDate + "/" + info + "/" + dute + "/" + strRes + "/" + m_imgName;
//        //QString repSql = QString("insert into tab_repairex (barcode,code,path,flag) values('%1','%2','%3',0)").arg(m_param.barCode).arg(m_imgName.left(m_imgName.size()-4)).arg(sharePath);
//        //QSqlQuery queryRep(db);
//        //bool bret = queryRep.exec(repSql);
//        //if(bret == false)
//        //{
//        //    qDebug() << "Saver::run === Insert respair table error. Info:"<<queryRep.lastError().text() << "sql:" << repSql;
//        //}
//        //else
//        //{
//        //    qDebug() << "Saver::run === Insert respair table ok. "<< "sql:" << repSql;
//        //}
//        //queryRep.clear();
//        insertPathEx(db, m_param.barCode, m_imgName.left(m_imgName.size()-4), sharePath);
//        //db.close();
//    }

    int ss = curDate.currentSecsSinceEpoch();

    QString strAiRes = "NG";
    QString code = m_imgName.left(m_imgName.size()-4);//m_imgName.remove(m_imgName.length() - 4, 4);
    QString sql = QString("insert into tab_result values ('%1','%2','%3','%4','%5','%6',%7,'%8','%9')").arg(code)
            .arg(m_wkshop).arg(m_line).arg(strAiRes).arg(m_res).arg(m_defect).arg(ss).arg(path).arg(m_level);
    QString sql2 = QString("insert into tab_result values ('%1','%2','%3','%4','%5','%6',%7,'%8','%9')").arg(code)
            .arg(m_wkshop).arg(m_line).arg(strAiRes).arg(m_res).arg(m_defect).arg(ss).arg(path2).arg(m_level);
    qDebug()<<"insert sql "<<sql;
    //insertData(sql, sql2);
    //insertDataEx(sql, sharePath, sql2);
    insertData2(db, sql, sql2);
    db.close();

    QStringList header;
    header<<QStringLiteral("条码")<<QStringLiteral("车间")<<QStringLiteral("产线")<<QStringLiteral("AI结果")
         <<QStringLiteral("复判结果")<<QStringLiteral("缺陷信息")<<QStringLiteral("测试时间")
        <<QStringLiteral("保存路径")<<QStringLiteral("分选等级");
    QStringList dataList;
    dataList<<code<<m_wkshop<<m_line<<strAiRes<<m_res<<m_defect<<curDate.toString("yyyy-MM-dd hh:mm:ss")
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
    if(!m_db.isOpen())
    {
        qDebug()<<"Saver::insertData: m_db not open";
        return;
    }

    QMutex mutex;
    mutex.lock();

    QSqlQuery query(m_db);
    query.exec(sql);

//    QByteArray ba;
//    QBuffer buffer(&ba);
//    m_image.save(&buffer, "JPG");
//    //QString code = m_param.barCode + QString("_") + m_imgName.remove(m_imgName.length() - 4, 4);
//    QString code = m_param.barCode + QString("_") + m_imgName;

//    QString repSql = QString("insert into tab_repair (code,image) values('%1',?)").arg(code);

//    query.prepare(repSql);
//    query.addBindValue(ba);
//    if(!query.exec())
//    {
//        qDebug()<<"Insert image query Error "<<query.lastError().text();
//    }
//    query.clear();

    if(m_db2.isOpen()==true)
    {
    	QSqlQuery query2(m_db2);
    	query2.exec(sql2);
    	query2.clear();
	}

    mutex.unlock();
}

void Saver::insertData2(QSqlDatabase &db, const QString &sql, const QString &sql2)
{
    if(!db.isOpen())
    {
        qDebug()<<"Saver::insertData2: db not open";
        return;
    }

    //QMutex mutex;
    //mutex.lock();

    if (db.transaction())
    {
        QSqlQuery query(db);
        query.prepare(sql);
        query.exec(sql);
        if (!db.commit())
        {
            qDebug() << "Saver::insertData2 === commit fail. err type:" << db.lastError().type() << " text:" << db.lastError().text();
            if (!db.rollback())
            {
                qDebug() << "Saver::insertData2 === commit fail. rollback fail. err type:" << db.lastError().type() << " text:" << db.lastError().text();
            }
            else
            {
                qDebug() << "Saver::insertData2 === commit fail. rollback ok.";
            }
        }
        else
        {
            qDebug() << "Saver::insertData2 === commit ok.";
            //query.clear();
        }
    }

    QMutex mutex;
    mutex.lock();

    if(m_db2.isOpen()==true)
    {
        QSqlQuery query2(m_db2);
        query2.exec(sql2);
        query2.clear();
    }

    mutex.unlock();
}

void Saver::insertDataEx(const QString &sql, const QString sharePath, const QString &sql2)
{
    QMutex mutex;
    mutex.lock();

    if(m_db.isOpen()==true)
    {
        QSqlQuery query(m_db);
        query.exec(sql);
        query.clear();

    //    QByteArray ba;
    //    QBuffer buffer(&ba);
    //    m_image.save(&buffer, "JPG");
        //QString code = m_param.barCode + QString("_") + m_imgName.remove(m_imgName.length() - 4, 4);
    //    QString code = m_param.barCode + QString("_") + m_imgName;

        QString code = m_imgName;

        QString repSql = QString("insert into tab_repairex (barcode,code,path,flag) values('%1','%2','%3',0)").arg(m_param.barCode).arg(code).arg(sharePath);

    //    query.prepare(repSql);
    //    query.addBindValue(ba);
    //    if(!query.exec())
        QSqlQuery queryRep(m_db);
        bool bret = queryRep.exec(repSql);
        if(bret == false)
        {
            qDebug()<<"Insert respair table error. "<<query.lastError().text() << "sql:" << repSql;
        }
        else
        {
            qDebug()<<"Insert respair table ok. "<< "sql:" << repSql;
        }
        queryRep.clear();
    }
    else
    {
        qDebug()<<"m_db not open.";
    }

    if(m_db2.isOpen()==true)
    {
        QSqlQuery query2(m_db2);
        query2.exec(sql2);
        query2.clear();
    }

    mutex.unlock();
}

void Saver::insertPath(const QString barCode, const QString code, const QString sharePath)
{
    if(!m_db.isOpen())
    {
        qDebug()<<"Saver::insertPath: m_db not open";
        return;
    }

//    QMutex mutex;
//    mutex.lock();

    //QString repSql = QString("insert into tab_repairex (barcode,code,path,flag) values('%1','%2','%3',0)").arg(barCode).arg(code).arg(sharePath);
    QString repSql = QString("insert into tab_repairex values('%1','%2','%3',0)").arg(barCode).arg(code).arg(sharePath);
    QSqlQuery queryRep(m_db);
    bool bret = queryRep.exec(repSql);
    if(bret == false)
    {
        qDebug() << "Saver::insertPath === Insert respair table error. Info:"<<queryRep.lastError().text() << "sql:" << repSql;
    }
    else
    {
        qDebug() << "Saver::insertPath === Insert respair table ok. "<< "sql:" << repSql;
    }
    queryRep.clear();

//    mutex.unlock();
}

void Saver::insertPathEx(QSqlDatabase &db, const QString barCode, const QString code, const QString sharePath)
{
    if(!db.isOpen())
    {
        qDebug()<<"Saver::insertPathEx: db not open";
        return;
    }

    if (db.transaction())
    {
        //QString repSql = QString("insert into tab_repairex (barcode,code,path,flag) values('%1','%2','%3',0)").arg(barCode).arg(code).arg(sharePath);
        QString repSql = QString("insert into tab_repairex values('%1','%2','%3',0)").arg(barCode).arg(code).arg(sharePath);
        qDebug() << "Saver::insertPathEx === sql:" << repSql;

        QSqlQuery queryRep(db);
        queryRep.prepare(repSql);
        queryRep.exec(repSql);
        if (!db.commit())
        {
            qDebug() << "Saver::insertPathEx === commit fail. err type:" << db.lastError().type() << " text:" << db.lastError().text();
            if (!db.rollback())
            {
                qDebug() << "Saver::insertPathEx === commit fail. rollback fail. err type:" << db.lastError().type() << " text:" << db.lastError().text();

                return;
            }
            else
            {
                qDebug() << "Saver::insertPathEx === commit fail. rollback ok.";

                return;
            }
        }
        else
        {
            qDebug() << "Saver::insertPathEx === commit ok.";
            //queryRep.clear();

            return;
        }
    }
    else
    {
        qDebug() << "Saver::insertPathEx === begin transaction fail. err type:" << db.lastError().type() << " text:" << db.lastError().text();
    }
}

void Saver::insertBinData()
{
    if(!m_db.isOpen())
    {
        qDebug()<<"Saver::insertBinData: m_db not open";
        return;
    }

    QByteArray ba;
    QBuffer buffer(&ba);
    m_image.save(&buffer, "JPG");
    //QString code = m_param.barCode + QString("_") + m_imgName.remove(m_imgName.length() - 4, 4);
    QString code = m_param.barCode + QString("_") + m_imgName;

    QString repSql = QString("insert into tab_repair (code,image) values('%1',?)").arg(code);

    QSqlQuery query(m_db);
    query.prepare(repSql);
    query.addBindValue(ba);
    if(!query.exec())
    {
        qDebug()<<"insertBinData: Insert respair table error. Info:"<<query.lastError().text() << "sql:" << repSql;
    }
    else
    {
        qDebug()<<"insertBinData: Insert respair table ok. "<< "sql:" << repSql;
    }
    query.clear();
}
