#include "textpreviewform.h"
#include "ui_textpreviewform.h"
#include <QPainter>

textPreviewForm::textPreviewForm(QWidget *parent) : QWidget(parent), ui(new Ui::textPreviewForm) {
    ui->setupUi(this);
    available = true;
    this->setMinimumHeight(100);
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
    ui->labelText->setText(text);
    ui->labelText->adjustSize();
    ui->labelDate->setText(date);
    ui->labelText->show();
    ui->labelDate->show();
    ui->checkBox->show();
    ui->line->show();
    this->adjustSize();
    available = false;
}

void textPreviewForm::hideElements() {
    ui->labelDate->hide();
    ui->labelText->hide();
    ui->labelText->adjustSize();
    ui->checkBox->hide();
    ui->line->hide();
    ui->labelOmit->hide();
    available = true;
}

bool textPreviewForm::isAvailable() const {
    return available;
}

bool textPreviewForm::isCheck() const {
    return ui->checkBox->isChecked();
}

void textPreviewForm::check() {
    ui->checkBox->setChecked(true);
}

void textPreviewForm::uncheck() {
    ui->checkBox->setChecked(false);
}

int textPreviewForm::getId() const {
    return id;
}

QString textPreviewForm::getDate() const {
    return ui->labelDate->text();
}

int textPreviewForm::getHeight() const{
    return ui->labelText->height();
}

void textPreviewForm::setOmit(bool omit) {
    if(omit) {
        ui->labelOmit->show();
    } else {
        ui->labelOmit->hide();
    }
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