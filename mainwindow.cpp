﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QTableWidgetItem>
#include <QModelIndex>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#include <QImageReader>
#include "com/clsSettings.h"
#include "model/AiThread.h"
#include "model/SortThread.h"
#include <QMutexLocker>
#include <QThread>
#include <QDateTime>
#include <QTime>
#include "model/ExcelSaver.h"
#include "model/Saver.h"
#include "view/ImageViewer.h"
#include "view/StandardDlg.h"
#include "view/AiPortDialog.h"
#include "view/ImageSetDialog.h"
#include "view/NgDialog.h"
#include <QSound>
#include "view/PwdDialog.h"
#include <QTextCodec>
#include <QThread>

#include <QDebug>

#define ROW 3
#define COLUMN 3
#define DB_COLUMN 9
#define SORT_COLUMN 15


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_curNgImgIndex(-1),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("OptPlugServer"));
    this->showMaximized();

    connect(this, SIGNAL(ServerRecved(qintptr,QTcpSocket*,QByteArray)), this, SLOT(ServerRecvedSlot(qintptr,QTcpSocket*,QByteArray)));
    connect(this, SIGNAL(readyReadData(qintptr, QTcpSocket*)),this,SLOT(readData(qintptr,QTcpSocket*)));
    model_ = new QStandardItemModel(this);

    tcp_server_ = new TcpServer(this);
    if(!tcp_server_->listen(QHostAddress::Any, 8000)) {
        qDebug()<<tcp_server_->errorString();    //错误信息
        return;
    }
    qDebug()<<"listening";    //错误信息
    model_->insertRow(0,new QStandardItem("listening"));
    connect(tcp_server_,
            &TcpServer::ClientConnected,
            this,
            &MainWindow::ClientConnected);//监听
    connect(tcp_server_,
            &TcpServer::ClientDisconnected,
            this,
            &MainWindow::ClientDisconnected);//监听

    m_excelSaver = new ExcelSaver();

//    m_heartTimer = new QTimer(this);
//    connect(m_heartTimer,SIGNAL(timeout()),this,SLOT(slot_heatTimeout()));
//    m_heartTimer->start(50000);

    m_testTimer = new QTimer;
    connect(m_testTimer,SIGNAL(timeout()),this,SLOT(slot_testTimeout()));

    m_ImgForm = nullptr;
    m_handle = -1;
    m_isFinished = true;
    m_sortNum = 0;

    ui->btnTest->setVisible(false);

    readSettings();

    initUi();
    init_Tb_client();
    initTableWidget_data();

    initDB2();
    initDB();
    readEquip();
    initLogic();

    // updateDB();
    m_sqlHeader<<"product"<<"sort"<<"defect"<<"enable"<<"guanChuan"<<"dg"<<"minArea"<<"maxArea"
              <<"minLen"<<"maxLen"<<"point1"<<"point2"<<"count_p"<<"count"<<"count_aff";

    m_isInit = true;
}

MainWindow::~MainWindow() {
    tcp_server_->close();
    delete tcp_server_;
    delete ui;
}

void MainWindow::readSettings()
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
        m_statusMap.insert(strUrl, false);
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

void MainWindow::sendImageToAi(QString &imgPath, QString &url, int handle)
{
    QString equip = m_paramMap.value(handle).info;
    SortMap sortMap = m_sortMap.value(equip);
    QList<double> list = m_imgParamMap.value(equip);
    if(list.length() < 5)
    {
        m_statusMap.insert(url, false);
        QMessageBox::information(this,QStringLiteral("提示"), QStringLiteral("图像设置里没有找到相应的设备号:\n") + equip, QMessageBox::Ok);
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
    connect(thd,SIGNAL(sig_aiResult(bool,QString,QString,QString,int,QImage,QStringList,QStringList,QStringList,AiDataMap,AiDataMap,QMap<QString,QStringList>)),
            this,SLOT(slot_aiResult(bool,QString,QString,QString,int,QImage,QStringList,QStringList,QStringList,AiDataMap,AiDataMap,QMap<QString,QStringList>)));
    connect(thd,SIGNAL(sig_aiDisConnect(QString,int)),this,SLOT(slot_aiDisconnect(QString,int)));
    connect(thd,SIGNAL(sig_sortResult(int,bool,QString)),this,SLOT(slot_sortResult(int,bool,QString)));
    connect(thd,SIGNAL(sig_sort_info(bool,QString,QString,int,QString)),this,SLOT(slot_sort_info(bool,QString,QString,int,QString)));
    connect(thd,SIGNAL(finished()),thd,SLOT(deleteLater()));
    thd->start();
}

void MainWindow::semdImageToAi(QString &imgPath, QString &url, int handle, SortThread &sortThd)
{

}

void MainWindow::updateDB()
{
    QSqlQuery query(m_db2);
    QString equip = QString("M1-L2-EL3-WG1");
    QString sql = QString("select * from tab_product where equip = '%1'").arg(equip);
    query.exec(sql);

    if(query.next())
    {
        sql = QString("insert tab_product values('%1','%2','%3')").arg(equip).arg("888").arg("AA,BB");
        query.exec(sql);

    }
    else
    {
        sql = QString("insert tab_product values('%1','%2','%3')").arg(equip).arg("999").arg("CA,QA");
        query.exec(sql);
    }
    query.clear();
}

void MainWindow::initLogic()
{
    m_aiTimer = new QTimer(this);
    connect(m_aiTimer,SIGNAL(timeout()),this,SLOT(slot_aiJudge()));
}

void MainWindow::slot_aiJudge()
{
    if(m_ngImgList.length() <= 0)
    {
        m_aiTimer->stop();
        return;
    }
    on_btnNg_clicked();
}

void MainWindow::initUi()
{
    ui->btnTest_2->setVisible(false);
    ui->textEdit->setReadOnly(true);
    ui->btnOk->setEnabled(false);
    ui->btnNg->setEnabled(false);
    ui->btnOk->setShortcut(Qt::Key_1);
    ui->btnNg->setShortcut(Qt::Key_9);
    m_pixmapItem = NULL;
    m_graphScene = new QGraphicsScene();
    ui->graphicsView->setScene(m_graphScene);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setBackgroundBrush(QBrush(QColor(10,10,10)));

    m_row = 0;
    m_column = 0;
}

void MainWindow::init_Tb_client()
{
    for(int i=0; i<ROW; i++)
    {
        ui->gridLayout_client->setRowStretch(i, 1);
    }
    for(int i=0; i<COLUMN; i++)
    {
        ui->gridLayout_client->setColumnStretch(i, 1);
    }

    QStringList clientList = m_imgParamMap.keys();
    for(int i=0; i<clientList.length(); i++)
    {
        int row = i / COLUMN;
        int column = i % COLUMN;

        ClientParam param;
        param.okNum = 0;
        param.ngNum = 0;
        param.missNum = 0;
        param.errorNum = 0;
        param.info = clientList.at(i);
        m_paramMap.insert(1, param);

        //更新数据到客户端列表

        ClientForm *client = new ClientForm();
        client->setParam(param);
        m_clientMap.insert(clientList.at(i), client);
        connect(client,SIGNAL(sig_switch(int,QString,bool)),this,SLOT(slot_clientSwitch(int,QString,bool)));
        connect(client,SIGNAL(sig_clear(int)),this,SLOT(slot_clear(int)));
        ui->gridLayout_client->addWidget(client, row, column);
    }
}

void MainWindow::initTableWidget_data()
{
    ui->tableWidget_data->clear();
    ui->tableWidget_data->verticalHeader()->setVisible(false);
    ui->tableWidget_data->setColumnCount(DB_COLUMN);
    ui->tableWidget_data->setAlternatingRowColors(true);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tableWidget_data->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    ui->tableWidget_data->setEditTriggers(QTableWidget::NoEditTriggers);
    QStringList header;
    header<<QStringLiteral("    条码    ")<<QStringLiteral("    车间    ")<<QStringLiteral("    产线    ")
         <<QStringLiteral("    AI结果    ")<<QStringLiteral("    复判结果    ")<<QStringLiteral("    缺陷信息    ")
        <<QStringLiteral("测试时间")<<QStringLiteral("保存路径")<<QStringLiteral("分选等级");
    ui->tableWidget_data->setHorizontalHeaderLabels(header);
}

void MainWindow::initDB()
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
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("数据库连接失败"),QMessageBox::Ok);
        return;
    }
    else
    {

    }
    m_db.exec("set NAMES UTF8");
}

void MainWindow::initDB2()
{
    qlonglong id = (qlonglong)QThread::currentThreadId();
    m_db2 = QSqlDatabase::addDatabase("QMYSQL",QString::number(id) + "_2");
    m_db2.setHostName("192.168.10.189");
    m_db2.setDatabaseName("fucv");
    m_db2.setUserName("root");
    m_db2.setPort(3306);
    m_db2.setPassword("123456");
    if(!m_db2.open())
    {
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("总控数据库连接失败"),QMessageBox::Ok);
        return;
    }
    m_db2.exec("set NAMES UTF8");
}

void MainWindow::readEquip()
{
    m_sortMap.clear();
    m_equipSortMap.clear();

    QStringList keys = m_imgParamMap.keys();
    for(int i=0; i< keys.length(); i++)
    {
        QString equip = keys.at(i);
        QSqlQuery query(m_db);
        QString strProduct = m_equipProductMap.value(equip);
        QString sql = QString("select * from  tab_recipe where product = '%1'").arg(strProduct);
        query.exec(sql);

        QString strSort = "";
        if(query.next())
        {
            strSort = query.value(1).toString();
        }
        QStringList sortList = strSort.split(",");
        m_equipSortMap.insert(equip, sortList);

        query.clear();

        readSorting(equip, sortList);
    }
}

void MainWindow::readSorting(QString equip, QStringList sortList)
{
    QString strOrder = m_equipProductMap.value(equip);
    qDebug()<<equip<<"product = "<<strOrder<<"sortList "<<sortList;
    QSqlQuery query(m_db);
    SortMap sortMap;
    for(int i=0; i<sortList.length(); i++)
    {
        if(sortList.at(i) == QString(""))
        {
            continue;
        }
        QString sql = QString("select * from tab_sort where product = '%1' and sort = '%2'")
                .arg(strOrder).arg(sortList.at(i));
        query.exec(sql);
        qDebug()<<"sql "<<sql;
        QString strSort = sortList.at(i);
        QMap<QString,QStringList> tabMap;
        int index = 0;
        while(query.next())
        {
            QString strDef;
            QStringList list;
            for(int i=2; i<SORT_COLUMN; i++)
            {
                if(i == 2)
                {
                    strDef = query.value(i).toString();
                }
                else
                {
                    list.append(query.value(i).toString());
                }
            }
            tabMap.insert(strDef, list);
            index++;
        }
        sortMap.insert(strSort, tabMap);
    }
    query.clear();
    m_sortMap.insert(equip, sortMap);
}

//更新在线的客户端
void MainWindow::update_Tb_client(int handle)
{
    //    QMap<int,ClientForm*>::iterator it;

    //    ui->gridLayout_client->removeWidget(m_clientMap.value(handle));
    //    delete m_clientMap.value(handle);
    //    m_clientMap.remove(handle);
    //    qDebug()<<"update_Tb_client delete handle : "<<handle;
    //    int i = 0;
    //    for(it=m_clientMap.begin(); it!=m_clientMap.end(); it++)
    //    {
    //        int row = i / ROW;
    //        int column = i % COLUMN;

    //        ui->gridLayout_client->addWidget(it.value(), row, column);
    //        i++;
    //    }
}

void MainWindow::update_Tb_client(const QString &strInfo, const int handle, bool value)
{
    if(m_clientMap.contains(strInfo))
    {
        m_clientMap.value(strInfo)->setClientEnabled(value);
    }
    slot_clientSwitch(handle, strInfo, value);
}

void MainWindow::ClientConnected(qintptr handle, QTcpSocket* socket) {
    model_->insertRow(0,new QStandardItem(QString::number(handle)+" connected"));

    socket->write("connected");

    m_socketMap.insert(handle, socket);

    HeartTimer *timer = new HeartTimer(handle);
    connect(timer, SIGNAL(sig_timeout(int)),this,SLOT(slot_heartTimeout(int)));
 //   qDebug()<<handle<<" insert timer";
 //   m_timerMap.insert(handle, timer);

    ClientParam param;
    param.handle = handle;
    m_paramMap.insert(handle, param);

    //  m_handle = handle;
    //    connect(socket, &QTcpSocket::readyRead,
    //            [=]() {
    //        emit ServerRecved(handle, socket, socket->readAll());
    //    });
    connect(socket, &QTcpSocket::readyRead,[=]() {
        emit readyReadData(handle, socket);
    });
}

void MainWindow::ClientDisconnected(qintptr handle)
{
    model_->insertRow(0,new QStandardItem(QString::number(handle)+" disconnected"));

    QString strInfo = m_paramMap.value(handle).info;
    m_socketMap.remove(handle);
//    m_timerMap.remove(handle);
    m_paramMap.remove(handle);
    m_row = 0;
    m_column = 0;
    m_equipStatusMap.remove(strInfo);
    update_Tb_client(strInfo, handle, false);
}

void MainWindow::ServerRecvedSlot(qintptr handle, QTcpSocket *socket, const QByteArray &data)
{
    QMutex mutex;
    mutex.lock();
    Q_UNUSED(handle);

    QString send_data = QString("%1 %2").
            arg(socket->peerAddress().toString()).
            arg(socket->peerPort());
    model_->insertRow(0,new QStandardItem(send_data));

    if(m_modelIndex == 1)
    {
        if(m_socketMap.value(handle))
        {
            m_socketMap.value(handle)->write("received");
            qDebug()<<"send recived";
        }
    }

//    if(m_timerMap.value(handle))
//    {
//        qDebug()<<"timer is not null";
//        m_timerMap.value(handle)->timerStart(50000);
//    }

      //  qDebug()<<handle<<"local recv data:"<<QString(strJson.toLocal8Bit().data());
        qDebug()<<handle<<"data "<<QString(data);

    QJsonDocument jdoc = QJsonDocument::fromJson(data);
    QJsonObject jObj = jdoc.object();


    QString info = jObj.value("EL_id").toString();
    int okNum = jObj.value("OK_Num").toInt();
    int ngNum = jObj.value("NG_Num").toInt();
    int missNum = jObj.value("WP_Num").toInt();
    int errorNum = jObj.value("LP_Num").toInt();
    QString line = m_equipLineMap.value(info);

    //更新每个数据
    ClientParam param = m_paramMap.value(handle);
    param.okNum = okNum;
    param.ngNum = ngNum;
    param.missNum = missNum;
    param.errorNum = errorNum;
    param.info = info;
    param.line = line;
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

    QString imgName = QString::fromLocal8Bit(jObj.value("Path").toString().toLocal8Bit().data());
    qDebug()<<"image name "<<imgName;
    QStringList pathList = m_equipPathMap.value(info);
    pathList.append(pathList);
    m_equipPathMap.insert(info, pathList);
    if(!m_equipStatusMap.contains(info))
    {
        m_equipStatusMap.insert(info, true);
        update_Tb_client(info, handle, true);
    }
    if(!m_equipSortMap.contains(info))
    {

        //  slot_clientSwitch(handle, info, true);
        // readEquip(info);
    }
    if(imgName == "")
    {
        return;
    }
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

    mutex.unlock();
}

QString MainWindow::getUrl()
{
    QMutex mutex;
    mutex.lock();

    QString url;
    QMap<QString,bool>::iterator it;
    for(it = m_statusMap.begin(); it != m_statusMap.end(); it++)
    {
        if(!it.value() && it.key() != "")
        {
            url = it.key();
            m_statusMap.insert(url, true);
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

void MainWindow::readData(qintptr handle, QTcpSocket *socket)
{
    QByteArray byteArray = socket->readAll();
    emit ServerRecved(handle, socket, byteArray);
}

void MainWindow::slot_imgFormClicked(QWidget* wid, int handle)
{
    if(!m_isFinished)
    {
        return;
    }

    m_curNgImgIndex = ui->horLayout->indexOf(wid);
    m_handle = handle;
    m_curImgPath = m_ngPathList.at(m_curNgImgIndex);
    qDebug()<<"m_curNgImgIndex "<<m_curNgImgIndex;
    for(int i=0; i<m_imgFormList.length(); i++)
    {
        if(i != m_curNgImgIndex)
        {
            m_imgFormList.at(i)->initUi();
        }
    }

    showImage(m_ngImgList.at(m_curNgImgIndex));

    //    QModelIndex index = ui->tbWid_list->indexAt(QPoint(x, y));
    //    qDebug()<<"clicked image handle = "<<handle<<" index = "<<index.column();
    //    int row = index.row();
    //    int column = index.column();

    //    m_curNgImgIndex = column;

    //    m_curImg = image.copy();
    //    m_handle = handle;
    //    m_curImgPath = imgPath;
    //    qDebug()<<"curImgPath = "<<m_curImgPath;

    //    ui->btnOk->setEnabled(true);
    //    ui->btnNg->setEnabled(true);
    //    showImage(m_curImg);
    //    qDebug()<<"handle = "<<handle<<" show image";

}

void MainWindow::slot_imgFormClicked(QPoint &p, QImage &image, int handle, QString &path)
{
    //    QModelIndex index = ui->tbWid_list->indexAt(p);
    //    int column = index.column();
    //    m_curNgImgIndex = column;
    //    m_curImg = image;
    //    m_handle = handle;
    //    m_curImgPath = path;

    //    ui->btnOk->setEnabled(true);
    //    ui->btnNg->setEnabled(true);
    //    showImage(m_curImg);
}

void MainWindow::showImage(QImage image)
{
    QPixmap pixMap(QPixmap::fromImage(image));

    if(m_pixmapItem)
    {
        m_graphScene->removeItem(m_pixmapItem);
        delete m_pixmapItem;
        m_pixmapItem = NULL;
    }
    m_graphScene->clear();
    ui->graphicsView->resetTransform();

    m_graphScene->setSceneRect(image.rect());
    m_pixmapItem = m_graphScene->addPixmap(pixMap);

    float fScale1 = (ui->graphicsView->width() - 50)*1.0 / image.width();
    float fScale2 = (ui->graphicsView->height() - 20)*1.0 / image.height();
    m_fScale = fScale1;
    if(fScale1 > fScale2)
        m_fScale = fScale2;
    ui->graphicsView->scale(m_fScale, m_fScale);

    drawHorizontalRuler(m_horPieces);
    drawLeftRuler(m_verPieces);
}

void MainWindow::drawHorizontalRuler(int num)
{
    QString strDev = m_paramMap.value(m_handle).info;
    QList<double> imgParams = m_imgParamMap.value(strDev);
    num = imgParams.at(1);
    int topPix = imgParams.at(3);
    int leftPix = imgParams.at(5);
    int rightPix = imgParams.at(6);

    if(num == 0)
        return;

    int width = (m_graphScene->width() - leftPix - rightPix) / num;
    int x = m_pixmapItem->x() + leftPix;

    for(int i=0; i<num; i++)
    {
        QGraphicsTextItem *item = new QGraphicsTextItem;
        item->setFont(QFont("Arial", 35));
        item->setHtml(QString("<font color=red>%1</front>").arg(i+1));
        item->setX(x + width / 2 + width*i);
        item->setY(-1*topPix - 50 - 10);
        m_graphScene->addItem(item);
    }

    QGraphicsLineItem *lineItem = new QGraphicsLineItem;
    QPen pen;
    pen.setColor(QColor(Qt::green));
    pen.setWidth(3);
    lineItem->setPen(pen);
    lineItem->setLine(0 + leftPix, -1*topPix, m_graphScene->width() - rightPix, -1*topPix);
    m_graphScene->addItem(lineItem);
    int wid = width ;
    for(int i=0;i<num + 1;i++)
    {
        QGraphicsLineItem *item = new QGraphicsLineItem;
        item->setPen(pen);
        item->setLine(i*wid + leftPix, -1*topPix, i*wid + leftPix, -30 - topPix);
        m_graphScene->addItem(item);
    }
}

void MainWindow::drawLeftRuler(int num)
{
    QString strDev = m_paramMap.value(m_handle).info;
    QList<double> imgParams = m_imgParamMap.value(strDev);
    num = imgParams.at(2);
    int topPix = imgParams.at(3);
    int btmPix = imgParams.at(4);

    if(num == 0)
        return;
    QStringList list;
    list<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K";
    int height = m_graphScene->height() / num;
    int posY = m_pixmapItem->y();

    int index = imgParams.at(0);

    if(index)
    {
        for(int i=0;i<num;i++)
        {
            QGraphicsTextItem *item = new QGraphicsTextItem;
            item->setFont(QFont("Arial", 35));
            item->setHtml(QString("<font color=red>%1</front>").arg(list.at(num - i - 1)));
            item->setX(-45);
            item->setY(posY + height / 2 + height * i);
            m_graphScene->addItem(item);
        }
    }
    else
    {
        for(int i=0;i<num;i++)
        {
            QGraphicsTextItem *item = new QGraphicsTextItem;
            item->setFont(QFont("Arial", 35));
            item->setHtml(QString("<font color=red>%1</front>").arg(list.at(i)));
            item->setX(-45);
            item->setY(posY + height / 2 + height * i);
            m_graphScene->addItem(item);
        }
    }

    QGraphicsLineItem *lineItem = new QGraphicsLineItem;
    QPen pen;
    pen.setColor(Qt::green);
    pen.setWidth(3);
    lineItem->setPen(pen);
    lineItem->setLine(-5, 0 + topPix, -5, m_graphScene->height() - btmPix);
    m_graphScene->addItem(lineItem);

    int y = (m_graphScene->height() - topPix - btmPix ) / num;
    for(int i=0; i<num + 1; i++)
    {
        QGraphicsLineItem *item = new QGraphicsLineItem;
        item->setPen(pen);
        item->setLine(-35, i*y + topPix, -5, i*y + topPix);
        m_graphScene->addItem(item);
    }
}

void MainWindow::getResult(bool value)
{
    if(value)
    {

    }
}

void MainWindow::sendResultToEl(QImage img, bool value, QStringList defList, QString strRes)
{

}

void MainWindow::sendResultToEl(bool value, QString strRes)
{
    QJsonObject obj;
    obj.insert("result", value);
    obj.insert("level", strRes);
    //    obj.insert("defect", strDef);
    //    obj.insert("pos", strPos);
    QJsonDocument jdoc;
    jdoc.setObject(obj);
    QByteArray data = jdoc.toJson();

    if(!m_socketMap.contains(m_handle))
    {
        return;
    }
    m_socketMap.value(m_handle)->write(data);
}

void MainWindow::on_btnOk_clicked()
{
    if(m_aiTimer->isActive())
    {
        m_aiTimer->stop();
    }

    if(m_curNgImgIndex < 0)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择要判断的图片"),QMessageBox::Ok);
        m_isFinished = true;
        return;
    }

//    if(m_timerMap.value(m_handle))
//    {
//        if(m_timerMap.value(m_handle)->isStart())
//        {
//            m_timerMap.value(m_handle)->stopTimer();
//        }
//    }

    m_isFinished = false;

    ui->btnOk->setEnabled(false);
    ui->btnNg->setEnabled(false);

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

    ClientParam param = m_paramMap.value(m_handle);
    Saver *saver = new Saver(param, m_curImg, m_db, m_db2, m_savePath, m_savePath2, imgName, true, "");
    saver->setParam(m_excelSaver, m_wkshop, level);
    connect(saver,SIGNAL(finished()),saver,SLOT(deleteLater()));
    saver->start();

    updateImgWidget(column);

    m_isFinished = true;
    showNextImage();
}

void MainWindow::on_btnNg_clicked()
{
    if(m_aiTimer->isActive())
    {
        m_aiTimer->stop();
    }

    if(m_curNgImgIndex < 0)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择要判断的图片"),QMessageBox::Ok);
        m_isFinished = true;
        return;
    }

//    if(m_timerMap.value(m_handle))
//    {
//        if(m_timerMap.value(m_handle)->isStart())
//        {
//            m_timerMap.value(m_handle)->stopTimer();
//        }
//    }

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
    if(m_modelIndex == 0)
    {
        NgDialog *ngDlg = new NgDialog(isTurn, horPic, verPic, m_mesDefList);
        if(ngDlg->exec() == QDialog::Accepted)
        {
            posList = ngDlg->getPosList();
            defList = ngDlg->getDefList();

            delete ngDlg;
        }
        else
        {
            bool isOk = ngDlg->getIsOk();
            if(!isOk)
            {
                delete ngDlg;
                return;
            }
        }
    }

    m_isFinished = false;

    ui->btnOk->setEnabled(false);
    ui->btnNg->setEnabled(false);

    QJsonDocument jdoc;
    QJsonObject jobj;

    QString strDef = stringListToString(defList);
    QString strPos = stringListToString(posList);

    jobj.insert("defect", strDef);
    jobj.insert("pos", strPos);

    jdoc.setObject(jobj);
    QByteArray res = jdoc.toJson();

    if(m_socketMap.contains(m_handle))
    {
        if(m_modelIndex == 0)
        {
            m_socketMap.value(m_handle)->write(res);
        }
    }

    QString level = m_ngLevelList.at(m_curNgImgIndex);
    m_positionLists.removeAt(m_curNgImgIndex);
    m_classLists.removeAt(m_curNgImgIndex);
    m_ngLevelList.removeAt(m_curNgImgIndex);

    int column = m_curNgImgIndex;
    QString imgName = m_curImgPath.split("/").last();
    qDebug()<<m_handle<<imgName<<" On BtnNG : "<<QString(res);

    if(!defList.contains(QStringLiteral("重拍")))
    {
        ClientParam param = m_paramMap.value(m_handle);
        Saver *saver = new Saver(param, m_curImg, m_db, m_db2, m_savePath, m_savePath2, imgName, false, strDef);
        saver->setParam(m_excelSaver, m_wkshop, level);
        connect(saver,SIGNAL(finished()),saver,SLOT(deleteLater()));
        saver->start();
    }

    updateImgWidget(column);

    m_isFinished = true;
    showNextImage();
}

QString MainWindow::stringListToString(QStringList list)
{
    QString str = "";
    for(int i=0; i<list.length(); i++)
    {
        if(i != list.length() - 1)
        {
            str = str + list.at(i) + ",";
        }
        else
        {
            str += list.at(i);
        }
    }

    return str;
}

void MainWindow::sorting()
{
    QMutexLocker locker(&m_mutex);
    QString equip = m_paramMap.value(m_handle).info;
    SortMap sortMap = m_sortMap.value(equip);
    AiDataMap areaMap = m_areaMapList.at(m_curNgImgIndex);
    AiDataMap lenMap = m_lenMapList.at(m_curNgImgIndex);
    QMap<QString,QStringList> posMap = m_posMapList.at(m_curNgImgIndex);

    QStringList defList =  m_defectLists.at(m_curNgImgIndex);
    QStringList posList = m_positionLists.at(m_curNgImgIndex);
    QStringList clsList = m_classLists.at(m_curNgImgIndex);

    SortThread *thd = new SortThread(sortMap, m_handle, m_curImgPath, m_sortList, defList, areaMap, lenMap, posMap);
    QDateTime dateTime = QDateTime::currentDateTime();
    QString code = dateTime.toString("yyyyMMddhhmmss");
    code = m_curImgPath.split("/").last();
    int len = code.length();
    code = code.remove(len - 4, 4);
    QImage img = m_curImg.copy();
    thd->setParams(m_db, m_db2, m_wkshop, m_paramMap.value(m_handle), m_savePath, img, code, false, m_dgList);
    thd->setPosList(posList);
    thd->setClsMap(m_clsMap);
    connect(thd,SIGNAL(sig_sort_result(int,bool,QString)),this,SLOT(slot_sortResult(int,bool,QString)));
    connect(thd,SIGNAL(sig_sort_info(QString,QString)),this,SLOT(slot_sort_info(QString,QString)));
    connect(thd,SIGNAL(finished()),thd,SLOT(deleteLater()));

    m_defectLists.removeAt(m_curNgImgIndex);
    m_positionLists.removeAt(m_curNgImgIndex);
    m_classLists.removeAt(m_curNgImgIndex);
    m_areaMapList.removeAt(m_curNgImgIndex);
    m_lenMapList.removeAt(m_curNgImgIndex);
    m_posMapList.removeAt(m_curNgImgIndex);

    thd->start();
    locker.unlock();
}

void MainWindow::updateImgWidget(int column)
{
    //添加线程锁
    QMutexLocker locker(&m_mutex);

    m_ngImgList.removeAt(column);
    m_ngPathList.removeAt(column);
    m_handleList.removeAt(column);

    ui->horLayout->removeWidget(m_imgFormList.at(column));
    delete m_imgFormList.at(column);
    m_imgFormList.removeAt(column);

    locker.unlock();

    if(!m_aiTimer->isActive())
    {
        m_aiTimer->start(m_delaySecs * 1000);
    }
}

void MainWindow::on_btnQuery_clicked()
{
    QString queryCmd = QString("select * from tab_result where");
    if(!ui->chkBox_date->isChecked() && !ui->chkBox_line->isChecked() && !ui->chkBox_res->isChecked())
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请选择查询条件"), QMessageBox::Ok);
        return;
    }
    ui->btnQuery->setEnabled(false);
    bool isChecked = false;
    if(ui->chkBox_res->isChecked())
    {
        QString res = ui->cmbBox_res->currentText();
        queryCmd += QString(" res = '%1'").arg(res);
        isChecked = true;
    }
    if(ui->chkBox_line->isChecked())
    {
        QString line = ui->cmbBox_line->currentText();
        if(isChecked)
        {
            queryCmd += QString(" and line = '%1'").arg(line);
        }
        else
        {
            queryCmd += QString(" line = '%1'").arg(line);
            isChecked = true;
        }
    }
    if(ui->chkBox_date->isChecked())
    {
        qint64 secs1 = ui->dateTimeEdit->dateTime().toSecsSinceEpoch();
        qint64 secs2 = ui->dateTimeEdit_2->dateTime().toSecsSinceEpoch();
        if(isChecked)
        {
            queryCmd += QString(" and time > %1 and time < %2").arg(QString::number(secs1)).arg(QString::number(secs2));
        }
        else
        {
            queryCmd += QString(" time > %1 and time < %2").arg(QString::number(secs1)).arg(QString::number(secs2));
        }
    }

    QSqlQuery query(m_db);

    query.exec(queryCmd);
    QList<QStringList> dataList;
    while(query.next())
    {
        QStringList list;
        for(int i=0; i<DB_COLUMN; i++)
        {
            list.append(query.value(i).toString());
        }
        dataList.append(list);
    }
    query.clear();

    ui->tableWidget_data->setRowCount(dataList.length());

    for(int i=0; i<dataList.length(); i++)
    {
        for(int j=0; j<DB_COLUMN; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            if(j == 6)
            {
                QString strDate = dataList.at(i).at(j);
                qint64 secs = strDate.toLong();
                strDate = QDateTime::fromSecsSinceEpoch(secs).toString("yyyy-MM-dd hh:mm:ss");
                item->setText(strDate);
            }
            else
            {
                item->setText(dataList.at(i).at(j));
            }
            ui->tableWidget_data->setItem(i, j, item);
        }
    }
    ui->btnQuery->setEnabled(true);
}

void MainWindow::on_btnQueryDefect_clicked()
{

}


void MainWindow::on_btnSort_clicked()
{
    PwdDialog *pwdDlg = new PwdDialog(this);
    if(pwdDlg->exec() == QDialog::Accepted)
    {
        QString strPwd = pwdDlg->getPwd();
        if(strPwd != QString("3825opt"))
        {
            QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral("密码错误"),QMessageBox::Ok);
            delete pwdDlg;
            return;
        }
    }
    else
    {
        delete pwdDlg;
        return;
    }

    SortingDialog *dlg = new SortingDialog(m_sortMap, m_equipSortMap, m_equipProductMap, m_sortList, m_horHeader, m_verHeader, m_pcList, m_dgList, this);

    qRegisterMetaType<SortMap>("SortMap");
    connect(dlg,SIGNAL(sig_applySorting(QStringList,QStringList,QStringList,QString,SortMap)),this,SLOT(slot_applySorting(QStringList,QStringList,QStringList,QString,SortMap)));
    connect(dlg,SIGNAL(sig_applySorting(QString,QStringList,QString,SortMap)),this,SLOT(slot_applySorting(QString,QStringList,QString,SortMap)));
    connect(dlg,SIGNAL(sig_addSorting(QString,QStringList)),this,SLOT(slot_addSorting(QString,QStringList)));
    connect(dlg,SIGNAL(sig_deleteSorting(QString,QString,QStringList)),this,SLOT(slot_deleteSorting(QString,QString,QStringList)));
    connect(dlg,SIGNAL(sig_modifySorting(QString,QString,QString,QStringList)),this,SLOT(slot_modifySorting(QString,QString,QString,QStringList)));

    dlg->exec();

    disconnect(dlg,SIGNAL(sig_applySorting(QStringList,QStringList,QStringList,QString,SortMap)),this,SLOT(slot_applySorting(QStringList,QStringList,QStringList,QString,SortMap)));
    disconnect(dlg,SIGNAL(sig_applySorting(QString,QStringList,QString,SortMap)),this,SLOT(slot_applySorting(QString,QStringList,QString,SortMap)));
    disconnect(dlg,SIGNAL(sig_addSorting(QString,QStringList)),this,SLOT(slot_addSorting(QString,QStringList)));
    disconnect(dlg,SIGNAL(sig_deleteSorting(QString,QString,QStringList)),this,SLOT(slot_deleteSorting(QString,QString,QStringList)));
    disconnect(dlg,SIGNAL(sig_modifySorting(QString,QString,QString,QStringList)),this,SLOT(slot_modifySorting(QString,QString,QString,QStringList)));

    delete dlg;
}

void MainWindow::slot_applySorting(QStringList equipList, QStringList verHeader, QStringList sortList, QString curText, SortMap sortMap)
{
    m_verHeader = verHeader;
    clsSettings *settings = new clsSettings("./cfg/Settings.ini");
    QString strNode = QString("Standard/");
    settings->writeSetting(strNode + "verHeader", m_verHeader);

    delete settings;

    QSqlQuery query(m_db);

    for(int i=0; i<equipList.length(); i++)
    {
        QString strSort = sortList.at(i);
        const QString strOrder = m_equipProductMap.value(equipList.at(i));

        //        QString sql = QString("select * from tab_equip where equip = '%1' and product = '%2'")
        //                .arg(equipList.at(i)).arg(strOrder);
        //        qDebug()<<"select sql = "<<sql;
        //        query.exec(sql);
        //        if(query.next())
        //        {
        //            sql = QString("update tab_equip set product = '%1', sort = '%2' where equip = '%3'").arg(strOrder)
        //                    .arg(strSort).arg(equipList.at(i));
        //            qDebug()<<"update sql: "<<sql;
        //        }
        //        else
        //        {
        //            sql = QString("insert into tab_equip values ('%1','%2','%3')").arg(equipList.at(i)).arg(strOrder).arg(strSort);
        //            qDebug()<<"insert sql: "<<sql;
        //        }
        //        query.exec(sql);
        //        qDebug()<<"sql = "<<sql;

        // sortList 等级1 等级2 等级3
        QString sql;
        QMap<QString,QStringList> defMap = sortMap.value(curText); //缺陷列表
        QMap<QString,QStringList>::iterator it = defMap.begin();

        for(it; it != defMap.end(); it++)
        {
            sql = QString("select * from tab_sort where product = '%1' and sort = '%2' and defect = '%3'")
                    .arg(strOrder).arg(curText).arg(it.key());
            query.exec(sql);
            if(query.next())
            {
                QStringList list = it.value();
                sql = QString("update tab_sort set ");

                for(int n=0; n<list.length(); n++)
                {
                    QString strSql;
                    if(n != list.length() - 1)
                    {
                        strSql = m_sqlHeader.at(n+3) + QString(" = '%1', ");
                    }
                    else
                    {
                        strSql = m_sqlHeader.at(n+3) + QString(" = '%1' ");
                    }
                    sql = sql + strSql.arg(list.at(n));
                }
                sql = sql + QString(" where equip = '%1' and product = '%2' and sort = '%3' and defect = '%4'")
                        .arg(equipList.at(i)).arg(strOrder).arg(curText).arg(it.key());

                query.exec(sql);

            }
            else
            {
                QStringList list = it.value();

                sql = QString("insert into tab_sort values ('%1','%2','%3'")
                        .arg(strOrder).arg(curText).arg(it.key());
                for(int n=0; n<list.length(); n++)
                {
                    QString strSql;
                    if(n != list.length() - 1)
                        strSql = QString(",'%1'").arg(list.at(n));
                    else
                    {
                        strSql = QString(",'%1')").arg(list.at(n));
                    }
                    sql = sql + strSql;
                }
                query.exec(sql);

            }
        }
        query.clear();
        QString key = equipList.at(i);
        SortMap tmpMap = m_sortMap.value(key);
        tmpMap.insert(curText, sortMap.value(curText));
        // m_sortMap.insert(key, sortMap);
        m_sortMap.insert(key, tmpMap);
    }

    QStringList keyList = m_sortMap.keys();
}

void MainWindow::slot_applySorting(QString equip, QStringList verHeader, QString sortText, SortMap sortMap)
{
    m_verHeader = verHeader;
    clsSettings *settings = new clsSettings("./cfg/Settings.ini");
    QString strNode = QString("Standard/");
    settings->writeSetting(strNode + "verHeader", m_verHeader);

    delete settings;

    const QString product = m_equipProductMap.value(equip);

    QSqlQuery query(m_db);

    QString sql;
    QMap<QString,QStringList> defMap = sortMap.value(sortText); //缺陷列表
    QMap<QString,QStringList>::iterator it = defMap.begin();
    for(it; it != defMap.end(); it++)
    {
        sql = QString("select * from tab_sort where product = '%1' and sort = '%2' and defect = '%3'")
                .arg(product).arg(sortText).arg(it.key());
        query.exec(sql);
        if(query.next())
        {
            QStringList list = it.value();
            sql = QString("update tab_sort set ");

            for(int n=0; n<list.length(); n++)
            {
                QString strSql;
                if(n != list.length() - 1)
                {
                    strSql = m_sqlHeader.at(n+3) + QString(" = '%1', ");
                }
                else
                {
                    strSql = m_sqlHeader.at(n+3) + QString(" = '%1' ");
                }
                sql = sql + strSql.arg(list.at(n));
            }
            sql = sql + QString(" where product = '%1' and sort = '%2' and defect = '%3'")
                    .arg(product).arg(sortText).arg(it.key());

            query.exec(sql);
        }
        else
        {
            QStringList list = it.value();
            sql = QString("insert into tab_sort values ('%1','%2','%3'")
                    .arg(product).arg(sortText).arg(it.key());
            for(int n=0; n<list.length(); n++)
            {
                QString strSql;
                if(n != list.length() - 1)
                    strSql = QString(",'%1'").arg(list.at(n));
                else
                {
                    strSql = QString(",'%1')").arg(list.at(n));
                }
                sql = sql + strSql;
            }
            query.exec(sql);
        }
    }
    query.clear();

    QMap<QString,QString>::iterator sIt;
    for(sIt = m_equipProductMap.begin(); sIt != m_equipProductMap.end(); sIt++)
    {
        if(sIt.value() == product)
        {
            QString tmpEquip = sIt.key();
            SortMap tmpMap = m_sortMap.value(tmpEquip);
            tmpMap.insert(sortText, sortMap.value(sortText));
            m_sortMap.insert(tmpEquip, tmpMap);
        }
    }

    updateDB2Sort(product, sortText, defMap);
}

void MainWindow::updateDB2Sort(const QString &product, const QString &sortText, QMap<QString,QStringList> &defMap)
{
    QSqlQuery query(m_db2);
    QString sql;
    sql = QString("delete from tab_sort where product = '%1' and sort = '%2'").arg(product).arg(sortText);
    query.exec(sql);

    QMap<QString,QStringList>::iterator it = defMap.begin();
    for(it; it != defMap.end(); it++)
    {
        QStringList list = it.value();
        sql = QString("insert into tab_sort values ('%1','%2','%3'")
                .arg(product).arg(sortText).arg(it.key());
        for(int n=0; n<list.length(); n++)
        {
            QString strSql;
            if(n != list.length() - 1)
                strSql = QString(",'%1'").arg(list.at(n));
            else
            {
                strSql = QString(",'%1')").arg(list.at(n));
            }
            sql = sql + strSql;
        }
        query.exec(sql);
    }
    query.clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<"Close MainWindow";
}

void MainWindow::slot_addSorting(const QString &equip, const QStringList &sortList)
{
    QString strSort;
    for(int i=0; i<sortList.length(); i++)
    {
        if(i != sortList.length() - 1)
        {
            strSort += sortList.at(i) + ",";
        }
        else
        {
            strSort += sortList.at(i);
        }
    }
    QString product = m_equipProductMap.value(equip);
    m_equipSortMap.insert(equip, sortList);

    QSqlQuery query(m_db);
    QString sql = QString("select * from tab_recipe where product = '%1'").arg(product);
    query.exec(sql);
    if(query.next())
    {
        sql = QString("update tab_recipe set sort = '%1' where product = '%2'").arg(strSort).arg(product);
        query.exec(sql);
    }
    else
    {
        sql = QString("insert tab_recipe values('%1','%2')")
                .arg(product).arg(strSort);
        query.exec(sql);
    }
    query.clear();

    updateDB2Recipe(product, strSort);
}

void MainWindow::updateDB2Recipe(const QString &product, const QString &sort)
{
    QSqlQuery query(m_db2);
    QMap<QString,QList<double>>::iterator it;
    QString sql;
    for(it = m_imgParamMap.begin(); it != m_imgParamMap.end(); it++)
    {
        QString equip = it.key();
        QString strPrd = m_equipProductMap.value(equip);
        if(strPrd == product)
        {
            sql = QString("select * from tab_equip where equip = '%1'").arg(equip);
            query.exec(sql);
            if(query.next())
            {
                sql = QString("update tab_equip set product = '%1' where equip = '%2'").arg(product).arg(equip);
            }
            else
            {
                sql = QString("insert tab_equip values('%1','%2')").arg(equip).arg(product);
            }
            query.exec(sql);
        }
    }
    sql = QString("select * from tab_recipe where product = '%1'").arg(product);
    query.exec(sql);
    if(query.next())
    {
        sql = QString("update tab_recipe set sort = '%1' where product = '%2'").arg(sort).arg(product);
    }
    else
    {
        sql = QString("insert tab_recipe values('%1','%2')").arg(product).arg(sort);
    }
    query.exec(sql);

    query.clear();
}

void MainWindow::slot_deleteSorting(const QString &equip, const QString &curSort, const QStringList &sortList)
{
    QString strSort;
    for(int i=0; i<sortList.length(); i++)
    {
        if(i != sortList.length() - 1)
        {
            strSort += sortList.at(i) + ",";
        }
        else
        {
            strSort += sortList.at(i);
        }
    }
    QString product = m_equipProductMap.value(equip);
    m_equipSortMap.insert(equip, sortList);

    QSqlQuery query(m_db);
    QString sql = QString("update tab_recipe set sort = '%1' where product = '%2'")
            .arg(strSort).arg(product);
    query.exec(sql);
    query.clear();

    SortMap sortMap = m_sortMap.value(equip);
    sortMap.remove(curSort);
    m_sortMap.insert(equip, sortMap);

    updateDB2Recipe(product, strSort);
}

void MainWindow::slot_modifySorting(const QString &equip, const QString &node, const QString &node1, const QStringList &sortList)
{
    QString strSort;
    for(int i=0; i<sortList.length(); i++)
    {
        if(i != sortList.length() - 1)
        {
            strSort += sortList.at(i) + ",";
        }
        else
        {
            strSort += sortList.at(i);
        }
    }
    QString product = m_equipProductMap.value(equip);
    m_equipSortMap.insert(equip, sortList);

    //修改产品批次列表中的产品分选列表
    QSqlQuery query(m_db);
    QString sql = QString("update tab_recipe set sort = '%1' where product = '%2'")
            .arg(strSort).arg(product);
    query.exec(sql);

    //修改分选列表中的分选名称
    sql = QString("update tab_sort set sort = '%1' where product = '%2' and sort = '%3'")
            .arg(node1).arg(product).arg(node);
    query.exec(sql);
    query.clear();

    SortMap sortMap = m_sortMap.value(equip);
    QMap<QString,QStringList> tmpMap = sortMap.value(node);
    sortMap.remove(node);
    sortMap.insert(node1, tmpMap);
    m_sortMap.insert(equip, sortMap);

    updateDB2Recipe(product, strSort);
}

void MainWindow::on_btnTest_clicked()
{
    if(ui->btnTest->isChecked())
    {
        m_testNum = 0;
        m_imgFormList.clear();
        m_testAiList.clear();
        m_testAiList.append("http://192.168.10.96:9001");
        m_testAiList.append("http://192.168.10.96:9002");
        m_testAiList.append("http://192.168.10.96:9003");
        m_testAiList.append("http://192.168.10.96:9004");
        m_testAiList.append("http://192.168.10.96:9005");
        m_testAiList.append("http://192.168.10.96:9006");
        m_testAiList.append("http://192.168.10.96:9007");
        m_testAiList.append("http://192.168.10.96:9008");

        m_imgNameList.clear();
        m_imgNameList.append("F:/fuck/fuck_1.jpg");
        m_imgNameList.append("F:/fuck/fuck_2.jpg");
        m_imgNameList.append("F:/fuck/fuck_3.jpg");
        m_imgNameList.append("F:/fuck/fuck_4.png");
        m_imgNameList.append("F:/fuck/fuck_5.jpg");
        m_imgNameList.append("F:/fuck/fuck_6.jpg");
        m_imgNameList.append("F:/fuck/fuck_7.jpg");
        m_imgNameList.append("F:/fuck/fuck_8.jpg");
        m_imgNameList.append("F:/fuck/fuck_9.jpg");

        m_testTimer->start(3000);
        ui->btnTest->setIcon(QIcon(":/images/images/stop.ico"));
    }
    else
    {
        if(m_testTimer->isActive())
        {
            m_testTimer->stop();
        }

        ui->btnTest->setIcon(QIcon(":/images/images/start.ico"));
    }
}

void MainWindow::slot_testTimeout()
{
    QImage image;
    if(m_testNum == 8)
        m_testNum = 0;
    QString ip = m_testAiList.at(m_testNum);

    QString imgName = "F:/fuck/fuck_4.png";
    image.load(imgName);
    if(image.isNull())
        return;
}

void MainWindow::sendNextImageToAi()
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
        m_statusMap.insert(url, true);
        sendImageToAi(imgName, url, handle);
        qDebug()<<handle<<" sendImageToAi "<<url;
        m_unSendImgList.removeAt(0);
    }

    mutex.unlock();
}

void MainWindow::slot_aiResult(bool isOk, QString url, QString level, QString imgPath, int handle, QImage img, QStringList defList, QStringList posList,
                               QStringList clsList, AiDataMap areaMap, AiDataMap lenMap, QMap<QString,QStringList> posMap)
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
        ui->horLayout->addWidget(imgForm);
        qDebug()<<"form index "<<ui->horLayout->indexOf(imgForm);
        if(m_ngImgList.length() == 1)
        {
            showNextImage();
        }
        mutex.unlock();
    }
    else
    {
        qDebug()<<handle<<" AI judge OK "<<imgPath;
//        if(m_timerMap.value(handle))
//        {
//            if(m_timerMap.value(handle)->isStart())
//            {
//                m_timerMap.value(handle)->stopTimer();
//            }
//        }

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

        return;

        QString pcInfo = m_paramMap.value(handle).info;
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

        //  QString imgPath = m_paramMap.value(m_handle)
        QString imgName = imgPath.split("/").last();
        QString path = m_savePath + "/" + strDate + "/" + pcInfo + dute + "/" + strRes ;
        qDebug()<<handle<<" save path = "<<path;
        Functions::mkDir(path);
        img.save(path + "/" + imgName);

        QString strCode = curTime.toString("yyyyMMddhhmmss");
        strCode = imgName.remove(imgName.length() - 4, 4);

        saveExcel(path, strCode);

        int ss = curTime.currentSecsSinceEpoch();
        QString line = m_paramMap.value(handle).line;
        pcInfo = m_paramMap.value(handle).info;
        QString strAiRes = "OK";
        QStringList levels = m_equipSortMap.value(pcInfo);
        QString level = levels.first();

        QString sql = QString("insert into tab_result values ('%1',%2,'%3','%4','%5','%6','%7','%8','%9')").arg(strCode)
                .arg(ss).arg(line).arg(pcInfo).arg(strAiRes).arg(path).arg(strRes).arg(level);

        insertResult(sql);
    }
}

void MainWindow::saveExcel(QString &path, QString &code)
{
    QString fileName = path + QStringLiteral("/统计OK.csv");
    IOStyle style = Appened;

    QStringList header;
    header<<QStringLiteral("条码")<<QStringLiteral("缺陷")<<QStringLiteral("位置");
    QString strDef;
    QString strPos;

    QStringList data;
    data<<code<<strDef<<strPos;

    ExcelSaver *saver = new ExcelSaver(header, data, style, fileName);
    saver->save();
    delete saver;
}

void MainWindow::slot_sortResult(int handle, bool isOk, QString level)
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

    ui->btnOk->setEnabled(false);
    ui->btnNg->setEnabled(false);
}

void MainWindow::slot_sort_info(bool res, QString info, QString level, int handle, QString url)
{
    m_statusMap.insert(url, false);
    qDebug()<<handle<<" "<<url<<" set free";
    if(m_sortNum == 100)
    {
        ui->textEdit->clear();
        m_sortNum = 0;
    }
    QString sortInfo;
    if(res)
    {
        sortInfo = QString("<font color=\"#00ff00\">%1%2</font>").arg(info).arg(level);
    }
    else
    {
        sortInfo = QString("<font color=\"#ff0000\">%1%2</font>").arg(info).arg(level);
    }
    ui->textEdit->append(sortInfo);
    ui->textEdit->append("");
    m_sortNum++;
    qDebug()<<info<<level;
}

void MainWindow::insertResult(QString sql)
{
    QSqlQuery query(m_db);
    query.exec(sql);

    query.clear();
}

void MainWindow::showNextImage()
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

    ui->btnOk->setEnabled(true);
    ui->btnNg->setEnabled(true);
    showImage(m_curImg);
}

void MainWindow::on_btnPic_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::slot_aiDisconnect(QString url, int handle)
{
    qDebug()<<handle<<url<<" slot_aiDisconnect";
    m_statusMap.insert(url, false);
    QString strDev = m_paramMap.value(handle).info;
    QString info = strDev + " " + url + QStringLiteral(": AI处理图像超时，人工判定");
    ui->textEdit->append(QString("<font color=\"#ff0000\">%1</font>").arg(info));
    sendNextImageToAi();
}

void MainWindow::on_btnList_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btnData_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btnSettings_clicked()
{
    SettingsDialog *dlg = new SettingsDialog(this);
    dlg->setDelaySecs(m_delaySecs);
    dlg->setModel(m_modelIndex);
    dlg->setDbParam(m_dbParam, m_dbParam2);
    dlg->setSavePath(m_savePath);
    dlg->setSavePath2(m_savePath2);
    dlg->setOrder(m_productList, m_strOrder);
    dlg->setDgList(m_dgList);
    dlg->setWkshop(m_wkshop);
    dlg->setImgModel(m_imgModel);

    if(dlg->exec() == QDialog::Accepted)
    {
        QStringList productList = m_productList;
        m_delaySecs = dlg->getDelaySecs();
        m_modelIndex = dlg->getModel();
        m_savePath = dlg->getSavePath();
        m_savePath2 = dlg->getSavePath2();
        m_dbParam = dlg->getDbParam();
        m_dbParam2 = dlg->getDbParam2();
        m_strOrder = dlg->getOrder();
        m_productList = dlg->getOrderList();
        m_dgList = dlg->getDgList();
        m_wkshop = dlg->getWkshop();
        m_imgModel = dlg->getImgModel();
        clsSettings *settings = new clsSettings("./cfg/Settings.ini");
        QString strNode;
        strNode = QString("System/");
        settings->writeSetting(strNode + "delaySecs", m_delaySecs);
        settings->writeSetting(strNode + "horPieces", m_horPieces);
        settings->writeSetting(strNode + "verPieces", m_verPieces);
        settings->writeSetting(strNode + "upPixel", m_topPixel);
        settings->writeSetting(strNode + "bottomPixel", m_bottomPixel);
        settings->writeSetting(strNode + "leftPixel", m_leftPixel);
        settings->writeSetting(strNode + "rightPixel", m_rightPixel);
        settings->writeSetting(strNode + "modelIndex", m_modelIndex);
        settings->writeSetting(strNode + "savePath", m_savePath);
        settings->writeSetting(strNode + "savePath2", m_savePath2);
        settings->writeSetting(strNode + "orderList", m_productList);
        settings->writeSetting(strNode + "curOrder", m_strOrder);
        settings->writeSetting(strNode + "dgList", m_dgList);
        settings->writeSetting(strNode + "wkshop", m_wkshop);
        settings->writeSetting(strNode + "imgModel", m_imgModel);

        strNode = QString("Database/");
        settings->writeSetting(strNode + "dbIp", m_dbParam.ip);
        settings->writeSetting(strNode + "dbPort", m_dbParam.port);
        settings->writeSetting(strNode + "dbName", m_dbParam.name);
        settings->writeSetting(strNode + "dbUser", m_dbParam.user);
        settings->writeSetting(strNode + "dbPwd", m_dbParam.pwd);

        settings->writeSetting(strNode + "dbIp2", m_dbParam2.ip);
        settings->writeSetting(strNode + "dbPort2", m_dbParam2.port);
        settings->writeSetting(strNode + "dbName2", m_dbParam2.name);
        settings->writeSetting(strNode + "dbUser2", m_dbParam2.user);
        settings->writeSetting(strNode + "dbPwd2", m_dbParam2.pwd);

        delete settings;

        for(int i=0; i<m_productList.length(); i++)
        {
            QString product = m_productList.at(i);
            if(productList.indexOf(product) < 0)
            {
                QSqlQuery query(m_db);
                QString sql = QString("insert into tab_recipe values ('%1')").arg(product);
                query.exec(sql);
                query.clear();
            }
        }
    }
    delete dlg;
}

void MainWindow::on_tableWidget_data_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    int column = index.column();
    QTableWidgetItem *item = ui->tableWidget_data->item(row, column);
    QString path = item->text();
    QString imgName = ui->tableWidget_data->item(row, 0)->text();
    imgName = path + "/" + imgName + ".jpg";
    ImageViewer *viewer = new ImageViewer(imgName, this);
    viewer->showImage();
    viewer->exec();
    delete viewer;
}

void MainWindow::on_btnAiUrlSet_clicked()
{
    PwdDialog *pwdDlg = new PwdDialog(this);
    if(pwdDlg->exec() == QDialog::Accepted)
    {
        QString strPwd = pwdDlg->getPwd();
        if(strPwd != QString("opt3825"))
        {
            QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral("密码错误"),QMessageBox::Ok);
            delete pwdDlg;
            return;
        }
    }
    else
    {
        delete pwdDlg;
        return;
    }

    QStringList portList = m_statusMap.keys();
    QStringList headers;
    headers<<QStringLiteral("序号")<<QStringLiteral("端口设置");
    AiPortDialog *dlg = new AiPortDialog(portList, headers, QString("Item"), this);
    if(dlg->exec() == QDialog::Accepted)
    {
        portList = dlg->getList();
        clsSettings *settings = new clsSettings("./cfg/Settings.ini");
        QString strNode = QString("AiUrl/");

        QMap<QString,bool> tmpMap = m_statusMap;
        m_statusMap.clear();
        int portNum = 0;

        for(int i=0; i<portList.length(); i++)
        {
            portNum++;
            QString strUrl = portList.at(i);
            if(!tmpMap.contains(strUrl))
            {
                m_statusMap.insert(strUrl, false);
                QString key = QString("url_%1").arg(QString::number(portNum));
                settings->writeSetting(strNode + key, strUrl);
            }
            else
            {
                bool value = tmpMap.value(strUrl);
                m_statusMap.insert(strUrl, value);
            }
        }
        settings->writeSetting(strNode + "urlNum", portNum);

        delete settings;
    }
    delete dlg;
}

void MainWindow::on_btnImgSet_clicked()
{
    //    PwdDialog *pwdDlg = new PwdDialog(this);
    //    if(pwdDlg->exec() == QDialog::Accepted)
    //    {
    //        qDebug()<<"pwgDlg accept";
    //        QString strPwd = pwdDlg->getPwd();
    //        if(strPwd != QString("opt3825"))
    //        {
    //            QMessageBox::information(this,QStringLiteral("提示"),QStringLiteral("密码错误"),QMessageBox::Ok);
    //            delete pwdDlg;
    //            return;
    //        }
    //    }
    //    else
    //    {
    //        qDebug()<<"pwgDlg reject";
    //        delete pwdDlg;
    //        return;
    //    }

    ImageSetDialog *dlg = new ImageSetDialog(m_imgParamMap, m_equipProductMap, m_productList);
    if(dlg->exec() == QDialog::Accepted)
    {
        m_imgParamMap.clear();
        m_equipProductMap.clear();
        m_imgParamMap = dlg->getImageParamMap();
        m_equipProductMap = dlg->getEquipProductMap();

        clsSettings *settings = new clsSettings("./cfg/Settings.ini");
        QString strNode = QString("ImgParam/");
        int devNum = m_imgParamMap.count();
        settings->writeSetting(strNode + "devNum", devNum);

        int i = 1;
        QMap<QString,QList<double>>::iterator it;
        QSqlQuery query(m_db);
        for(it = m_imgParamMap.begin(); it != m_imgParamMap.end(); it++)
        {
            QString key = it.key();
            QList<double> list = it.value();
            const QString product = m_equipProductMap.value(key);

            settings->writeSetting(strNode + QString("dev_%1").arg(QString::number(i)), key);
            settings->writeSetting(strNode + QString("param_%1").arg(QString::number(i)), list);
            settings->writeSetting(strNode + QString("product_%1").arg(QString::number(i)), product);
            i++;

            QString strSort = "";

            m_equipProductMap.insert(key, product);

            QString sql = QString("select * from tab_equip where equip = '%1' and product = '%2'")
                    .arg(key).arg(product);
            query.exec(sql);
            if(!query.next())
            {
                sql = QString("insert into tab_equip values ('%1','%2','%3')").arg(key).arg(product).arg(strSort);
            }

            query.exec(sql);

            m_equipSortMap.clear();
            m_sortMap.clear();
            readEquip();
        }
        query.clear();
        delete settings;
    }
    delete dlg;
}

void MainWindow::slot_clientSwitch(const int handle, QString info, const bool value)
{
    if(value)
    {
        ui->label_status->setText(info + QStringLiteral(" 在线"));
        ui->label_status->setStyleSheet("background: rgb(18, 255, 18); font: 75 12pt");
    }
    else
    {
        ui->label_status->setText(info + QStringLiteral(" 离线"));
        ui->label_status->setStyleSheet("background: rgb(255, 18, 18); font: 75 12pt");
    }

    if(m_socketMap.contains(handle))
    {
        QJsonDocument jdoc;
        QJsonObject jobj;

        if(value)
        {
            jobj.insert("switch", QString("ON"));
        }
        else
        {
            jobj.insert("switch", QString("OFF"));
        }
        jdoc.setObject(jobj);
        QByteArray data = jdoc.toJson();
        if(m_socketMap.value(handle))
        {
            m_socketMap.value(handle)->write(data);
        }
    }
}

void MainWindow::slot_clear(int handle)
{
    if(m_socketMap.value(handle))
    {
        QJsonDocument jdoc;
        QJsonObject jobj;

        jobj.insert("cmd", QString("clear"));

        jdoc.setObject(jobj);
        QByteArray data = jdoc.toJson();
        m_socketMap.value(handle)->write(data);
        qDebug()<<handle<<" clear: "<<QString(data);
    }
}

void MainWindow::slot_heartTimeout(int handle)
{
    if(m_socketMap.value(handle))
    {
        m_socketMap.value(handle)->write("heart");
        qDebug()<<handle<<m_paramMap.value(handle).info<<"send heart";
    }
}