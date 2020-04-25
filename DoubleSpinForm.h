#ifndef DOUBLESPINFORM_H
#define DOUBLESPINFORM_H

#include <QWidget>

namespace Ui {
class DoubleSpinForm;
}

class DoubleSpinForm : public QWidget
{
    Q_OBJECT

public:
    explicit DoubleSpinForm(QWidget *parent = 0);
    ~DoubleSpinForm();

    void setMinValue(double value);
    double getMinValue();

    void setMaxValue(double value);
    double getMaxValue();

private:
    Ui::DoubleSpinForm *ui;
};

#endif // DOUBLESPINFORM_H
