#include "Functions.h"
#include <QDir>
#include <QDateTime>
#include <QApplication>

#include <QDebug>


Functions::Functions()
{

}

void Functions::sleepMs(int ms)
{
    QDateTime dieTime = QDateTime::currentDateTime().addMSecs(ms);
    while(QDateTime::currentDateTime() < dieTime)
    {
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void Functions::mkDir(QString strPath)
{
   QStringList pathList = strPath.split("/");
   QString path = "";
   for(int i=0;i<pathList.length(); i++)
   {
       path = path + pathList.at(i);
       if(!QDir(path).exists())
       {
           if(!QDir().mkdir(path))
           {
               return;
           }
       }
       path = path + "/";
   }
}

QString Functions::stringListToString(const QStringList &list)
{
    QString str;
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

void Functions::saveImage(const QImage &img, const QString &savePath, const QString &imgName, const QString &pcInfo, const QString &strRes)
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

    QString path = savePath + "/" + strDate + "/" +pcInfo + dute + "/" + strRes ;

    Functions::mkDir(path);
    img.save((path + imgName));
}


void OptDefects::clear()
{
    areaMap.clear();      //单片电池片上某一缺陷对应的长度列表
    lenMap.clear();    //单个带电池片缺陷对应的长度列表
    dgMap.clear();      //灰度值
    pointMap.clear();  //坐标
    posMap.clear(); //缺陷所在位置 A1、A2等
    piecNumMap.clear();     //电池片上某一缺陷的总数
    numMap.clear();         //组件上某一缺陷的总数
    ngNumMap.clear();       //一个组件对应的缺陷损坏的电池片数
}
