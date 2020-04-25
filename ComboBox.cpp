#include "ComboBox.h"
#include "ui_ComboBox.h"

#include <QDebug>

ComboBox::ComboBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComboBox)
{
    ui->setupUi(this);
    m_text = ui->cmbBox->currentText();
}

ComboBox::~ComboBox()
{
    delete ui;
}

void ComboBox::setEditable(const bool value)
{
    ui->cmbBox->setEditable(value);
}

void ComboBox::setCurrentText(QString text)
{
    ui->cmbBox->setCurrentText(text);
}

QString ComboBox::currentText()
{
    return m_text;
}

void ComboBox::clear()
{
    ui->cmbBox->clear();
}

void ComboBox::addItems(const QStringList &items)
{
    ui->cmbBox->addItems(items);
}

int ComboBox::currentIndex() const
{
    return ui->cmbBox->currentIndex();
}

void ComboBox::on_cmbBox_currentTextChanged(const QString &arg1)
{
    m_text = arg1;
}
