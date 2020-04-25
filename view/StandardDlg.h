#ifndef STANDARDDLG_H
#define STANDARDDLG_H

#include <QDialog>

namespace Ui {
class StandardDlg;
}

class StandardDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StandardDlg(const QStringList &horHeaders, const QStringList &verHeader, const QMap<QString,QStringList> &sortMap, QWidget *parent = 0);
    ~StandardDlg();

private:
    void initUi(const QStringList &horHeader, const QStringList &verHeader, const QMap<QString, QStringList> &sortMap);

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::StandardDlg *ui;
};

#endif // STANDARDDLG_H
