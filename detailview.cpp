#include "detailview.h"
#include "ocrmenu.h"
#include "typeeditmenu.h"
#include "ui_detailview.h"
#include <QFileDialog>
#include <QGraphicsItem>
#include <QIcon>
#include <QMessageBox>
#include <QSqlError>

extern QString imgBase;

DetailView::DetailView(QWidget *parent, QSqlDatabase &db) :
        QMainWindow(parent),
        ui(new Ui::DetailView),
        db(db) {
    ui->setupUi(this);
    ui->zoomInButton->setIcon(QIcon(":/pic/zoomIn.png"));
    ui->zoomOutButton->setIcon(QIcon(":/pic/zoomOut.png"));
    ui->zoomResetButton->setIcon(QIcon(":/pic/reset.png"));
    disableEdit();
    connect(ui->zoomInButton, &QPushButton::clicked, ui->graphicsView, &imageView::slot_zoomIn);
    connect(ui->zoomOutButton, &QPushButton::clicked, ui->graphicsView, &imageView::slot_zoomOut);
    connect(ui->zoomResetButton, &QPushButton::clicked, ui->graphicsView, &imageView::slot_reset);
    connect(ui->commitButton, &QPushButton::clicked, this, &DetailView::commitChange);
    connect(ui->cancelButton, &QPushButton::clicked, this, &DetailView::cancelChange);
    connect(ui->typeMenuButton, &QPushButton::clicked, this, &DetailView::typeMenuOpen);
    connect(ui->ocrMenuButton, &QPushButton::clicked, this, &DetailView::ocrMenuOpen);
    connect(ui->editEnableButton, &QPushButton::clicked, this, &DetailView::enableEdit);
    connect(ui->picSaveButton, &QPushButton::clicked, this, &DetailView::savePic);
    connect(ui->radioButtonScale, &QRadioButton::clicked, this, [this]() { ui->graphicsView->setWheelMode(imageView::WheelMode::Scale); });
    connect(ui->radioButtonScroll, &QRadioButton::clicked, this, [this]() { ui->graphicsView->setWheelMode(imageView::WheelMode::Scroll); });
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    updateTypes();
}

void DetailView::OpenImg(QString href) {
    current = href;
    ui->graphicsView->loadImage(imgBase + href);
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
        ui->dateEdit->setDate(QDate::fromString(query.value("date").toString(), "yyyy-MM-dd"));
        ui->desText->setText(query.value("description").toString());
        ui->typeSelect->setCurrentText(query.value("type").toString());
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
}

void DetailView::cancelChange() {
    QSqlQuery query(db);
    QString sql = "select * from pictures where href=:href;";
    query.prepare(sql);
    query.bindValue(":href", current);
    query.exec();
    if (query.next()) {
        ui->dateEdit->setDate(QDate::fromString(query.value("date").toString(), "yyyy-MM-dd"));
        ui->desText->setText(query.value("description").toString());
        ui->typeSelect->setCurrentText(query.value("type").toString());
    }
    disableEdit();
}

void DetailView::typeMenuOpen() {
    auto newWindow = new typeEditMenu(nullptr, db);
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
    QString targetDirPath = QFileDialog::getExistingDirectory(this, "选择保存的路径", "/");
    targetDirPath += "/";
    if (QFile::copy(imgBase + current, targetDirPath + current)) {
        QMessageBox::information(this, "导出", targetDirPath + current + "导出完成");
    } else {
        QMessageBox::warning(this, "导出", targetDirPath + current + "导出失败：文件已存在");
    }
}

DetailView::~DetailView() {
    delete ui;
}

void DetailView::ocrMenuOpen() {
    auto newWindow = new ocrMenu(nullptr, db, current);
    newWindow->show();
}
