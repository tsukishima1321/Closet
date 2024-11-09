#include "textpreviewform.h"
#include "ui_textpreviewform.h"
#include <QPainter>

TextPreviewForm::TextPreviewForm(QWidget *parent) : QWidget(parent), ui(new Ui::TextPreviewForm) {
    ui->setupUi(this);
    available = true;
    this->setMinimumHeight(100);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    this->hideElements();
    ui->labelText->setWordWrap(true);
    ui->labelText->setAlignment(Qt::AlignTop);
}

TextPreviewForm::~TextPreviewForm() {
    delete ui;
}

void TextPreviewForm::setText(QString text, QString date, int id) {
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

void TextPreviewForm::hideElements() {
    ui->labelDate->hide();
    ui->labelText->hide();
    ui->labelText->adjustSize();
    ui->checkBox->hide();
    ui->line->hide();
    ui->labelOmit->hide();
    available = true;
}

bool TextPreviewForm::isAvailable() const {
    return available;
}

bool TextPreviewForm::isCheck() const {
    return ui->checkBox->isChecked();
}

void TextPreviewForm::check() {
    ui->checkBox->setChecked(true);
}

void TextPreviewForm::uncheck() {
    ui->checkBox->setChecked(false);
}

int TextPreviewForm::getId() const {
    return id;
}

QString TextPreviewForm::getDate() const {
    return ui->labelDate->text();
}

int TextPreviewForm::getHeight() const{
    return ui->labelText->height();
}

void TextPreviewForm::setOmit(bool omit) {
    if(omit) {
        ui->labelOmit->show();
    } else {
        ui->labelOmit->hide();
    }
}

void TextPreviewForm::mouseDoubleClickEvent(QMouseEvent *event) {
    (void)event;
    emit isClicked(id);
}

void TextPreviewForm::paintEvent(QPaintEvent *event) {
    (void)event;
    if(available) {
        return QWidget::paintEvent(event);
    }
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(0, 0, this->width(), this->height());
    return QWidget::paintEvent(event);
}