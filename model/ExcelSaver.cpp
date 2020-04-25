#include "ExcelSaver.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

ExcelSaver::ExcelSaver(QStringList &header, QStringList &data, IOStyle style, QString fileName)
    :m_header(header), m_data(data), m_style(style), m_fileName(fileName)
{

}

ExcelSaver::ExcelSaver()
{

}

void ExcelSaver::setParam(const QStringList &header, const QStringList &data, IOStyle style, QString fileName)
{
    m_header = header;
    m_data = data;
    m_style = style;
    m_fileName = fileName;
}

void ExcelSaver::save()
{
    QFile *file = new QFile(m_fileName);
    bool isExists = file->exists();
    bool isOpen = true;

    switch (m_style) {
    case ReadWrite:
        isOpen = file->open(QIODevice::ReadWrite);
        break;
    case Appened:
        isOpen = file->open(QIODevice::Append);
        break;
    default:
        break;
    }

    if(!isOpen)
    {
        return;
    }

    SACsvStream csv(file);
    QTextStream stream(file);
    stream.setCodec(QTextCodec::codecForName("GB2312"));
    csv.setTextStream(&stream);

    if(!isExists)
    {
        for(int i=0; i<m_header.length(); i++)
        {
            csv<<m_header.at(i);
        }
        csv<<endl;
    }

    for(int i=0; i<m_data.length(); i++)
    {
        csv<<m_data.at(i);
    }
    csv<<endl;

    file->close();
}
