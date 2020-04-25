#ifndef TABSPINBOX_H
#define TABSPINBOX_H

#include <QWidget>

namespace Ui {
class TabSpinBox;
}

class TabSpinBox : public QWidget
{
    Q_OBJECT

public:
    explicit TabSpinBox(QWidget *parent = 0);
    ~TabSpinBox();
    void setValue(int value);

private:
    Ui::TabSpinBox *ui;
};

#endif // TABSPINBOX_H
