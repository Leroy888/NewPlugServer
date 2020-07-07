#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getModel() const;

private slots:
    void on_btnOK_clicked();

private:
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
