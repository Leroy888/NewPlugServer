#include "AiThread.h"
#include "com/clsSettings.h"
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>
#include <QEventLoop>
#include <QTimer>
#include <QJsonArray>
#include <QList>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QApplication>
#include <qmath.h>
#include <QImageReader>

#include <QDebug>


AiThread::AiThread(QString &imgPath, QString strUrl, int horNum, int verNum, int handle, int index)
    :m_imgPath(imgPath),m_strUrl(strUrl),m_horNum(horNum),m_verNum(verNum),m_handle(handle), m_index(index)
{
    readAiModel("");
    m_waitTimeMs = 5000;
    m_replay = NULL;
    m_timer = NULL;
    m_isStop = false;
}

AiThread::~AiThread()
{

    //qDebug()<<"Delete AiThread";
}

void AiThread::setImageParam(const int horPic, const int verPic, const int topPix, const int btmPix, const int leftPix,
                             const int rightPix, const int imgModel)
{
    m_horNum = horPic;
    m_verNum = verPic;
    m_leftPixel = leftPix;
    m_bottomPixel = btmPix;
    m_topPixel = topPix;
    m_rightPixel = rightPix;
    m_imgModel = imgModel;
}

void AiThread::run()
{
    qDebug()<<"AiThread handle = "<<m_handle;
    Functions::sleepMs(100);

    QImageReader *reader = new QImageReader();
    reader->setFileName(m_imgPath);
    if(reader->canRead())
    {
        m_img = reader->read();
        for(int i=0; i<20; i++)
        {
            if(m_img.isNull())
            {
                Functions::sleepMs(20);
                m_img = reader->read();
            }
            else
            {
                break;
            }
        }
    }
    delete reader;

    if(m_img.isNull())
    {
        QString info = m_param.info + QStringLiteral(" 无法读取图片，检查客户端共享路径");
        sig_sort_info(false, info, "", m_handle, m_strUrl);
        qDebug()<<"image is null";
        return;
    }

    if(m_imgModel)
    {
        int wid = m_img.width() / 2;
        int height = m_img.height() / 2;
        m_img = m_img.scaled(wid, height);
    }
    else
    {
        if(m_horNum == 20)
        {
            m_img = m_img.scaled(5750, 3550);
        }
        else if(m_horNum == 24)
        {
            m_img = m_img.scaled(6900, 3550);
        }
        else if(m_horNum == 26)
        {
            m_img = m_img.scaled(7475, 3550);
        }
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QByteArray ba;
    QBuffer buf(&ba);
    m_img.save(&buf, "JPG");
    QByteArray hexBa = ba.toBase64();

    QJsonObject jObj;
    jObj.insert("img", hexBa.toStdString().c_str());
    QJsonDocument jDoc;
    jDoc.setObject(jObj);

    QByteArray simpBa = jDoc.toJson(QJsonDocument::Compact);
    qDebug()<<m_handle<<" send image url : "<<m_strUrl;
    QNetworkRequest request;
    request.setUrl(QUrl(m_strUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QNetworkReply *reply = manager->post(request, simpBa);

    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    timer.start(15000);   // 15 secs. timeout
    qDebug()<<"post image data ";
    loop.exec();

    if(timer.isActive() && (reply->error() == QNetworkReply::NoError))
    {
        timer.stop();
        //        QString strRes = "{\"result\": [{\"y2\": 1088, \"class_id\": 1, \"dg\": 37, \"y1\": 695, \"score\": 0.9997709393501282, \"x2\": 2707, \"x1\": 2500}, {\"y2\": 1694, \"class_id\": 1, \"dg\": 31, \"y1\": 1335, \"score\": 0.9998655319213867, \"x2\": 1745, \"x1\": 1473}, {\"y2\": 1586, \"class_id\": 39, \"dg\": 35, \"y1\": 1394, \"score\": 0.9918626546859741, \"x2\": 2313, \"x1\": 2294}, {\"y2\": 1662, \"class_id\": 1, \"dg\": 60, \"y1\": 1294, \"score\": 0.44863200187683105, \"x2\": 2523, \"x1\": 2428}]}";
        //        QByteArray resBa = strRes.toLatin1();
        QByteArray resBa = reply->readAll();
        qDebug()<<m_handle<<"AI result: "<<QString(resBa);

        QJsonDocument resDoc = QJsonDocument::fromJson(QString::fromUtf8(resBa).toLatin1());
        QJsonObject resObj = resDoc.object();
        QJsonValue value = resObj.value(QString("result"));

        QList<AiResult> resList;
        m_isOk = true;

        if(value.isArray())
        {
            QJsonArray array = value.toArray();

            for(int i = 0; i< array.size(); i++)
            {
                QJsonObject obj = array.at(i).toObject();
                AiResult aiRes;
                aiRes.clsId = obj.value("class_id").toInt();
//                if(m_imgModel)
//                {
//                    aiRes.x1 = obj.value("x1").toInt() * 2;
//                    aiRes.y1 = obj.value("y1").toInt() * 2;
//                    aiRes.x2 = obj.value("x2").toInt() * 2;
//                    aiRes.y2 = obj.value("y2").toInt() * 2;
//                }
//                else
//                {
//                    aiRes.x1 = obj.value("x1").toInt();
//                    aiRes.y1 = obj.value("y1").toInt();
//                    aiRes.x2 = obj.value("x2").toInt();
//                    aiRes.y2 = obj.value("y2").toInt();
//                }
                aiRes.x1 = obj.value("x1").toInt();
                aiRes.y1 = obj.value("y1").toInt();
                aiRes.x2 = obj.value("x2").toInt();
                aiRes.y2 = obj.value("y2").toInt();
                aiRes.dg = obj.value("dg").toInt();

                int width = aiRes.x2 - aiRes.x1;
                int len = aiRes.y2 - aiRes.y1;

                aiRes.len = sqrtf(width*width + len*len);
                resList.append(aiRes);
            }
            if(array.size() > 0)
            {
                m_isOk = false;
            }
        }
        drawImage(resList);

        sort();
    }
    else
    {
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug()<<"error = "<<reply->errorString();
        }
        disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

        reply->abort();
        reply->close();
        reply->deleteLater();

        QString info = m_strUrl + QStringLiteral(" : AI服务器请求超时");
        qDebug()<<m_handle<<info;

        emit sig_aiDisConnect(m_strUrl, m_handle);
        emit sig_aiResult(false, true, m_strUrl, "", m_imgPath, m_handle, m_img, m_defectList, m_posList, m_clsList, m_areaMap, m_lenMap, m_posMap);

     //   QString pcInfo = m_param.info;
      //  Functions::saveImage(m_img, m_savePath, imgName, pcInfo, QStringLiteral("超时/"));
    }
    reply->close();
    reply->deleteLater();
    delete manager;
}

void AiThread::drawImage(QList<AiResult> resList)
{
    QStringList verLabels;
    verLabels<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K";

    int width = m_img.width();
    int height = m_img.height();
    int widPiec = (width - m_leftPixel - m_rightPixel) / m_horNum;
    int heigPiec = (height - m_topPixel - m_bottomPixel) / m_verNum;

    QMap<QString,int> posCountMap;
    //  QMap<QString,QMap<QString,int> tmpMap;  //QString 1:缺陷, QString 2:位置（A1、A2）, int:某一缺陷在某一位置的个数

    m_img = m_img.convertToFormat(QImage::Format_RGB888);
    QPainter painter(&m_img);
    qDebug()<<m_handle<<"start draw image";
    for(int i=0; i<resList.length(); i++)
    {
        AiResult aiRes = resList.at(i);
        QColor color(255, 0, 0);
        QPen pen(color, 6);
        painter.setPen(pen);
        QFont font;
        font.setPointSize(24);
        painter.setFont(font);
        QRect rect(aiRes.x1-50, aiRes.y1-50, aiRes.x2-aiRes.x1+100, aiRes.y2-aiRes.y1+100);
        painter.drawRect(rect);

        int x = (aiRes.x1 - 10 + aiRes.x2 + 20) / 2;
        int y = (aiRes.y1 - 10 + aiRes.y2 + 20) / 2;

        if(m_horNum == 0 || m_verNum == 0)
        {
            continue;
        }
        int wid = m_img.width()-20;
        int xMin = wid / m_horNum;
        int yMin = (m_img.height() - 20) / m_verNum;

        int a = x / xMin;
        int b = y / yMin;
        if(b >= 1)
        {
            a = a + m_horNum;
        }
        int index_w = ((aiRes.x2 + aiRes.x1 - m_leftPixel - m_rightPixel) / 2) / widPiec + 1;
        int index_h = ((aiRes.y2 + aiRes.y1 - m_topPixel - m_bottomPixel) / 2) / heigPiec;
        QString strPos = verLabels.at(index_h) + QString::number(index_w);  //缺陷所在哪一电池片：A1、A2、B3等
        AiPoint point = getAiPoint(aiRes, widPiec, heigPiec);


        double dbDg = aiRes.dg; //对比度
        double area = (aiRes.x2 - aiRes.x1) * (aiRes.y2 - aiRes.y1) * 1.0 / (widPiec * heigPiec) * 100;
        double dbVal = (aiRes.x2 - aiRes.x1)*(aiRes.x2 - aiRes.x1) + (aiRes.y2 - aiRes.y1)*(aiRes.y2 - aiRes.y1);

        double len = sqrt(dbVal) / widPiec * 100;

        QString strDef = m_Maps[aiRes.clsId];
        QString strCls = m_clsMap[strDef];  //获取改缺陷对应的大类  ： 隐裂、虚焊、其他等

        QStringList posList = m_posMap.value(strCls);
        posList.append(strPos);
        m_posMap.insert(strCls, posList);

        QList<double> dgList = m_dgMap.value(strCls);
        dgList.append(dbDg);
        m_dgMap.insert(strCls, dgList);

        QList<double> areaList = m_areaMap.value(strCls);
        areaList.append(area);
        m_areaMap.insert(strCls, areaList);

        QList<double> lenList = m_lenMap.value(strCls);
        lenList.append(len);
        m_lenMap.insert(strCls, lenList);

        QList<AiPoint> pointList = m_pointMap.value(strCls);
        pointList.append(point);
        m_pointMap.insert(strCls, pointList);

        m_posList.append(strPos);

        m_clsList.append(strCls);
        m_defectList.append(m_Maps[aiRes.clsId]);

        if(m_clsList.count(strCls) == 0)
        {
            int count = 1;
            m_countMap.insert(strCls, count);
        }
        else
        {
            int count = m_countMap.value(strCls) + 1;
            m_countMap.insert(strCls, count);
            int num = posCountMap.value(strPos);
        }
    }
    painter.end();
    qDebug()<<m_handle<<"draw image end";
    QMap<QString,int> ttMap = m_countPieceMap.value(QStringLiteral("隐裂"));
    QList<int> list = ttMap.values();

    qSort(list.begin(), list.end());
}

AiPoint AiThread::getAiPoint(const AiResult& aiRes, int widPic, int heightPic)
{
    AiPoint point;
    point.x1 = (aiRes.x1 - m_leftPixel) % widPic;
    point.x2 = (aiRes.x2 - m_leftPixel) % widPic;
    point.y1 = (aiRes.y1 - m_topPixel) % heightPic;
    point.y2 = (aiRes.y1 - m_topPixel) % heightPic;

    return point;
}

void AiThread::sort()
{
    SortThread sortObj(m_sortMap, m_handle, m_imgPath, m_sortList, m_defectList, m_areaMap, m_lenMap, m_posMap);
    QImage img = m_img.copy();
    sortObj.setParams(m_db, m_db2, m_wkshop, m_param, m_savePath, img, m_code, m_isOk, m_dgList);
    sortObj.setPosList(m_posList);
    sortObj.setClsMap(m_clsMap);
    sortObj.setMaps(m_dgMap, m_pointMap);
    connect(&sortObj,SIGNAL(sig_sort_result(int,bool,QString)),this,SLOT(slot_sortResult(int,bool,QString)));
    connect(&sortObj,SIGNAL(sig_sort_info(bool,QString,QString)),this,SLOT(slot_sort_info(bool,QString,QString)));

    sortObj.run();
}

bool AiThread::sorting()
{
    bool isOk ;
    m_sortInfo = QStringLiteral("分选结果： OK");
    QStringList posKeys = m_posMap.keys();
    QList<QStringList> posValues = m_posMap.values();
    //qDebug()<<"-------------------------------- posMap -------------------------------------";
    for(int i = 0; i< m_posMap.count(); i++)
    {
        //qDebug()<<posKeys.at(i)<<" : "<<posValues.at(i);
    }
    //qDebug()<<"--------------------------------- posMap -------------------------------------";

    SortMap::iterator it;
    QStringList leveList = m_sortMap.keys();
    int levelIndex = 0;
    for(it=m_sortMap.begin(); it != m_sortMap.end(); it++)
    {
        isOk = true;
        QString grade = it.key();
        QMap<QString,QStringList> defMap = it.value();
        QMap<QString,QStringList>::iterator defIt;

        AiDataMap::iterator areaIt = m_areaMap.begin();
        AiDataMap::iterator lenIt = m_lenMap.begin();

        for(areaIt; areaIt != m_areaMap.end(); areaIt++)    //遍历所有缺陷
        {
            QString strDef = lenIt.key();
            QList<double> areaList = areaIt.value();
            QList<double> lenList = lenIt.value();

            QStringList defStdList = defMap.value(strDef);

            if(defStdList.length() == 0)
            {
                continue;
            }

            QString strMinGray = defStdList.at(2);
            QString strMaxGray = defStdList.at(3);
            QString strMinArea = defStdList.at(4);
            QString strMaxArea = defStdList.at(5);
            QString strMinLen = defStdList.at(6);
            QString strMaxLen = defStdList.at(7);

            QString strPiecNum = defStdList.at(10);
            QString strNum = defStdList.at(11);
            QString strAffNum = defStdList.at(12);

            double minGray = strMinGray.toDouble();
            double maxGray = strMaxGray.toDouble();
            double minArea = strMinArea.toDouble();
            double maxArea = strMaxArea.toDouble();
            double minLen = strMinLen.toDouble();
            double maxLen = strMaxLen.toDouble();
            int stdPiecNum = strPiecNum.toInt();
            int stdNum = strNum.toInt();
            int stdAffNum = strAffNum.toInt();
            //qDebug()<<"MinArea = "<<minArea <<" ;MaxArea = "<<maxArea;
            //qDebug()<<"MinLen = "<<minLen<<" ;MaxLen = "<<maxLen;
            if(defStdList.isEmpty())
            {
                m_level = grade;
                break;
            }
            if(defStdList.at(0) == "false")
            {
                QString tmpStr = QStringLiteral(": 不允许");
                m_sortInfo = strDef + tmpStr;
                isOk = false;
                continue;
            }
            else if(defStdList.at(0) == QStringLiteral("不允许"))
            {
                QString tmpStr = QStringLiteral(": 不允许串珊");
                m_sortInfo = strDef + tmpStr;
                isOk = false;
                continue;
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
                    if((areaList.at(i) > minArea && areaList.at(i) < maxArea) && (lenList.at(i) >= minLen && lenList.at(i) <= maxLen))
                    {
                        piecNum = tmpPosMap.value(posList.at(i)) + 1;
                        tmpPosMap.insert(posList.at(i), piecNum);
                    }
                    else if(areaList.at(i) > maxArea)
                    {
                        //qDebug()<<"Area is lager max area    =======  "<<QString::number(areaList.at(i));
                        QString tmpArea = QString::number(areaList.at(i));
                        QString tmpStr = QStringLiteral(":面积为");
                        m_sortInfo = strDef + tmpStr + tmpArea + ">" + QString::number(maxArea);
                        isOk = false;
                        break;
                    }
                    else if(lenList.at(i) > maxLen)
                    {
                        QString tmpLen = QString::number(lenList.at(i));
                        QString tmpStr = QStringLiteral(":长度为");
                        m_sortInfo = strDef + tmpStr + tmpLen + ">" + QString::number(maxLen);
                        isOk = false;
                        break;
                    }
                }

                if(!isOk)
                {
                    break;
                }

                QList<int> valList = tmpPosMap.values();
                qSort(valList.begin(), valList.end());
                piecNum = valList.last();
                num = tmpPosMap.keys().length();

                if(piecNum > stdPiecNum)
                {
                    QString tmpPiecNum = QString::number(piecNum);
                    QString tmpStr = QStringLiteral(":单片缺陷个数");
                    m_sortInfo = strDef + tmpStr + tmpPiecNum + ">" + QString::number(stdPiecNum);
                    isOk = false;
                    break;
                }
                if(num > stdNum)
                {
                    QString tmpNum = QString::number(num);
                    QString tmpStr = QStringLiteral(":组件缺陷个数");
                    m_sortInfo = strDef + tmpStr + tmpNum + ">" + QString::number(stdNum);
                    isOk = false;
                    break;
                }
            }

            if(!isOk)
            {
                break;
            }
            lenIt++;
        }
        if(isOk)

        {
            m_level = leveList.at(levelIndex);
            break;
        }
        levelIndex++;
    }
    return isOk;
}

//设置跟Ai通信的相关参数
void AiThread::setParam(QImage &img, QString strUrl, int horNum, int verNum)
{
    this->m_img = img.copy();
    this->m_strUrl = strUrl;
    this->m_horNum = horNum;
    this->m_verNum = verNum;
}

void AiThread::setParams(SortMap &sortMap, QSqlDatabase &db, const QSqlDatabase &db2, const QString &wkshop,
                         QStringList sortList,  ClientParam &param, QString savePath,  QString &code, const QStringList &dgList)
{
    m_sortMap = sortMap;
    m_db = db;
    m_db2 = db2;
    m_wkshop = wkshop;
    m_sortList = sortList;
    // qDebug()<<"AiThd sortList "<<m_sortList;
    m_param = param;
    m_savePath = savePath;
    m_code = code;
    m_dgList = dgList;
}

//读取配置文件中的AI模型
void AiThread::readAiModel(QString fileName)
{
    QStringList defList;
    defList<<QStringLiteral("隐裂")<<QStringLiteral("破片")<<QStringLiteral("隐裂不")<<QStringLiteral("虚焊")<<QStringLiteral("轻虚")
          <<QStringLiteral("非虚")<<QStringLiteral("虚焊不")<<QStringLiteral("明暗片")<<QStringLiteral("不上电")<<QStringLiteral("明暗突变")
         <<QStringLiteral("短路")<<QStringLiteral("黑边")<<QStringLiteral("断栅块")<<QStringLiteral("断栅")<<QStringLiteral("划伤")
        <<QStringLiteral("吸盘印")<<QStringLiteral("黑角")<<QStringLiteral("污染")<<QStringLiteral("遮挡");
    QStringList clsList;
    clsList<<QStringLiteral("隐裂")<<QStringLiteral("隐裂")<<QStringLiteral("隐裂")<<QStringLiteral("虚焊")<<QStringLiteral("虚焊")
          <<QStringLiteral("虚焊")<<QStringLiteral("虚焊")<<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他")
         <<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他")
        <<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他")<<QStringLiteral("其他");
    for(int i =0; i< 19; i++)
    {
        QString strDef = defList.at(i);
        m_Maps[i+1] = defList.at(i);
        m_clsMap[strDef] = clsList.at(i);
    }
}

void AiThread::setAiModel(QMap<int, QString> &defMap, QMap<QString, QString> &clsMap)
{
    m_clsMap.clear();
    m_Maps.clear();

    m_Maps = defMap;
    m_clsMap = clsMap;
}

void AiThread::setPixels(const int top, const int bottom, const int left, const int right)
{
    m_topPixel = top;
    m_bottomPixel = bottom;
    m_leftPixel = left;
    m_rightPixel = right;
}

bool AiThread::waitForConnect()
{

    return true;
}

void AiThread::sleepMs(int ms)
{
    QDateTime dieTime = QDateTime::currentDateTime().addMSecs(ms);
    while(QDateTime::currentDateTime() < dieTime && (!m_isStop))
    {
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void AiThread::slot_waitTimeout()
{
    QString info = QStringLiteral("Ai等待超时");
    emit sig_aiInfo(info);
    m_isTimeout = true;
    m_isStop = true;
    emit signal_waitTimeout();
}

void AiThread::slot_readFinished()
{
    this->m_isStop = true;
}

void AiThread::slot_sortResult(int handle, bool isOk, QString level)
{
    emit sig_aiResult(isOk, false, m_strUrl, level, m_imgPath, handle, m_img, m_defectList, m_posList, m_clsList, m_areaMap, m_lenMap, m_posMap);
}

void AiThread::slot_sort_info(bool res, QString info, QString level)
{
    emit sig_sort_info(res, info, level, m_handle, m_strUrl);
}
