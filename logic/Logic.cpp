#include "Logic.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSound>
#include "view/NgDialog.h"
#include "model/Saver.h"
#include "com/Functions.h"
#include "com/clsSettings.h"
#include "mainwindow.h"

#include <QDebug>

#define AI_TIMEOUT_COUNT 3

Logic::Logic(MainWindow *window, QMap<QString,ClientForm*> &clientMap) : m_window(window), m_clientMap(clientMap)
{
    readSettings();
    m_excelSaver = new ExcelSaver();

    m_ImgForm = nullptr;
    m_handle = -1;
    m_isFinished = true;
    m_sortNum = 0;

    initDB();
    m_sqlHeader<<"product"<<"sort"<<"defect"<<"enable"<<"guanChuan"<<"dg"<<"minArea"<<"maxArea"
              <<"minLen"<<"maxLen"<<"point1"<<"point2"<<"count_p"<<"count"<<"count_aff";

    m_isInit = true;

    m_tcpServer = new TcpServer;
    connect(m_tcpServer,SIGNAL(ClientConnected(qintptr,QTcpSocket*)),this,SLOT(slot_connected(qintptr,QTcpSocket*)));
    connect(m_tcpServer,SIGNAL(ClientDisconnected(qintptr)),this,SLOT(slot_disconnected(qintptr)));
    if(!m_tcpServer->listen(QHostAddress::Any, 8000))
    {
        qDebug()<<"TcpServer listen error:"<<m_tcpServer->errorString();
    }
    else
    {
        qDebug()<<"TcpServer is listening";
    }

}

Logic::~Logic()
{
    m_tcpServer->close();
    delete m_tcpServer;
}

void Logic::readSettings()
{
    clsSettings *settings = new clsSettings("./cfg/Settings.ini");
    QString strNode;
    strNode = QString("System/");
    settings->readSetting(strNode + "delaySecs", m_delaySecs);
    settings->readSetting(strNode + "horPieces", m_horPieces);
    settings->readSetting(strNode + "verPieces", m_verPieces);
    settings->readSetting(strNode + "upPixel", m_topPixel);
    settings->readSetting(strNode + "bottomPixel", m_bottomPixel);
    settings->readSetting(strNode + "leftPixel", m_leftPixel);
    settings->readSetting(strNode + "rightPixel", m_rightPixel);
    settings->readSetting(strNode + "modelIndex", m_modelIndex);
    settings->readSetting(strNode + "savePath", m_savePath);
    settings->readSetting(strNode + "orderList", m_productList);
    settings->readSetting(strNode + "curOrder", m_strOrder);
    settings->readSetting(strNode + "dgList", m_dgList);
    settings->readSetting(strNode + "wkshop", m_wkshop);
    settings->readSetting(strNode + "imgModel", m_imgModel);
    settings->readSetting(strNode + "ngDlgModel", m_ngDlgModel);

    strNode = QString("SharePath/");
    settings->readSetting(strNode + "shareIP", m_shareIP);
    settings->readSetting(strNode + "shareFolderName", m_shareFolderName);

    strNode = QString("Database/");
    settings->readSetting(strNode + "dbIp", m_dbParam.ip);
    settings->readSetting(strNode + "dbName", m_dbParam.name);
    settings->readSetting(strNode + "dbUser", m_dbParam.user);
    settings->readSetting(strNode + "dbPwd", m_dbParam.pwd);
    m_dbParam.port = 3306;

    settings->readSetting(strNode + "dbIp2", m_dbParam2.ip);
    settings->readSetting(strNode + "dbName2", m_dbParam2.name);
    settings->readSetting(strNode + "dbUser2", m_dbParam2.user);
    settings->readSetting(strNode + "dbPwd2", m_dbParam2.pwd);
    m_dbParam2.port = 3306;

    strNode = QString("Standard/");
    settings->readSetting(strNode + "def_num", m_stdNum);
    settings->readSetting(strNode + "horHeader", m_horHeader);
    settings->readSetting(strNode + "verHeader", m_verHeader);
    settings->readSetting(strNode + "sortList", m_sortList);
    settings->readSetting(strNode + "pcList", m_pcList);

    strNode = QString("ImgParam/");
    int devNum;
    settings->readSetting(strNode + "devNum", devNum);
    for(int i=1; i<devNum + 1; i++)
    {
        QString strDev;
        settings->readSetting(strNode + QString("dev_%1").arg(QString::number(i)), strDev);

        QString line;
        settings->readSetting(strNode + QString("line_%1").arg(QString::number(i)), line);
        m_equipLineMap.insert(strDev, line);

        QList<double> imgParamList;
        settings->readSetting(strNode + QString("param_%1").arg(QString::number(i)), imgParamList);
        m_imgParamMap.insert(strDev, imgParamList);

        QString product;
        settings->readSetting(strNode + QString("product_%1").arg(QString::number(i)), product);
        m_equipProductMap.insert(strDev, product);
    }

    strNode = QString("MesDef/");
    int mesDefNum = 0;
    settings->readSetting(strNode + "mesDefNum", mesDefNum);
    for(int i=1; i<mesDefNum + 1; i++)
    {
        QString strMesDef;
        settings->readSetting(strNode + QString("mesDef_%1").arg(QString::number(i)), strMesDef);
        m_mesDefList.append(strMesDef);
    }

    strNode = QString("AiUrl/");
    int portNum;
    settings->readSetting(strNode + "urlNum", portNum);
    for(int i=1; i<portNum + 1; i++)
    {
        QString strUrl;
        QString key = QString("url_%1").arg(QString::number(i));
        settings->readSetting(strNode + key, strUrl);
        AiStatus status;
        m_statusMap.insert(strUrl, status);
    }

    strNode = QString("AiModel/");
    QString clsNode = QString("AiClass/");

    int defNum;
    settings->readSetting(strNode + "DefectNum", defNum);
    for(int i=1; i< defNum+1; i++)
    {
        QString defNode = QString("defect_%1").arg(QString::number(i));
        QString strDef ;
        settings->readSetting(strNode + defNode,strDef);

        QString strCls;
        settings->readSetting(clsNode + QString("cls_%1").arg(QString::number(i)), strCls);

        m_defMap[i] = strDef;
        m_clsMap[strDef] = strCls;
    }

    for(int i=0; i < m_sortList.length(); i++)
    {
        QString key = m_sortList.at(i);
        QMap<QString,QStringList> defMap;
        for(int j=0; j<m_verHeader.length(); j++)
        {
            QStringList list;
            strNode = QString("SortStd_%1/").arg(QString::number(i));
            QString tmpKey = QString("def_%1").arg(QString::number(j));
            settings->readSetting(strNode + tmpKey, list);
            defMap.insert(tmpKey, list);
        }
        m_stdMap.insert(key, defMap);
    }

    delete settings;
}

void Logic::initDB()
{
    qlonglong id = (qlonglong)QThread::currentThreadId();
    m_db = QSqlDatabase::addDatabase("QMYSQL",QString::number(id));
    m_db.setHostName(m_dbParam.ip);
    m_db.setDatabaseName(m_dbParam.name);
    m_db.setUserName(m_dbParam.user);
    m_db.setPort(m_dbParam.port);
    m_db.setPassword(m_dbParam.pwd);
    if(!m_db.open())
    {
       // QMessageBox::warning(nullptr,QStringLiteral("提示"),QStringLiteral("数据库连接失败"),QMessageBox::Ok);
        return;
    }

    m_db.exec("set NAMES UTF8");
}

void Logic::slot_connected(qintptr handle, QTcpSocket *socket)
{
    m_socketMap.insert(handle, socket);

    ClientParam param;
    param.handle = handle;
    m_paramMap.insert(handle, param);
    qDebug()<<"slot_connected"<<handle<<"connected";

    connect(socket,&QTcpSocket::readyRead,this,&Logic::slot_readData);
}

void Logic::slot_disconnected(qintptr handle)
{
    m_socketMap.remove(handle);
    qDebug()<<"slot_disconnected"<<handle<<"disconnected";
}

void Logic::slot_readData()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*> (sender());
    QByteArray data = socket->readAll();
    qDebug()<<"data"<<data;
    const int handle = m_socketMap.key(socket);

    ClientParam param = m_paramMap.value(handle);
    parseData(handle, param, m_equipLineMap, data);
}

void Logic::sendNextImageToAi()
{
    QMutex mutex;
    mutex.lock();
    if(m_unSendImgList.length() == 0)
    {
        return;
    }
    QString url = getUrl();
    if(url != "")
    {
        QMap<QString,int> tmpMap = m_unSendImgList.at(0);
        QMap<QString,int>::iterator it = tmpMap.begin();
        QString imgName = it.key();
        int handle = it.value();
        AiStatus status;
        status.isBusy = false;
        m_statusMap.insert(url, status);
        sendImageToAi(imgName, url, handle);
        qDebug()<<handle<<" sendImageToAi "<<url;
        m_unSendImgList.removeAt(0);
    }

    mutex.unlock();
}

void Logic::showNextImage()
{
    if(m_ngImgList.length() <= 0)
    {
        m_curImg = m_curImg.scaled(0, 0);
        return;
    }

    m_curNgImgIndex = 0;

    m_curImg = m_ngImgList.at(0);
    m_handle = m_handleList.at(0);
    m_curImgPath = m_ngPathList.at(0);

//    ui->btnOk->setEnabled(true);
//    ui->btnNg->setEnabled(true);
//    showImage(m_curImg);
    emit sig_showImage(m_curImg);
}

void Logic::slot_aiResult(bool isOk, bool bTimeout, QString url, QString level, QString imgPath, int handle, QImage img, QStringList defList, QStringList posList, QStringList clsList, AiDataMap areaMap, AiDataMap lenMap, QMap<QString, QStringList> posMap)
{
    //添加线程锁
    m_aiOk = isOk;

    sendNextImageToAi();
    if(!isOk)
    {
        qDebug()<<handle<<" Ai judge ng";

        QMutex mutex;
        mutex.lock();
        QSound * sound = new QSound(":/sound/Audio.wav");
        sound->play();
        if(!bTimeout)
        {
            AiStatus status = m_statusMap.value(url);
            status.bTimeout = false;
            status.isBusy = false;
            status.timeoutNum = 0;

            m_statusMap.insert(url, status);
        }
        m_defList = defList;
        m_posList = posList;
        m_clsList = clsList;
        if(m_ngImgList.length()  <= 0)
        {
            m_aiTimer->start(m_delaySecs*1000);
        }
        QString info = m_paramMap.value(handle).info;

        m_defectLists.append(defList);
        m_positionLists.append(posList);
        m_classLists.append(clsList);

        m_ngLevelList.append(level);
        m_ngImgList.append(img);
        m_ngPathList.append(imgPath);
        m_handleList.append(handle);
        m_areaMapList.append(areaMap);
        m_lenMapList.append(lenMap);
        m_posMapList.append(posMap);

        ImgForm *imgForm = new ImgForm(info, imgPath, handle);
        connect(imgForm,SIGNAL(sig_clicked(QWidget*,int)),this,SLOT(slot_imgFormClicked(QWidget*,int)));
        connect(imgForm,SIGNAL(destroyed(QObject*)),imgForm,SLOT(deleteLater()));

        m_imgFormList.append(imgForm);
     //   ui->horLayout->addWidget(imgForm);
     //   qDebug()<<"form index "<<ui->horLayout->indexOf(imgForm);
        if(m_ngImgList.length() == 1)
        {
            showNextImage();
        }
        mutex.unlock();
    }
    else
    {
        qDebug()<<handle<<" AI judge OK "<<imgPath;
        QJsonDocument jdoc;
        QJsonObject jobj;
        jobj.insert("defect", "");
        jobj.insert("pos", "");

        jdoc.setObject(jobj);
        QByteArray res = jdoc.toJson();
        if(m_socketMap.contains(handle))
        {
            if(m_modelIndex == 0)
            {
                m_socketMap.value(handle)->write(res);
            }
        }
    }
}

void Logic::slot_aiDisconnect(QString url, int handle)
{
    qDebug()<<handle<<url<<" slot_aiDisconnect";
    AiStatus status = m_statusMap.value(url);
    if(status.bTimeout)
    {
        status.timeoutNum++;
    }

    if(status.timeoutNum < AI_TIMEOUT_COUNT)
    {
        status.isBusy = false;
        m_window->updateAiDisconnected(url);
    }

    m_statusMap.insert(url, status);
    QString strDev = m_paramMap.value(handle).info;
    QString info = strDev + " " + url + QStringLiteral(": AI处理图像超时，人工判定");
    m_window->appendText(QString("<font color=\"#ff0000\">%1</font>").arg(info));
    sendNextImageToAi();
}

void Logic::slot_sortResult(int handle, bool isOk, QString level)
{
    QJsonObject jObj;
    jObj.insert("result", isOk);
    jObj.insert("level", level);

    QJsonDocument jDoc(jObj);
    QByteArray cmd = jDoc.toJson();

    if(m_socketMap.contains(handle))
    {
        m_socketMap.value(handle)->write(cmd);
    }

    m_window->
}

void Logic::parseData(const int handle, ClientParam &param, const QMap<QString, QString> &lineMap, const QByteArray &data)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(data);
    QJsonObject jObj = jdoc.object();

    QString info = jObj.value("EL_id").toString();
    int okNum = jObj.value("OK_Num").toInt();
    int ngNum = jObj.value("NG_Num").toInt();
    int missNum = jObj.value("WP_Num").toInt();
    int errorNum = jObj.value("LP_Num").toInt();
    QString imgName = QString::fromLocal8Bit(jObj.value("Path").toString().toLocal8Bit().data());
    QString barCode = jObj.value("BarCode").toString();
    QString ngBarcode = jObj.value("NG_Bar").toString();
    QString ngCode = jObj.value("NG_Code").toString();
    QString ngSharePath = QString::fromLocal8Bit(jObj.value("NG_Shpath").toString().toLocal8Bit().data());
    QString strRecvInfo1 = QString("====ServerRecvedSlot: EL--->YRDJ. EL_id=%1 OK_Num=%2 NG_Num=%3 WP_Num=%4 LP_Num=%5").arg(info).arg(QString::number(okNum)).arg(QString::number(ngNum)).arg(QString::number(missNum)).arg(QString::number(errorNum));
    QString strRecvInfo2 = QString("====ServerRecvedSlot: EL--->YRDJ. Path=%1 BarCode=%2 NG_Bar=%3 NG_Code=%4 NG_Shpath=%5").arg(imgName).arg(barCode).arg(ngBarcode).arg(ngCode).arg(ngSharePath);
    qDebug()<< strRecvInfo1;
    qDebug()<< strRecvInfo2;

    //更新每个数据
    QString line = lineMap.value(info);
    param.okNum = okNum;
    param.ngNum = ngNum;
    param.missNum = missNum;
    param.errorNum = errorNum;
    param.info = info;
    param.line = line;
    param.barCode= barCode;
    m_paramMap.insert(handle, param);

    //更新数据到客户端列表
    if(!m_clientMap.contains(info))
    {
        ClientForm *form = new ClientForm();
        m_clientMap.insert(info, form);
    }
    ClientForm *form = m_clientMap.value(info);
    form->setParam(param);
    form->setClientEnabled(true);

    QStringList pathList = m_equipPathMap.value(info);
    pathList.append(pathList);
    m_equipPathMap.insert(info, pathList);
    if(!m_equipStatusMap.contains(info))
    {
        m_equipStatusMap.insert(info, true);
        emit sig_updateTb_client(info, handle, true);
    }
    if(!m_equipSortMap.contains(info))
    {
          emit sig_clientSwitch(handle, info, true);
    }

    //场景2：客户端通知一人多机保存NG到返修表
    if(!ngCode.isEmpty() && !ngSharePath.isEmpty())
    {
        if(m_modelIndex == 0) //常规模式。需要登记到返修表格
        {
            qDebug()<<"====ServerRecvedSlot: get save-respair cmd. work mode, save to repair table...";
//          insertPathEx(m_db,ngBarcode,ngCode,ngSharePath);

            return;
        }
        else
        {
            qDebug()<<"====ServerRecvedSlot: get save-respair cmd. test mode, not save to repair table.";
        }
    }

    //场景1：新图到了处理
    //新图到了
    if(imgName.isEmpty())
    {
     //   mutex.unlock();
        return;
    }

    qDebug()<<"====ServerRecvedSlot: image name="<<imgName<<" BarCode="<<barCode;

    //将从外挂接收到的图片发送到AI服务器
    QString url = getUrl();
    if(url != "")
    {
        sendImageToAi(imgName, url, handle);
        qDebug()<<"send image to ai, handle = "<<handle<<" -> "<<info;
    }
    else
    {
        QMap<QString,int> imgHandleMap;
        imgHandleMap.insert(imgName, handle);
        m_unSendImgList.append(imgHandleMap);
        qDebug()<<"urls are busy, handle = "<<handle<<" -> "<<info;
    }

    //    mutex.unlock();
}

bool Logic::judge(bool value)
{
    if(m_aiTimer->isActive())
    {
        m_aiTimer->stop();
    }

    if(m_curNgImgIndex < 0)
    {
        QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("请选择要判断的图片"),QMessageBox::Ok);
        m_isFinished = true;
        return false;
    }

    if(value)
    {
        if(m_aiTimer->isActive())
        {
            m_aiTimer->stop();
        }

        if(m_curNgImgIndex < 0)
        {
            QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("请选择要判断的图片"),QMessageBox::Ok);
            m_isFinished = true;
            return false;
        }

        m_isFinished = false;

        ClientParam param = m_paramMap.value(m_handle);
        QString strInfo = param.info;
        QString strLine = param.line;
        QDateTime curDate = QDateTime::currentDateTime();
        QString strDate = curDate.toString("yyyy-MM-dd");
        QTime curTime = QTime::currentTime();
        QString strDute = QStringLiteral("白班");
        if(curTime.hour() < 8 || curTime.hour() > 19)
        {
            strDute = QStringLiteral("晚班");
        }
        if(curTime.hour() < 8)
        {
            strDate = curDate.addDays(-1).toString("yyyy-MM-dd");
        }
        QString strRes = "OK";

        QJsonDocument jdoc;
        QJsonObject jobj;
        jobj.insert("defect", "");
        jobj.insert("pos", "");

        jdoc.setObject(jobj);
        QByteArray res = jdoc.toJson();

        if(m_socketMap.contains(m_handle))
        {
            if(m_modelIndex == 0)
            {
                m_socketMap.value(m_handle)->write(res);
            }
        }

        int column = m_curNgImgIndex;
        QString level = m_ngLevelList.at(m_curNgImgIndex);
        m_positionLists.removeAt(m_curNgImgIndex);
        m_classLists.removeAt(m_curNgImgIndex);
        m_ngLevelList.removeAt(m_curNgImgIndex);

        QString imgName = m_curImgPath.split("/").last();
        qDebug()<<m_handle<<imgName<<" On BtnOK : "<<QString(res);

        //ClientParam param = m_paramMap.value(m_handle);
        Saver *saver = new Saver(param, m_curImg, m_db, m_db2, m_savePath, m_savePath2, imgName, true, "");
        saver->setParam(m_excelSaver, m_wkshop, level);
        saver->setInfoParam(strDate,strInfo,strDute,strRes,strLine);
        connect(saver,SIGNAL(finished()),saver,SLOT(deleteLater()));
        saver->start();

        updateImgWidget(column);

        m_isFinished = true;
        showNextImage();
    }
    else
    {
        QString equip = m_paramMap.value(m_handle).info;
        SortMap sortMap = m_sortMap.value(equip);
        QMap<QString, QStringList> tmpMap;
        if(sortMap.keys().length() != 0)
        {
            QString key = sortMap.keys().at(0);
            tmpMap = sortMap.value(key);
        }

        int horPic = m_imgParamMap.value(equip).at(1);
        int verPic = m_imgParamMap.value(equip).at(2);
        bool isTurn = false;
        if(m_imgParamMap.value(equip).length() > 0 && m_imgParamMap.value(equip).at(0))
        {
            isTurn = true;
        }

        QStringList posList;
        QStringList defList;
        QString level;
        if(m_modelIndex == 0 && m_ngDlgModel == 0) //常规 且 显示缺陷选择对话框
        {
            NgDialog *ngDlg = new NgDialog(isTurn, horPic, verPic, m_mesDefList);
            if(ngDlg->exec() == QDialog::Accepted)
            {
                posList = ngDlg->getPosList();
                defList = ngDlg->getDefList();
                level = ngDlg->getLevel();

                delete ngDlg;
            }
            else
            {
                bool isOk = ngDlg->getIsOk();
                if(!isOk)
                {
                    delete ngDlg;
                    return false;
                }
            }
        }

        m_isFinished = false;

        QJsonDocument jdoc;
        QJsonObject jobj;

        QString strDef = Functions::stringListToString(defList);
        QString strPos = Functions::stringListToString(posList);
        if(m_ngDlgModel != 0)  //隐蔽缺陷选择对话框
        {
            strDef = QStringLiteral("虚焊");
            strPos = QString("J8");
        }

        ClientParam param = m_paramMap.value(m_handle);
        QString strInfo = param.info;
        QString strLine = param.line;
        QDateTime curDate = QDateTime::currentDateTime();
        QString strDate = curDate.toString("yyyy-MM-dd");
        QTime curTime = QTime::currentTime();
        QString strDute = QStringLiteral("白班");
        if(curTime.hour() < 8 || curTime.hour() > 19)
        {
            strDute = QStringLiteral("晚班");
        }
        if(curTime.hour() < 8)
        {
            strDate = curDate.addDays(-1).toString("yyyy-MM-dd");
        }
        //QString strRes = "OK";
        QString strRes = "NG";
        QString imgName = m_curImgPath.split("/").last();

        QString sharePath = QString("//") + m_shareIP + "/" + m_shareFolderName + "/"+ strDate + "/" + strInfo + "/" + strDute + "/" + strRes + "/" + imgName;

        jobj.insert("defect", strDef);
        jobj.insert("pos", strPos);
        jobj.insert("level", level);
        if(!defList.contains(QStringLiteral("重拍")))
        {
            jobj.insert("ng_bar",param.barCode);
            jobj.insert("ng_code",imgName.left(imgName.size()-4));
            jobj.insert("ng_shpath",sharePath);

            qDebug()<<"====on_btnNg_clicked: ng_bar=" << param.barCode<< "ng_code=" << imgName.left(imgName.size()-4) << "ng_shpath=" << sharePath;
        }

        jdoc.setObject(jobj);
        QByteArray res = jdoc.toJson();

        if(m_socketMap.contains(m_handle))
        {
            if(m_modelIndex == 0) //常规模式。向客户端回复NG
            {
                m_socketMap.value(m_handle)->write(res);

                qDebug()<<"====on_btnNg_clicked: YRDJ--->EL NG";
            }
        }

        m_positionLists.removeAt(m_curNgImgIndex);
        m_classLists.removeAt(m_curNgImgIndex);
        m_ngLevelList.removeAt(m_curNgImgIndex);

        int column = m_curNgImgIndex;
        qDebug()<<m_handle<<imgName<<" On BtnNG : "<<QString(res);

        if(!defList.contains(QStringLiteral("重拍")))
        {
            //ClientParam param = m_paramMap.value(m_handle);
            Saver *saver = new Saver(param, m_curImg, m_db, m_db2, m_savePath, m_savePath2, imgName, false, strDef);
            saver->setParam(m_excelSaver, m_wkshop, level);
            saver->setInfoParam(strDate,strInfo,strDute,strRes,strLine);
            connect(saver,SIGNAL(finished()),saver,SLOT(deleteLater()));
            saver->start();
        }

        updateImgWidget(column);

        m_isFinished = true;
        showNextImage();
    }
    return true;
}

void Logic::updateImgWidget(int column)
{
    //添加线程锁
    QMutexLocker locker(&m_mutex);

    m_ngImgList.removeAt(column);
    m_ngPathList.removeAt(column);
    m_handleList.removeAt(column);

//    ui->horLayout->removeWidget(m_imgFormList.at(column));
    m_window->removeImageForm(m_imgFormList.at(column));
    delete m_imgFormList.at(column);
    m_imgFormList.removeAt(column);

    locker.unlock();

    if(!m_aiTimer->isActive())
    {
        m_aiTimer->start(m_delaySecs * 1000);
    }
}

void Logic::sendImageToAi(QString &imgPath, QString &url, int handle)
{
    QString equip = m_paramMap.value(handle).info;
    SortMap sortMap = m_sortMap.value(equip);
    QList<double> list = m_imgParamMap.value(equip);
    if(list.length() < 5)
    {
        AiStatus status;
        m_statusMap.insert(url, status);
        QMessageBox::information(nullptr,QStringLiteral("提示"), QStringLiteral("图像设置里没有找到相应的设备号:\n") + equip, QMessageBox::Ok);
        return;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
    QString code = dateTime.toString("yyyyMMddhhmmss");
    code = m_curImgPath.split("/").last();
    int len = code.length();
    code = code.remove(len - 4, 4);
    ClientParam param = m_paramMap.value(handle);
    QStringList sortList = m_equipSortMap.value(equip);

    AiThread *thd = new AiThread(imgPath, url, m_horPieces, m_verPieces, handle);
    thd->setAiModel(m_defMap, m_clsMap);
    //thd->setPixels(m_topPixel, m_bottomPixel, m_leftPixel, m_rightPixel);
    thd->setParams(sortMap, m_db, m_db2, m_wkshop, sortList, param, m_savePath, code, m_dgList);

    int horPic = list.at(1);
    int verPic = list.at(2);
    int topPix = list.at(3);
    int btmPix = list.at(4);
    int leftPix = list.at(5);
    int rightPix = list.at(6);

    thd->setImageParam(horPic, verPic, topPix, btmPix, leftPix, rightPix, m_imgModel);

    qRegisterMetaType<AiDataMap>("AiDataMap");
    qRegisterMetaType<QMap<QString,QStringList> >("AiPosMap");
    connect(thd,SIGNAL(sig_aiResult(bool,bool,QString,QString,QString,int,QImage,QStringList,QStringList,QStringList,AiDataMap,AiDataMap,QMap<QString,QStringList>)),
            this,SLOT(slot_aiResult(bool,bool,QString,QString,QString,int,QImage,QStringList,QStringList,QStringList,AiDataMap,AiDataMap,QMap<QString,QStringList>)));
    connect(thd,SIGNAL(sig_aiDisConnect(QString,int)),this,SLOT(slot_aiDisconnect(QString,int)));
    connect(thd,SIGNAL(sig_sortResult(int,bool,QString)),this,SLOT(slot_sortResult(int,bool,QString)));
    connect(thd,SIGNAL(sig_sort_info(bool,QString,QString,int,QString)),this,SLOT(slot_sort_info(bool,QString,QString,int,QString)));
    connect(thd,SIGNAL(finished()),thd,SLOT(deleteLater()));
    thd->start();
}

QString Logic::getUrl()
{
    QMutex mutex;
    mutex.lock();

    QString url;
    QMap<QString,AiStatus>::iterator it;
    for(it = m_statusMap.begin(); it != m_statusMap.end(); it++)
    {
        AiStatus status = it.value();
        if(!status.isBusy && it.key() != "")
        {
            url = it.key();
            status.isBusy = true;
            m_statusMap.insert(url, status);
            qDebug()<<it.key()<<" is free";
            break;
        }
        else
        {
            qDebug()<<it.key()<<" is busy";
        }
    }
    mutex.unlock();

    return url;
}

QMap<QString, QList<double> > Logic::getImgParamMap() const
{
    return m_imgParamMap;
}

