#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QWidget *parent = 0);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint m_lastMousePos;
    bool m_isPresssed;
};

#endif // GRAPHICSVIEW_H
