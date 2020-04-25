#include "ImgForm.h"
#include "ui_ImgForm.h"
#include <QModelIndex>
#include <QPoint>

#include <QDebug>


ImgForm::ImgForm(QString info, QString imgPath, int handle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImgForm),
    m_info(info)
{
    ui->setupUi(this);

    m_handle = handle;
    m_imgPath = imgPath;
    m_secs = 20;
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slot_timeout()));
    ui->label->setText(QString::number(m_secs) + "\n" + info);
    m_timer->start(1000);
}

ImgForm::~ImgForm()
{
    disconnect(m_timer,SIGNAL(timeout()),this,SLOT(slot_timeout()));
    delete m_timer;
    delete ui;
}

void ImgForm::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit sig_clicked(this, m_handle);

        QString style = "border-radius: 5px;border:1px solid black;background:#c6ffff; color: rgb(18, 18, 18);font-size: 25px;";
        ui->label->setStyleSheet(style);
    }
}

void ImgForm::initUi()
{
    QString style = "border-radius: 5px;border:1px solid black;background:#AFAFAF; color: rgb(18, 18, 18);font-size: 25px;";
    ui->label->setStyleSheet(style);
}

void ImgForm::slot_timeout()
{
    if(m_secs == 0)
    {
        return;
    }
    else
    {
        --m_secs;
        if(m_secs < 10)
        {
            if(m_secs % 2 == 0)
            {
                this->setStyleSheet(QString::fromUtf8("border:5px solid red"));
            }
            else
            {
                this->setStyleSheet(QString::fromUtf8("border:5px solid green"));
            }
            QString text = QString::number(m_secs) + "\n" + m_info;
            ui->label->setText(text);
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::red);
            ui->label->setPalette(pa);
        }
        else
        {
            ui->label->setText(QString::number(m_secs) + "\n" + m_info);
        }
    }
}
