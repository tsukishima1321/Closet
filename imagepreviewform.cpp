#include "imagepreviewform.h"
#include "ui_imagepreviewform.h"

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

void imagePreviewForm::setImg(QString href, QImage *img, QString des) {
    if (this->img) {
        delete this->img;
    }
    this->href = href;
    this->img = img;
    if (img->isNull()) {
        ui->labelImg->setPixmap(QPixmap(imgBase + "href"));
    } else {
        ui->labelText->setText(des);
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

int imagePreviewForm::getHeight() {
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
    emit isClicked(href);
}

imagePreviewForm::~imagePreviewForm() {
    if (img) {
        delete img;
    }
    delete ui;
}
