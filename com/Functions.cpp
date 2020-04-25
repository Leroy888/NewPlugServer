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

