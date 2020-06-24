#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>


struct DB_Param
{
    QString ip;
    QString name;
    int port;
    QString user;
    QString pwd;
};

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    void iniUi();

    void updateUi();

    int getDelaySecs();
    void setDelaySecs(int secs);
//    int getHorPieces();
//    void setHorPieces(int horPieces);

//    int getVerPieces();
//    void setVerPieces(int verPieces);

    int getModel();
    void setModel(const int &model);

    DB_Param getDbParam() const;
    void setDbParam(const DB_Param &dbParam, const DB_Param &dbParam2);

    QString getSavePath() const;
    void setSavePath(const QString &savePath);

//    int getUpPixel() const;
//    void setUpPixel(int upPixel);

//    int getBottomPixel() const;
//    void setBottomPixel(int bottomPixel);

//    int getLeftPixel() const;
//    void setLeftPixel(int leftPixel);

//    int getRightPixel() const;
//    void setRightPixel(int rightPixel);

    QString getOrder() const;
    QStringList getOrderList() const;
    void setOrder(const QStringList &orderList, const QString &order);

    QStringList getDgList() const;
    void setDgList(const QStringList &dgList);

    int getImgModel() const;
    void setImgModel(const int model);

    QString getSavePath2() const;
    void setSavePath2(const QString &savePath2);


    QString getWkshop() const;
    void setWkshop(const QString &wkshop);

    int getNgDlgModel() const;
    void setNgDlgModel(const int model);

    DB_Param getDbParam2() const;
protected:
    void readSettings(QString fileName);
private slots:
    void on_btnOk_clicked();

    void on_btnPath_clicked();

    void on_btnCancel_clicked();

    void on_spinBox_up_valueChanged(int arg1);

    void on_spinBox_bottom_valueChanged(int arg1);

    void on_spinBox_left_valueChanged(int arg1);

    void on_spinBox_right_valueChanged(int arg1);

    void on_btnOrderEdit_clicked();

    void on_btnDgEdit_clicked();

private:
    Ui::SettingsDialog *ui;
    int m_column;
    int m_row;
    QStringList m_defList;
    QStringList m_headerList;
    QList<QComboBox*> m_cmbList;
    QString m_savePath;
    QString m_savePath2;
    QString m_wkshop;
    QMap<int,QStringList> m_stdMap;
    int m_horPieces;
    int m_verPieces;
    int m_upPixel;
    int m_bottomPixel;
    int m_leftPixel;
    int m_rightPixel;
    QStringList m_orderList;
    QStringList m_dgList;
    int m_modelIndex;
    int m_imgModel;
    bool m_bShow;

    //数据库变量
    DB_Param m_dbParam;
    DB_Param m_dbParam2;

};

#endif // SETTINGSDIALOG_H
