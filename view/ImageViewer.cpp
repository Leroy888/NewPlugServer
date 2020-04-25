#include "ImageViewer.h"
#include "ui_ImageViewer.h"
#include <QImage>

#include <QDebug>

ImageViewer::ImageViewer(const QString &path, QWidget *parent) :
    QDialog(parent), m_path(path),
    ui(new Ui::ImageViewer)
{
    ui->setupUi(this);
    this->setWindowTitle(QStringLiteral("图片预览"));
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinMaxButtonsHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    initUi();
}

ImageViewer::~ImageViewer()
{
    qDebug()<<"fuck viewer";
    delete m_pixmapItem;
    delete m_graphScene;
    delete ui;
}

void ImageViewer::showImage()
{
    //QImage image(m_path);
    m_image.load(m_path);
    QPixmap pixMap(QPixmap::fromImage(m_image));

    if(m_pixmapItem)
    {
        m_graphScene->removeItem(m_pixmapItem);
        delete m_pixmapItem;
        m_pixmapItem = NULL;

    }
    m_graphScene->clear();
    ui->graphicsView->resetTransform();

    m_graphScene->setSceneRect(m_image.rect());
    m_pixmapItem = m_graphScene->addPixmap(pixMap);

    float fScale1 = (ui->graphicsView->width())*1.0 / m_image.width();
    float fScale2 = (ui->graphicsView->height())*1.0 / m_image.height();
    qDebug()<<"scal1:"<<fScale1<<" scal2: "<<fScale2;
    float m_fScale = fScale1;
    if(fScale1 > fScale2)
        m_fScale = fScale2;
    ui->graphicsView->scale(m_fScale, m_fScale);
}

void ImageViewer::on_btnOk_clicked()
{
    this->accept();
}

void ImageViewer::on_btnCancel_clicked()
{
    this->reject();
}

void ImageViewer::initUi()
{
    m_pixmapItem = NULL;
    m_graphScene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(m_graphScene);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setBackgroundBrush(QBrush(QColor(10,10,10)));

   // showImage();
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
    if(m_image.isNull())
        return;
    showImage();
}
