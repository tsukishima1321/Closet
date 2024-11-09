#include "typeeditmenu.h"
#include "ui_typeeditmenu.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

TypeEditMenu::TypeEditMenu(QWidget *parent, QSqlDatabase &db) :
        QWidget(parent),
        ui(new Ui::TypeEditMenu),
        db(db) {
    ui->setupUi(this);
    ui->pushButtonDelete->hide();
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &TypeEditMenu::addType);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &TypeEditMenu::deleteType);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &TypeEditMenu::listWidget_itemClicked);
    updateTypes();
}

void TypeEditMenu::addType() {
    bool qRed = false;
    QString newType = QInputDialog::getText(this, "新建分类", "新分类名称：", QLineEdit::Normal, "", &qRed);
    if (qRed && !newType.isEmpty()) {
        QSqlQuery query(db);
        query.prepare("INSERT INTO types VALUES (null,:type)");
        query.bindValue(":type", newType);
        if (!query.exec()) {
            QSqlError sqlerror = query.lastError();
            qDebug() << sqlerror.nativeErrorCode();
            QString errortext = sqlerror.text();
            qDebug() << errortext;
            QMessageBox::critical(this, "错误", errortext);
        } else {
            updateTypes();
        }
    }
}

void TypeEditMenu::deleteType() {
    QSqlQuery query(db);
    query.prepare("DELETE FROM types WHERE typename=:type");
    query.bindValue(":type", itemSelected);
    if (!query.exec()) {
        QSqlError sqlerror = query.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        QMessageBox::critical(this, "错误", errortext);
    } else {
        updateTypes();
        ui->pushButtonDelete->hide();
    }
}

void TypeEditMenu::updateTypes() {
    emit typeUpdateEvent();
    ui->listWidget->clear();
    QString sql = "SELECT * FROM types";
    QSqlQuery query(db);
    query.prepare(sql);
    query.exec();
    while (query.next()) {
        QString type = query.value("typename").toString();
        ui->listWidget->addItem(type);
    }
}

TypeEditMenu::~TypeEditMenu() {
    delete ui;
}

void TypeEditMenu::listWidget_itemClicked(QListWidgetItem *item) {
    itemSelected = item->text();
    ui->pushButtonDelete->show();
}
