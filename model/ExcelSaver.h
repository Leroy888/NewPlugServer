#ifndef EXCELSAVER_H
#define EXCELSAVER_H

#include <QObject>
#include "model/CsvStream.h"
#include "com/Functions.h"

class ExcelSaver
{
public:
    ExcelSaver(QStringList &header, QStringList &data, IOStyle style, QString fileName);
    ExcelSaver();

    void setParam(const QStringList &header, const QStringList &data, IOStyle style, QString fileName);

    void save();

private:
    QStringList m_header;
    QStringList m_data;
    IOStyle m_style;
    QString m_fileName;
};

#endif // EXCELSAVER_H
