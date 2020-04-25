#ifndef IMAGESETDIALOG_H
#define IMAGESETDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class ImageSetDialog;
}

class ImageSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageSetDialog(QMap<QString,QList<double>> &imgParamMap, QMap<QString, QString> &eqpProductMap, const QStringList &orderList, QWidget *parent = 0);
    ~ImageSetDialog();

    QMap<QString, QList<double> > getImageParamMap() const;
    QMap<QString,QString> getEquipProductMap() const;
private:
    void initUi();

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

private:
    Ui::ImageSetDialog *ui;

    int m_curRow;
    QMap<QString,QList<double>> m_imgParamMap;
    QMap<QString,QString> m_equipProductMap;
    QStringList m_orderList;
};

#endif // IMAGESETDIALOG_H
