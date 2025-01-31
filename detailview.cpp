#include "detailview.h"
#include "iconresources.h"
#include "ocrmenu.h"
#include "typeeditmenu.h"
#include "item.h"
#include "config.h"
#include "ui_detailview.h"
#include <QClipboard>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QSqlError>

DetailView::DetailView(QWidget *parent, QSqlDatabase &db) :
        Window(parent),
        ui(new Ui::DetailView),
        db(db) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(ui->imgToolBar, &ImgToolBar::zoomIn, ui->graphicsView, &ImageView::slot_zoomIn);
    connect(ui->imgToolBar, &ImgToolBar::zoomOut, ui->graphicsView, &ImageView::slot_zoomOut);
    connect(ui->imgToolBar, &ImgToolBar::reset, ui->graphicsView, &ImageView::slot_reset);
    connect(ui->imgToolBar, &ImgToolBar::rotateLeft, ui->graphicsView, &ImageView::slot_rotateLeft);
    connect(ui->imgToolBar, &ImgToolBar::rotateRight, ui->graphicsView, &ImageView::slot_rotateRight);
    connect(ui->imgToolBar, &ImgToolBar::setWheelMode, ui->graphicsView, &ImageView::setWheelMode);
    connect(ui->commitButton, &QPushButton::clicked, this, &DetailView::commitChange);
    connect(ui->cancelButton, &QPushButton::clicked, this, &DetailView::cancelChange);
    connect(ui->typeMenuButton, &QPushButton::clicked, this, &DetailView::typeMenuOpen);
    connect(ui->ocrMenuButton, &QPushButton::clicked, this, &DetailView::ocrMenuOpen);
    connect(ui->editEnableButton, &QPushButton::clicked, this, &DetailView::enableEdit);
    connect(ui->picSaveButton, &QPushButton::clicked, this, &DetailView::savePic);

    QAction *zoomInAction = new QAction("放大", ui->graphicsView);
    QAction *zoomOutAction = new QAction("缩小", ui->graphicsView);
    QAction *zoomResetAction = new QAction("重置", ui->graphicsView);
    QAction *separator = new QAction(ui->graphicsView);
    separator->setSeparator(true);
    QAction *setScrollAction = new QAction("滚动模式", ui->graphicsView);
    QAction *setScaleAction = new QAction("缩放模式", ui->graphicsView);
    QAction *separator_2 = new QAction(ui->graphicsView);
    separator_2->setSeparator(true);
    QAction *savePicAction = new QAction("图片另存为", ui->graphicsView);
    QAction *copyPicAction = new QAction("复制到剪贴板", ui->graphicsView);
    ui->graphicsView->addAction(zoomInAction);
    ui->graphicsView->addAction(zoomOutAction);
    ui->graphicsView->addAction(zoomResetAction);
    ui->graphicsView->addAction(separator);
    ui->graphicsView->addAction(setScrollAction);
    ui->graphicsView->addAction(setScaleAction);
    ui->graphicsView->addAction(separator_2);
    ui->graphicsView->addAction(savePicAction);
    ui->graphicsView->addAction(copyPicAction);
    ui->graphicsView->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(zoomInAction, &QAction::triggered, ui->graphicsView, &ImageView::slot_zoomIn);
    connect(zoomOutAction, &QAction::triggered, ui->graphicsView, &ImageView::slot_zoomOut);
    connect(zoomResetAction, &QAction::triggered, ui->graphicsView, &ImageView::slot_reset);
    connect(setScrollAction, &QAction::triggered, ui->graphicsView, [this]() { ui->graphicsView->setWheelMode(WheelMode::Scroll); });
    connect(setScaleAction, &QAction::triggered, ui->graphicsView, [this]() { ui->graphicsView->setWheelMode(WheelMode::Scale); });
    connect(savePicAction, &QAction::triggered, this, &DetailView::savePic);
    connect(copyPicAction, &QAction::triggered, this, [this]() {
        if (ui->graphicsView->getImgHref() != "") {
            QApplication::clipboard()->setImage(QImage(ui->graphicsView->getImgHref()));
        }
    });

    updateTypes();
    disableEdit();
}

void DetailView::OpenImg(QString href) {
    current = href;
    ui->graphicsView->loadImage(Config::getInstance()->getImgBase() + href);
    QSqlQuery query(db);
    query.prepare("select * from pictures where href=:href");
    query.bindValue(":href", href);
    if (!query.exec()) {
        QSqlError sqlerror = query.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        if (errortext == "") {
            errortext = "empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    } else {
        query.next();
        Item item(query.record());
        ui->dateEdit->setDate(QDate::fromString(item.date, "yyyy-MM-dd"));
        ui->desText->setText(item.description);
        ui->typeSelect->setCurrentText(item.type);
    }
}

void DetailView::enableEdit() {
    ui->dateEdit->setReadOnly(false);
    ui->desText->setReadOnly(false);
    ui->typeSelect->setEnabled(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->typeMenuButton->show();
}

void DetailView::disableEdit() {
    ui->dateEdit->setReadOnly(true);
    ui->desText->setReadOnly(true);
    ui->typeSelect->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(1);
    ui->typeMenuButton->hide();
}

void DetailView::commitChange() {
    QSqlQuery query(db);
    QString sql = "select * from pictures where href=:href;";
    query.prepare(sql);
    query.bindValue(":href", current);
    query.exec();
    if (query.next()) {
        sql = "update pictures set description=:des,date=:date,type=:type where href=:href;";
    } else {
        sql = "insert into pictures values(href=:href,description=:des,date=:date,type=:type);";
    }
    query.prepare(sql);
    query.bindValue(":href", current);
    query.bindValue(":des", ui->desText->toPlainText());
    query.bindValue(":date", ui->dateEdit->text());
    query.bindValue(":type", ui->typeSelect->currentText());
    if (!query.exec()) {
        QSqlError sqlerror = query.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        if (errortext == "") {
            errortext = "empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    } else {
        QMessageBox::information(this, "成功", "修改成功");
    }
    disableEdit();
    emit edit();
}

void DetailView::cancelChange() {
    QSqlQuery query(db);
    QString sql = "select * from pictures where href=:href;";
    query.prepare(sql);
    query.bindValue(":href", current);
    query.exec();
    if (query.next()) {
        Item item(query.record());
        ui->dateEdit->setDate(QDate::fromString(item.date, "yyyy-MM-dd"));
        ui->desText->setText(item.description);
        ui->typeSelect->setCurrentText(item.type);
    }
    disableEdit();
}

void DetailView::typeMenuOpen() {
    auto newWindow = new TypeEditMenu(nullptr, db);
    connect(newWindow, &QWidget::destroyed, this, &DetailView::updateTypes);
    newWindow->show();
}

void DetailView::updateTypes() {
    QString sql = "SELECT * FROM types";
    QSqlQuery query(db);
    query.prepare(sql);
    query.exec();
    while (query.next()) {
        QString type = query.value("typename").toString();
        ui->typeSelect->addItem(type);
    }
}

void DetailView::savePic() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec() == QDialog::Accepted) {
        auto files = dialog.selectedFiles();
        if (files.size() >= 0) {
            QString targetDirPath = files.at(0) + "/";
            if (QFile::copy(Config::getInstance()->getImgBase() + current, targetDirPath + current)) {
                QMessageBox::information(this, "导出", targetDirPath + current + "导出完成");
            } else {
                QMessageBox::warning(this, "导出", targetDirPath + current + "导出失败：文件已存在");
            }
        }
    }
}

DetailView::~DetailView() {
    delete ui;
}

void DetailView::ocrMenuOpen() {
    auto newWindow = new OCRMenu(nullptr, db, current);
    newWindow->show();
}
