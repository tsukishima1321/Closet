#include "ocrmenu.h"
#include "qsqlerror.h"
#include "ui_ocrmenu.h"
#include "item.h"
#include <QMessageBox>
#include <QSqlQuery>

OCRMenu::OCRMenu(QWidget *parent, QSqlDatabase &db, QString key) :
        QWidget(parent),
        ui(new Ui::OCRMenu),
        db(db),
        key(key) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    QSqlQuery query(db);
    QString sql = "select * from pictures_ocr where href=:href;";
    query.prepare(sql);
    query.bindValue(":href", key);
    query.exec();
    if (query.next()) {
        ui->textEdit->setText(Item(query.record()).ocr);
    }
    connect(ui->commitButton, &QPushButton::clicked, this, &OCRMenu::editCommit);
    connect(ui->cancelButton, &QPushButton::clicked, this, &OCRMenu::editCancel);
}

void OCRMenu::editCommit() {
    QSqlQuery query(db);
    QString sql = "select * from pictures_ocr where href=:href;";
    query.prepare(sql);
    query.bindValue(":href", key);
    query.exec();
    if (query.next()) {
        sql = "update pictures_ocr set ocr_result=:text where href=:href";
    } else {
        sql = "insert into pictures_ocr values(:href,:text);";
    }
    query.prepare(sql);
    query.bindValue(":href", key);
    query.bindValue(":text", ui->textEdit->toPlainText());
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
}

void OCRMenu::editCancel() {
    QSqlQuery query(db);
    QString sql = "select * from pictures_ocr where href=:href;";
    query.prepare(sql);
    query.bindValue(":href", key);
    query.exec();
    if (query.next()) {
        ui->textEdit->setText(Item(query.record()).ocr);
    }
}

OCRMenu::~OCRMenu() {
    delete ui;
}
