#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>

class imageView : public QGraphicsView {
    Q_OBJECT
public:
    imageView(QWidget *parent = nullptr);
    void loadImage(QString href);

public slots:
    void slot_zoomIn() { scale(1.2, 1.2); }
    void slot_zoomOut() { scale(1 / 1.2, 1 / 1.2); }
    void slot_reset();

private:
    double zoom;
    QPoint lastMousePos;
    bool mousePressed;
    void wheelEvent(QWheelEvent *event) override;
    void MyScale(double step);
    void MyMove(QPointF delta);
};

#endif // IMAGEVIEW_H
