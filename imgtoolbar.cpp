#include "imgtoolbar.h"
#include "ui_imgtoolbar.h"
#include "iconresources.h"

ImgToolBar::ImgToolBar(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ImgToolBar) {
    ui->setupUi(this);
    connect(ui->zoomInButton, &QToolButton::clicked, this, &ImgToolBar::zoomIn);
    connect(ui->zoomOutButton, &QToolButton::clicked, this, &ImgToolBar::zoomOut);
    connect(ui->zoomResetButton, &QToolButton::clicked, this, &ImgToolBar::reset);
    connect(ui->rotateLeftButton, &QToolButton::clicked, this, &ImgToolBar::rotateLeft);
    connect(ui->rotateRightButton, &QToolButton::clicked, this, &ImgToolBar::rotateRight);
    connect(ui->radioButtonScroll, &QRadioButton::clicked, this, [this]() { emit setWheelMode(WheelMode::Scroll); });
    connect(ui->radioButtonScale, &QRadioButton::clicked, this, [this]() { emit setWheelMode(WheelMode::Scale); });
    ui->zoomInButton->setIcon(IconResources::getIcons()["zoom-in"]);
    ui->zoomOutButton->setIcon(IconResources::getIcons()["zoom-out"]);
    ui->zoomResetButton->setIcon(IconResources::getIcons()["refresh"]);
    ui->rotateLeftButton->setIcon(IconResources::getIcons()["rotate-left"]);
    ui->rotateRightButton->setIcon(IconResources::getIcons()["rotate-right"]);
}

ImgToolBar::~ImgToolBar() {
    delete ui;
}