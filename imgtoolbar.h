#ifndef IMGTOOLBAR_H
#define IMGTOOLBAR_H

#include <QWidget>

namespace Ui {
    class ImgToolBar;
}

class ImgToolBar : public QWidget {
    Q_OBJECT

public:
    ImgToolBar(QWidget *parent = nullptr);
    ~ImgToolBar();

signals:
    void zoomIn();
    void zoomOut();
    void reset();
    void rotateLeft();
    void rotateRight();
    void setScroll();
    void setScale();

private:
    Ui::ImgToolBar *ui;
};

#endif // IMGTOOLBAR_H