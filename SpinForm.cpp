#include "SpinForm.h"
#include "ui_SpinForm.h"

SpinForm::SpinForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpinForm)
{
    ui->setupUi(this);
}

SpinForm::~SpinForm()
{
    delete ui;
}
