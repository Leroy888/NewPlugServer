#include "SortThread.h"
#include "com/Functions.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "model/ExcelSaver.h"
#include <QMutex>

#include <QDebug>


bool operator <=( AiPoint &pt1,  AiPoint &pt2)
{
    if((pt1.x1 >= pt2.x1) && (pt1.y1 >= pt2.y1) && (pt1.x2 <= pt2.x2) && (pt1.y2 < pt2.y2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

SortThread::SortThread(const SortMap &sortMap, int handle, QString &imgPath, QStringList sortList, QStringList defList, AiDataMap &areaMap, AiDataMap &lenMap, QMap<QString, QStringList> posMap)
    :m_sortMap(sortMap),m_handle(handle),m_imgPath(imgPath), m_sortList(sortList), m_defList(defList), m_areaMap(areaMap),
      m_lenMap(lenMap),m_posMap(posMap)
{
    m_isBreak = false;

    m_dgLevels<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T";
}

SortThread::SortThread()
{
    m_isBreak = false;
}

void SortThread::setDefects(QStringList sortList, QStringList defList, AiDataMap &areaMap, AiDataMap &lenMap, QMap<QString, QStringList> posMap)
{
    m_sortList = sortList;
    m_defList = defList;
    m_areaMap = areaMap;
    m_lenMap = lenMap;
    m_posMap = posMap;
}

void SortThread::setSortMap(const SortMap &sortMap)
{
    m_sortMap = sortMap;
}

void SortThread::setDB(const QSqlDatabase &db)
{
    m_db = db;
}

void SortThread::setCode(const QString &code)
{
    m_code = code;
}

void SortThread::setAiReslut(const bool isOk)
{
    m_aiOk = isOk;
}

void SortThread::setParams(const QSqlDatabase &db, const QSqlDatabase &db2,const QString &wkshop,
                           const ClientParam &param,const QString savePath, const QImage &img,
                           const QString &code, const bool isOk, const QStringList &dgList)
{
    m_db = db;
    m_db2 = db2;
    m_param = param;
    m_wkshop = wkshop;
    m_savePath = savePath;
    m_image = img;
    m_code = code;
    m_aiOk = isOk;
    m_dgList = dgList;
}

void SortThread::setParams(const SortMap &sortMap, const QSqlDatabase &db, const QSqlDatabase &db2, const ClientParam &param,
                           const QString savePath, const QString &code, const QStringList &dgList)
{
    m_sortMap = sortMap;
    m_db = db;
    m_db2 = db2;
    m_param = param;
    m_savePath = savePath;
    m_code = code;
    m_dgList = dgList;
}

void SortThread::setDateTime(const int ms)
{
    m_timeMs = ms;
}

void SortThread::setPosList(const QStringList &list)
{
    m_posList = list;
}

void SortThread::setClsList(const QStringList &list)
{
    m_clsList = list;
}

void SortThread::setClsMap(const QMap<QString, QString> clsMap)
{
    m_clsMap = clsMap;
}

void SortThread::setMaps(const AiDataMap &dgMap, const AiPointMap &ptMap)
{
    m_dgMap = dgMap;
    m_pointMap = ptMap;
}

void SortThread::run()
{
    m_isOk = sorting();
    emit sig_sort_result(m_handle, m_isOk, m_level);
    qDebug()<<"sortThread level "<<m_level;

    QString level;
    if(m_isOk)
    {
        level = QStringLiteral(" 等级：") + m_level;
    }
    emit sig_sort_info(m_isOk, m_sortInfo, level);
    if(m_isOk)
    {
        m_defect = "";
        saveImage();
    }
  //  saveSqlData();
}

void SortThread::saveSqlData()
{
    QMutex mutex;
    mutex.lock();
    QSqlQuery query(m_db);

    for(int i=0; i<m_defList.length(); i++)
    {

        QString sql = QString("insert into tab_defect values ('%1',%2,'%3','%4')").arg(m_code).arg(QString::number(m_timeMs))
                .arg(m_defList.at(i)).arg(m_posList.at(i));
        query.exec(sql);
    }
    query.clear();
    mutex.unlock();
}

bool SortThread::sorting()
{
    bool isOk ;
    QString imgName = m_imgPath.split("/").last();
    QString strDev = m_param.info + " : " + imgName + " ";

    QStringList posKeys = m_posMap.keys();
    QList<QStringList> posValues = m_posMap.values();

    QStringList dgKeys = m_dgMap.keys();

    qDebug()<<"-------------------------------- dgMap -------------------------------------";
    for(int i=0; i<m_dgMap.count(); i++)
    {
        qDebug()<<dgKeys.at(i)<<" : "<<m_dgMap.value(dgKeys.at(i));
    }
    qDebug()<<"-------------------------------- dgMap -------------------------------------";
    qDebug()<<"-------------------------------- posMap -------------------------------------";
    for(int i = 0; i< m_posMap.count(); i++)
    {
        qDebug()<<posKeys.at(i)<<" : "<<posValues.at(i);
    }
    qDebug()<<"--------------------------------- posMap -------------------------------------";

    int levelIndex = 0;

    for(int i=0; i< m_sortList.length(); i++)
    {
        isOk = true;
        m_sortInfo = strDev + QStringLiteral(" : 分选结果：OK");
        m_level = m_sortList.at(i);
        QMap<QString,QStringList> defMap = m_sortMap.value(m_level);
        QMap<QString,QStringList>::iterator defIt;

        AiDataMap::iterator areaIt = m_areaMap.begin();
        AiDataMap::iterator lenIt = m_lenMap.begin();
        AiDataMap::iterator dgIt = m_dgMap.begin();
        AiPointMap::iterator ptIt = m_pointMap.begin();

        for(areaIt; areaIt != m_areaMap.end(); areaIt++)    //遍历所有缺陷
        {
            QString strDef = lenIt.key();
            m_defect = strDef;
            qDebug()<<m_handle<<strDef;
            QList<double> areaList = areaIt.value();
            QList<double> lenList = lenIt.value();
            QList<double> dgList = dgIt.value();
            QList<AiPoint> ptList = ptIt.value();

            QStringList defStdList = defMap.value(strDef);
            qDebug()<<m_handle<<strDef<<" stdList "<<defStdList;

            if(defStdList.length() == 0)
            {
                m_sortInfo = strDef + QStringLiteral("没有分选标准");
                isOk = false;
                break;
            }

            //            QString strMinDg = defStdList.at(2);
            //            QString strMaxDg = defStdList.at(3);
            QString strDg = defStdList.at(2);
            int index = m_dgLevels.indexOf(strDg);
            int dgValue = QString(m_dgList.at(index)).toInt();
            QString strMinArea = defStdList.at(3);
            QString strMaxArea = defStdList.at(4);
            QString strMinLen = defStdList.at(5);
            QString strMaxLen = defStdList.at(6);
            QString strPoint1 = defStdList.at(7);   //坐标1
            QString strPoint2 = defStdList.at(8);   //坐标2
            QString strPiecNum = defStdList.at(9);
            QString strNum = defStdList.at(10);
            QString strAffNum = defStdList.at(11);

            double minArea = strMinArea.toDouble();
            double maxArea = strMaxArea.toDouble();
            double minLen = strMinLen.toDouble();
            double maxLen = strMaxLen.toDouble();

            AiPoint point;  //坐标设置的标准
            QStringList list = strPoint1.split(",");
            point.x1 = QString(list.at(0)).toInt();
            point.y1 = QString(list.at(1)).toInt();
            list = strPoint2.split(",");
            point.x2 = QString(list.at(0)).toInt();
            point.y2 = QString(list.at(1)).toInt();

            int stdPiecNum = strPiecNum.toInt();
            int stdNum = strNum.toInt();
            int stdAffNum = strAffNum.toInt();

            if(defStdList.isEmpty())
            {
                break;
            }
            //   qDebug()<<m_handle<<strDef<<defStdList;
            if(defStdList.at(0) == QStringLiteral("否"))
            {
                QString tmpStr = QStringLiteral(" : 不允许 ");
                m_sortInfo = strDev + strDef + tmpStr + m_level;
                isOk = false;
                break;
            }
            else
            {
                int piecNum = 0;    //某一缺陷在电池片上的最大个数
                int num = 0;        //组件上有多少电池片有某一缺陷
                int affNum = 0;

                QMap<QString,int> tmpPosMap;
                QStringList posList = m_posMap.value(strDef);

                for(int i=0; i<lenList.length(); i++)
                {
                    AiPoint resPoint = ptList.at(i);

                    if(/*(resPoint <= point) &&*/ (dgList.at(i) < dgValue) && (areaList.at(i) > minArea && areaList.at(i) < maxArea) && (lenList.at(i) >= minLen && lenList.at(i) <= maxLen))
                    {
                        piecNum = tmpPosMap.value(posList.at(i)) + 1;
                        tmpPosMap.insert(posList.at(i), piecNum);
                    }
                    else if(dgList.at(i) >= dgValue)
                    {
                        QString tmpDg = QString::number(dgList.at(i));
                        QString tmpStr = QStringLiteral(":对比度 ");
                        m_sortInfo = strDev + strDef + tmpStr + tmpDg + ">" + QString::number(dgValue) + " " + m_level;
                        isOk = false;
                        break;
                    }
                    else if(!(resPoint <= point))
                    {
                        QString info = QString("point1:(%1,%2;%3,%4)").arg(QString::number(resPoint.x1)).arg(QString::number(resPoint.y1))
                                .arg(QString::number(resPoint.x2)).arg(QString::number(resPoint.y2))
                                + QString("lager point2:(%1,%2;%3,%4)").arg(QString::number(point.x1)).arg(QString::number(point.y1))
                                .arg(QString::number(point.x2)).arg(QString::number(point.y2));
                        m_sortInfo = strDev + strDef + info + QStringLiteral(":位置超出范围") + " " + m_level;
                        isOk = false;
                        break;
                    }
                    else if(areaList.at(i) > maxArea)
                    {
                        QString tmpArea = QString::number(areaList.at(i));
                        QString tmpStr = QStringLiteral(":面积 ");
                        m_sortInfo = strDev + strDef + tmpStr + tmpArea + "% > " + QString::number(maxArea) + "%" + " " + m_level;
                        isOk = false;
                        break;
                    }
                    else if(lenList.at(i) > maxLen)
                    {
                        QString tmpLen = QString::number(lenList.at(i));
                        QString tmpStr = QStringLiteral(":长度 ");
                        m_sortInfo = strDev + strDef + tmpStr + tmpLen + "% > " + QString::number(maxLen) + "%" + " " + m_level;
                        isOk = false;
                        break;
                    }
                }

                if(!isOk)
                {
                    continue;
                }

                QList<int> valList = tmpPosMap.values();
                qSort(valList.begin(), valList.end());
                piecNum = valList.last();
                num = tmpPosMap.keys().length();

                if(piecNum > stdPiecNum)
                {
                    QString tmpPiecNum = QString::number(piecNum);
                    QString tmpStr = QStringLiteral(":单片缺陷个数 ");
                    m_sortInfo = strDev + strDef + tmpStr + tmpPiecNum + ">" + QString::number(stdPiecNum) + " " + m_level;
                    isOk = false;
                    break;
                }
                if(num > stdNum)
                {
                    QString tmpNum = QString::number(num);
                    QString tmpStr = QStringLiteral(":组件缺陷个数 ");
                    m_sortInfo = strDev + strDef + tmpStr + tmpNum + ">" + QString::number(stdNum) + " " + m_level;
                    isOk = false;
                    break;
                }
            }

            if(!isOk)
            {
                break;
            }
            lenIt++;
            ptIt++;
            dgIt++;
        }
        if(isOk)
        {
            m_level = m_sortList.at(levelIndex);
            break;
        }
        levelIndex++;
    }
    return isOk;
}

void SortThread::saveImage()
{
    QDateTime curTime = QDateTime::currentDateTime();
    QString strDate = curTime.toString("yyyy-MM-dd");
    int hour = QTime::currentTime().hour();
    QString dute = QStringLiteral("/白班");
    if(hour < 8 || hour > 19)
    {
        dute = QStringLiteral("/晚班");
    }
    if(hour < 8)
    {
        strDate = curTime.addDays(-1).toString("yyyy-MM-dd");
    }
    QString strRes = "OK";

    QString imgName = m_imgPath.split("/").last();
    QString info = m_param.info;
    QString line = m_param.line;
    QString path = m_savePath + "/" + strDate + "/" +info + dute + "/" + strRes  + "/";
    QString path2 = m_savePath + "/" + strDate + "/" +info + dute + "/" + strRes  + "/";
    QString csvPath = m_savePath + "/" + strDate + "/" + info + dute + "/";
    m_csvPath = path;
    Functions::mkDir(path);
    m_image.save(path + imgName);

    QString code = imgName.remove(imgName.length() - 4, 4);
    int ss = curTime.currentSecsSinceEpoch();

    QString strAiRes = "OK";
    if(!m_aiOk)
    {
        strAiRes = "NG";
    }

    QString sql = QString("insert into tab_result values ('%1','%2','%3','%4','%5','%6',%7,'%8','%9')")
            .arg(code).arg(m_wkshop).arg(line).arg(strAiRes).arg(strRes).arg(m_defect).arg(ss).arg(path).arg(m_level);
    QString sql2 = QString("insert into tab_result values ('%1','%2','%3','%4','%5','%6',%7,'%8','%9')")
            .arg(code).arg(m_wkshop).arg(line).arg(strAiRes).arg(strRes).arg(m_defect).arg(ss).arg(path2).arg(m_level);

    insertResult(sql, sql2);

    QStringList header;
    header<<QStringLiteral("条码")<<QStringLiteral("车间")<<QStringLiteral("产线")<<QStringLiteral("AI结果")
         <<QStringLiteral("复判结果")<<QStringLiteral("缺陷信息")<<QStringLiteral("测试时间")
        <<QStringLiteral("保存路径")<<QStringLiteral("分选等级");
    QStringList dataList;
    dataList<<code<<m_wkshop<<line<<strAiRes<<strRes<<m_defect<<curTime.toString("yyyy-MM-dd hh:mm:ss")
           <<path<<m_level;

    saveExcel(header, dataList, csvPath);
}

void SortThread::insertResult(const QString &sql, const QString &sql2)
{
    QSqlQuery query(m_db);
    query.exec(sql);

    QSqlQuery query2(m_db2);
    query2.exec(sql2);

    query.clear();
    query2.clear();
}

void SortThread::saveExcel(const QStringList &header, const QStringList &dataList, const QString &path)
{
    ExcelSaver * excelSaver = new ExcelSaver();
    excelSaver->setParam(header, dataList, Appened, path + QStringLiteral("/统计表.csv"));
    excelSaver->save();


    return;

    QString fileName = m_csvPath + QStringLiteral("/统计OK.csv");
    IOStyle style = ReadWrite;
    if(!m_isOk)
    {
        fileName = m_csvPath + QStringLiteral("/统计NG.csv");
        style = Appened;
    }

    QStringList headerList;
    headerList<<QStringLiteral("条码")<<QStringLiteral("缺陷")<<QStringLiteral("位置")/*<<QStringLiteral("面积")<<QStringLiteral("长度")*/;
    QString strDef;
    for(int i=0; i< m_defList.length(); i++)
    {
        if(i != m_defList.length() - 1)
        {
            strDef = strDef + m_defList.at(i) + ",";
        }
        else
        {
            strDef = strDef + m_defList.at(i);
        }
    }

    QString strPos;
    for(int i=0; i< m_posList.length(); i++)
    {
        if(i != m_posList.length() - 1)
        {
            strPos = strPos + m_posList.at(i) + ",";
        }
        else
        {
            strPos = strPos + m_posList.at(i);
        }
    }

    QStringList data;
    data<<m_code<<strDef<<strPos;

    ExcelSaver *saver = new ExcelSaver(headerList, data, style, fileName);
    saver->save();
}

