#include "ImageViewWidget.h"
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <QStyleOption>

ImageViewWidget::ImageViewWidget(QWidget *parent) :
        QWidget(parent) {
    //新建了5个Action
    QAction *actionLoad = new QAction(tr("Load"), this);
    connect(actionLoad, &QAction::triggered, this, &ImageViewWidget::onLoadImage);
    QAction *actionZoomIn = new QAction(tr("Zoom In"), this);
    connect(actionZoomIn, &QAction::triggered, this, &ImageViewWidget::onZoomInImage);
    QAction *actionZoomOut = new QAction(tr("Zoom Out"), this);
    connect(actionZoomOut, &QAction::triggered, this, &ImageViewWidget::onZoomOutImage);
    QAction *actionReset = new QAction(tr("Reset"), this);
    connect(actionReset, &QAction::triggered, this, &ImageViewWidget::onPresetImage);

    //开始添加Action
    addAction(actionLoad);
    addAction(actionZoomIn);
    addAction(actionZoomOut);
    addAction(actionReset);

    //设置右键菜单策略为上下文菜单
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

ImageViewWidget::~ImageViewWidget() {
}

void ImageViewWidget::contextMenuEvent(QContextMenuEvent *event) {
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    QMenu *menu = new QMenu(this);

    QAction *loadImage = new QAction(tr("Load Image"));
    QObject::connect(loadImage, &QAction::triggered, this, &ImageViewWidget::onLoadImage);
    menu->addAction(loadImage);
    menu->addSeparator();

    QAction *zoomInAction = new QAction(tr("Zoom In"));
    QObject::connect(zoomInAction, &QAction::triggered, this, &ImageViewWidget::onZoomInImage);
    menu->addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(tr("Zoom Out"));
    QObject::connect(zoomOutAction, &QAction::triggered, this, &ImageViewWidget::onZoomOutImage);
    menu->addAction(zoomOutAction);

    QAction *presetAction = new QAction(tr("Preset"));
    QObject::connect(presetAction, &QAction::triggered, this, &ImageViewWidget::onPresetImage);
    menu->addAction(presetAction);

    menu->exec(pos);
}

void ImageViewWidget::paintEvent(QPaintEvent *event) {
    // 绘制样式
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (m_Image.isNull())
        return QWidget::paintEvent(event);

    // 根据窗口计算应该显示的图片的大小
    int width = qMin(m_Image.width(), this->width());
    int height = width * 1.0 / (m_Image.width() * 1.0 / m_Image.height());
    height = qMin(height, this->height());
    width = height * 1.0 * (m_Image.width() * 1.0 / m_Image.height());

    // 平移
    painter.translate(this->width() / 2 + m_XPtInterval, this->height() / 2 + m_YPtInterval);

    // 缩放
    painter.scale(m_ZoomValue, m_ZoomValue);

    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    painter.drawImage(picRect, m_Image);
}

void ImageViewWidget::wheelEvent(QWheelEvent *event) {
    int value = event->angleDelta().y();
    if (value > 0)
        onZoomInImage();
    else
        onZoomOutImage();

    this->update();
}

void ImageViewWidget::mousePressEvent(QMouseEvent *event) {
    m_OldPos = event->pos();
    m_Pressed = true;
}

void ImageViewWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!m_Pressed)
        return QWidget::mouseMoveEvent(event);

    this->setCursor(Qt::SizeAllCursor);
    QPoint pos = event->pos();
    int xPtInterval = pos.x() - m_OldPos.x();
    int yPtInterval = pos.y() - m_OldPos.y();

    m_XPtInterval += xPtInterval;
    m_YPtInterval += yPtInterval;

    m_OldPos = pos;
    this->update();
}

void ImageViewWidget::mouseReleaseEvent(QMouseEvent *event) {
    m_Pressed = false;
    this->setCursor(Qt::ArrowCursor);
}

void ImageViewWidget::loadImage(QString name) {
    QFile file(name);
    if (!file.exists())
        return;
    m_Image.load(name);
}

void ImageViewWidget::onLoadImage(void) {
    QString imageFile = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Images (*.png *.xpm *.jpg)"));

    QFile file(imageFile);
    if (!file.exists())
        return;
    m_Image.load(imageFile);
    m_ZoomValue = 1.0;
    m_XPtInterval = 0;
    m_YPtInterval = 0;
    this->update();
}

void ImageViewWidget::onZoomInImage(void) {
    m_ZoomValue += 0.2;
    this->update();
}

void ImageViewWidget::onZoomOutImage(void) {
    m_ZoomValue -= 0.2;
    if (m_ZoomValue <= 0) {
        m_ZoomValue += 0.2;
        return;
    }

    this->update();
}

void ImageViewWidget::onPresetImage(void) {
    m_ZoomValue = 1.0;
    m_XPtInterval = 0;
    m_YPtInterval = 0;
    this->update();
}
