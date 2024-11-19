#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include "enum.h"

class ImageView : public QGraphicsView {
    Q_OBJECT

public:
    ImageView(QWidget *parent = nullptr);
    void loadImage(QString href);
    void setWheelMode(WheelMode mode);
    QString getImgHref();
    ~ImageView();

public slots:
    void slot_zoomIn() { scale(1.2, 1.2); }
    void slot_zoomOut() { scale(1 / 1.2, 1 / 1.2); }
    void slot_reset();
    void slot_rotateLeft();
    void slot_rotateRight();

private:
    QString imageHref;
    double zoom;
    QGraphicsScene *_scene;
    QPoint lastMousePos;
    WheelMode wheelMode;
    bool mousePressed;
    void wheelEvent(QWheelEvent *event) override;
    void MyScale(double step);
};

#endif // IMAGEVIEW_H
