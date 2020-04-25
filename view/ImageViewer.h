#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QImage>

namespace Ui {
class ImageViewer;
}

class ImageViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ImageViewer(const QString &path, QWidget *parent = 0);
    ~ImageViewer();

    void showImage();

    void resizeEvent(QResizeEvent *event);
private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    void initUi();

private:
    Ui::ImageViewer *ui;

    QGraphicsPixmapItem *m_pixmapItem;
    QGraphicsScene *m_graphScene;
    QString m_path;
    QImage m_image;

};

#endif // IMAGEVIEWER_H
