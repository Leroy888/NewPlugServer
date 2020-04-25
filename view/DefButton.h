#ifndef DEFBUTTON_H
#define DEFBUTTON_H

#include <QWidget>

namespace Ui {
class DefButton;
}

class DefButton : public QWidget
{
    Q_OBJECT

public:
    explicit DefButton(const int index, QWidget *parent = 0);
    ~DefButton();

    void setText(const QString& text);
    void setChecked(bool value);
    bool isChecked();

    QString text() const;

private slots:
    void on_pushButton_clicked();

signals:
    void sig_onBtnClicked(bool, int, QString);

private:
    Ui::DefButton *ui;

    int m_index;
};

#endif // DEFBUTTON_H
