#include "imageview.h"
#include <QGraphicsItem>
#include <QWheelEvent>

ImageView::ImageView(QWidget *parent) :
        QGraphicsView(parent),
        zoom(1),
        _scene(nullptr),
        mousePressed(false) {
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    wheelMode = WheelMode::Scroll;
}

void ImageView::loadImage(QString href) {
    if (_scene) {
        delete _scene;
    }
    imageHref = href;
    _scene = new QGraphicsScene;
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap(href));
    _scene->addItem(item);
    this->setScene(_scene);
    this->slot_reset();
}

QString ImageView::getImgHref() {
    return imageHref;
}

void ImageView::setWheelMode(WheelMode mode) {
    this->wheelMode = mode;
}

void ImageView::slot_reset() {
    if (this->scene()) {
        double ratio = this->size().width() / this->scene()->items()[0]->boundingRect().width();
        resetTransform();
        scale(ratio, ratio);
        centerOn(0, 0);
    }
}

void ImageView::slot_rotateLeft() {
    rotate(-90);
}

void ImageView::slot_rotateRight() {
    rotate(90);
}

void ImageView::wheelEvent(QWheelEvent *event) {
    if (wheelMode == WheelMode::Scale) {
        this->MyScale(event->angleDelta().y() / 2);
    } else if (wheelMode == WheelMode::Scroll) {
        QGraphicsView::wheelEvent(event);
    }
}

void ImageView::MyScale(double step) {
    double factor = 1.0 + step / 500.0;
    this->zoom *= factor;
    if (this->zoom < 0.01)
        return;
    this->scale(factor, factor);
}

ImageView::~ImageView() {
    delete this->scene();
}
