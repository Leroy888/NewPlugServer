#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(const QStringList &ports, const QString &header, QWidget *parent = 0);
    ~EditDialog();

    QStringList getList() const ;

private:
    virtual void initUi(const QStringList &header) = 0;

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_btnDelete_clicked();

    void on_btnAdd_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

private:
    Ui::EditDialog *ui;

    QStringList m_portList;
    int m_curRow;
};

#endif // EDITDIALOG_H
