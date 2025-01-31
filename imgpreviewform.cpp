#include "imgpreviewform.h"
#include "ui_imgpreviewform.h"
#include "item.h"
#include "config.h"
#include <QPainter>

ImgPreviewForm::ImgPreviewForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ImgPreviewForm),
        img(nullptr),
        available(true) {
    ui->setupUi(this);
    this->setMinimumHeight(300);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    this->hideElements();
    connect(ui->checkBox, &QCheckBox::stateChanged, [this](int state) {
        (void)state;
        emit checked(index.row());
    });
}

void ImgPreviewForm::setImg(QSqlRecord record, QImage *img, QModelIndex index) {
    this->index = index;
    this->item = Item(record);
    if (this->img) {
        delete this->img;
    }
    this->img = img;
    if (img->isNull()) {
        ui->labelImg->setPixmap(QPixmap(Config::getInstance()->getImgBase() + "href"));
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
    ui->checkBox->show();
    available = false;
}

int ImgPreviewForm::getHeight() const {
    if (img) {
        if (!img->isNull()) {
            return this->img->height();
        }
    }
    return 0;
}

void ImgPreviewForm::hideElements() {
    ui->labelImg->clear();
    ui->labelImg->adjustSize();
    ui->labelImg->hide();
    ui->labelText->hide();
    ui->checkBox->hide();
    available = true;
}

bool ImgPreviewForm::isAvailable() const {
    return available;
}

void ImgPreviewForm::mouseDoubleClickEvent(QMouseEvent *event) {
    (void)event;
    emit isClicked(item.href, index.row());
}

void ImgPreviewForm::check() {
    ui->checkBox->setChecked(true);
}

void ImgPreviewForm::uncheck() {
    ui->checkBox->setChecked(false);
}

/*void ImgPreviewForm::paintEvent(QPaintEvent *event) {
    (void)event;
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(0, 0, this->width(), this->height());
    QWidget::paintEvent(event);
}*/

ImgPreviewForm::~ImgPreviewForm() {
    if (img) {
        delete img;
    }
    delete ui;
}
