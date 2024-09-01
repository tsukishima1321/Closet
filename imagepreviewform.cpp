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

void imagePreviewForm::setImg(QString href, std::shared_ptr<QImage> img, QString des) {
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
    }
    ui->labelImg->show();
    ui->labelText->show();
    available = false;
}

int imagePreviewForm::getHeight() {
    if (img) {
        return this->img->height();
    } else {
        return 0;
    }
}

void imagePreviewForm::hideElements() {
    ui->labelImg->hide();
    ui->labelText->hide();
    available = true;
}

bool imagePreviewForm::isAvailable() const {
    return available;
}

void imagePreviewForm::mouseDoubleClickEvent(QMouseEvent *event) {
    emit isClicked(href);
}

imagePreviewForm::~imagePreviewForm() {
    delete ui;
}
