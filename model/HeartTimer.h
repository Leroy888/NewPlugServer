#ifndef HEARTTIMER_H
#define HEARTTIMER_H

#include <QObject>
#include <QTimer>

class HeartTimer : public QTimer
{
    Q_OBJECT
public:
    HeartTimer(const int handle);
    void timerStart(const int ms);
    bool isStart() ;
    void stopTimer();

private slots:
    void slot_timeout();

signals:
    void sig_timeout(int);
private:
    int m_handle;

};

#endif // HEARTTIMER_H
