#ifndef SPINFORM_H
#define SPINFORM_H

#include <QWidget>

namespace Ui {
class SpinForm;
}

class SpinForm : public QWidget
{
    Q_OBJECT

public:
    explicit SpinForm(QWidget *parent = 0);
    ~SpinForm();

private:
    Ui::SpinForm *ui;
};

#endif // SPINFORM_H
