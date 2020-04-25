#ifndef POINTFORM_H
#define POINTFORM_H

#include <QWidget>

namespace Ui {
class PointForm;
}

class PointForm : public QWidget
{
    Q_OBJECT

public:
    explicit PointForm(QWidget *parent = 0);
    ~PointForm();

    void setPoint1(QString p);
    QString getPoint1();

    void setPoint2(QString p);
    QString getPoint2();

private:
    Ui::PointForm *ui;
};

#endif // POINTFORM_H
