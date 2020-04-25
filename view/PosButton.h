#ifndef POSBUTTON_H
#define POSBUTTON_H

#include <QWidget>

namespace Ui {
class PosButton;
}

class PosButton : public QWidget
{
    Q_OBJECT

public:
    explicit PosButton(QWidget *parent = 0);
    ~PosButton();

    void setText(const QString &text);
    void setSheetStyle(const QString &style);

    void setChecked(const bool value);
private slots:
    void on_pushButton_clicked();

signals:
    void sig_onBtnClicked(QString text);

private:
    Ui::PosButton *ui;
};

#endif // POSBUTTON_H
