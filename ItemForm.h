#ifndef ITEMFORM_H
#define ITEMFORM_H

#include <QWidget>

namespace Ui {
class ItemForm;
}

class ItemForm : public QWidget
{
    Q_OBJECT

public:
    explicit ItemForm(QWidget *parent = 0);
    ~ItemForm();

    QString getPoint1();
    void setPoint1(QPoint value);

    QString getPoint2();
    void setPoint2(QPoint value);

private:
    Ui::ItemForm *ui;

    double m_min;
    double m_max;
};

#endif // ITEMFORM_H
