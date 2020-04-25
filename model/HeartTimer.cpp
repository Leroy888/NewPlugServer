#include "HeartTimer.h"

#include <QDebug>

HeartTimer::HeartTimer(const int handle) : m_handle(handle)
{
    connect(this,SIGNAL(timeout()),this,SLOT(slot_timeout()));
}

void HeartTimer::timerStart(const int ms)
{
    this->start(ms);
    qDebug()<<m_handle<<" start heart timer";
}

bool HeartTimer::isStart()
{
    if(this->isActive())
    {
        return true;
    }

    return false;
}

void HeartTimer::stopTimer()
{
    this->stop();
}

void HeartTimer::slot_timeout()
{
    qDebug()<<m_handle<<" emit sig_timeout";
    emit sig_timeout(m_handle);
}
