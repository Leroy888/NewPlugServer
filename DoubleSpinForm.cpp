#include "DoubleSpinForm.h"
#include "ui_DoubleSpinForm.h"

DoubleSpinForm::DoubleSpinForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoubleSpinForm)
{
    ui->setupUi(this);
}

DoubleSpinForm::~DoubleSpinForm()
{
    delete ui;
}

void DoubleSpinForm::setMinValue(double value)
{
    ui->doubleSpinBox_min->setValue(value);
}

double DoubleSpinForm::getMinValue()
{
    return ui->doubleSpinBox_min->value();
}

void DoubleSpinForm::setMaxValue(double value)
{
    ui->doubleSpinBox_max->setValue(value);
}

double DoubleSpinForm::getMaxValue()
{
    return ui->doubleSpinBox_max->value();
}
