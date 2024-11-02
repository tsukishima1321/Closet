#include "imagepreviewform.h"
#include "ui_imagepreviewform.h"
#include "item.h"
#include <QPainter>

extern QString imgBase;

imagePreviewForm::imagePreviewForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::imagePreviewForm),
        img(nullptr),
        available(true) {
    ui->setupUi(this);
    this->setMinimumHeight(300);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    this->hideElements();
}

void imagePreviewForm::setImg(QSqlRecord record, QImage *img, QModelIndex index) {
    this->index = index;
    this->item = Item(record);
    if (this->img) {
        delete this->img;
    }
    this->img = img;
    if (img->isNull()) {
        ui->labelImg->setPixmap(QPixmap(imgBase + "href"));
    } else {
        ui->labelText->setText(item.description);
        QPixmap pixmap = QPixmap::fromImage(*img);
        pixmap = pixmap.scaled(ui->labelImg->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        //ui->labelImg->setScaledContents(true);
        ui->labelImg->setPixmap(pixmap);
        ui->labelImg->adjustSize();
    }
    ui->labelImg->show();
    ui->labelText->show();
    available = false;
}

int imagePreviewForm::getHeight() const {
    if (img) {
        if (!img->isNull()) {
            return this->img->height();
        }
    }
    return 0;
}

void imagePreviewForm::hideElements() {
    ui->labelImg->clear();
    ui->labelImg->adjustSize();
    ui->labelImg->hide();
    ui->labelText->hide();
    available = true;
}

bool imagePreviewForm::isAvailable() const {
    return available;
}

void imagePreviewForm::mouseDoubleClickEvent(QMouseEvent *event) {
    (void)event;
    emit isClicked(item.href, index.row());
}

/*void imagePreviewForm::paintEvent(QPaintEvent *event) {
    (void)event;
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(0, 0, this->width(), this->height());
    QWidget::paintEvent(event);
}*/

imagePreviewForm::~imagePreviewForm() {
    if (img) {
        delete img;
    }
    delete ui;
}
