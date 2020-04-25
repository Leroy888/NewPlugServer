#ifndef NGDIALOG_H
#define NGDIALOG_H

#include <QDialog>
#include <view/DefButton.h>
#include <view/PosButton.h>

namespace Ui {
class NgDialog;
}

class NgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NgDialog(const bool isTurn, const int horPic, const int verPic, const QStringList &defList, QWidget *parent = 0);
    ~NgDialog();

    QStringList getPosList() const;
    QStringList getDefList() const;

    bool getIsOk() const;
private:
    void initUi();

private slots:
    void on_btnReset_clicked();

    void on_btnCancel_clicked();

    void on_btnOk_clicked();

    void slot_onDefBtnClicked(bool value, int index, QString text);
    void slot_onPosBtnClicked(QString text);

private:
    Ui::NgDialog *ui;

    int m_horPic;
    int m_verPic;
    QStringList m_defList;
    bool m_isChecked;

    QStringList m_posList;
    QStringList m_mesDefList;
    QList<DefButton*> m_defBtnList;
    QList<PosButton*> m_posBtnList;
    int m_defIndex;
    int m_posIndex;
    bool m_isTurn;
    bool m_isOk;
};

#endif // NGDIALOG_H
