#include "TabSpinBox.h"
#include "ui_TabSpinBox.h"

TabSpinBox::TabSpinBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabSpinBox)
{
    ui->setupUi(this);
}

TabSpinBox::~TabSpinBox()
{
    delete ui;
}

void TabSpinBox::setValue(int value)
{
    ui->spinBox_tab->setValue(value);
}
