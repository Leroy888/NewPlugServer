#ifndef AIPORTDIALOG_H
#define AIPORTDIALOG_H

#include <QDialog>

namespace Ui {
class AiPortDialog;
}

class AiPortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AiPortDialog(const QStringList &ports, const QStringList &header, const QString &type, QWidget *parent = 0);
    ~AiPortDialog();

    QStringList getList() const ;

private:
    void initUi(const QStringList &header, const QString &type);

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_btnDelete_clicked();

    void on_btnAdd_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

private:
    Ui::AiPortDialog *ui;

    QStringList m_portList;
    int m_curRow;
    QString m_type;
};

#endif // AIPORTDIALOG_H
