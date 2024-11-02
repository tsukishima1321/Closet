#include "textpreviewform.h"
#include "ui_textpreviewform.h"
#include <QPainter>

textPreviewForm::textPreviewForm(QWidget *parent) : QWidget(parent), ui(new Ui::textPreviewForm) {
    ui->setupUi(this);
    available = true;
    this->setMinimumHeight(200);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    this->hideElements();
    ui->labelText->setWordWrap(true);
    ui->labelText->setAlignment(Qt::AlignTop);
}

textPreviewForm::~textPreviewForm() {
    delete ui;
}

void textPreviewForm::setText(QString text, QString date, int id) {
    this->id = id;
    ui->labelText->setText(text + "\n......");
    ui->labelText->adjustSize();
    ui->labelDate->setText(date);
    ui->labelText->show();
    ui->labelDate->show();
    available = false;
}

void textPreviewForm::hideElements() {
    ui->labelDate->hide();
    ui->labelText->hide();
    ui->labelText->adjustSize();
    available = true;
}

bool textPreviewForm::isAvailable() const {
    return available;
}

int textPreviewForm::getHeight() {
    return ui->labelText->height();
}

void textPreviewForm::mouseDoubleClickEvent(QMouseEvent *event) {
    (void)event;
    emit isClicked(id);
}

void textPreviewForm::paintEvent(QPaintEvent *event) {
    (void)event;
    if(available) {
        return QWidget::paintEvent(event);
    }
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(0, 0, this->width(), this->height());
    return QWidget::paintEvent(event);
}