#ifndef IMGFORM_H
#define IMGFORM_H

#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QPoint>
#include <QTimer>


namespace Ui {
class ImgForm;
}

class ImgForm : public QWidget
{
    Q_OBJECT

public:
    explicit ImgForm(QString info, QString imgPath, int handle, QWidget *parent = 0);
    ~ImgForm();
    void mousePressEvent(QMouseEvent *event);

    void initUi();

signals:
    void sig_clicked(QWidget*, QImage &img, int handle, QString imgPath);
    void sig_clicked(QWidget*, int handle);

private slots:
    void slot_timeout();
private:
    Ui::ImgForm *ui;
    QString m_info;
    int m_handle;
    QString m_imgPath;
    QTimer* m_timer;
    int m_secs;
};

#endif // IMGFORM_H
