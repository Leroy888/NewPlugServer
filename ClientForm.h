#ifndef CLIENTFORM_H
#define CLIENTFORM_H

#include <QWidget>
#include "com/Functions.h"


namespace Ui {
class ClientForm;
}

class ClientForm : public QWidget
{
    Q_OBJECT

public:
    explicit ClientForm(QWidget *parent = 0);
    ~ClientForm();

    void setParam(ClientParam param);
    void setOk(int value);
    void setNg(int value);
    void setMiss(int value);
    void setError(int value);
    void setIp(QString strIp);
    void setClientEnabled(bool value);

private slots:
    void on_btnSettings_clicked();

    void on_btnSwitch_clicked();

    void on_btnClear_clicked();

signals:
    void sig_switch(int, QString, bool);
    void sig_clear(int);

private:
    Ui::ClientForm *ui;

    int m_handle;
};

#endif // CLIENTFORM_H
