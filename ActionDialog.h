#ifndef ACTIONDIALOG_H
#define ACTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ActionDialog;
}

class ActionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ActionDialog(QWidget *parent = 0);
    ~ActionDialog();
    QString getNode();
    void setNode(QString node);

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::ActionDialog *ui;

};

#endif // ACTIONDIALOG_H
