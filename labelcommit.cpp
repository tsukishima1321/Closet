#include "labelcommit.h"
#include "detailview.h"
#include "ui_labelcommit.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QSqlError>

labelCommit::labelCommit(QWidget *parent, QList<Item> *itemList, QSqlDatabase &db) :
        QWidget(parent),
        db(db),
        ui(new Ui::labelCommit) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    //连接来自gui的信号
    //connect(ui->pushButtonBuild, &QPushButton::clicked, this, &labelCommit::pushButtonBuild_clicked);
    connect(ui->pushButtonCommit, &QPushButton::clicked, this, &labelCommit::pushButtonCommit_clicked);
    connect(ui->pushButtonCommitAll, &QPushButton::clicked, this, &labelCommit::pushButtonCommitAll_clicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &labelCommit::pushButtonDelete_clicked);
    //connect(ui->pushButtonSendSQL, &QPushButton::clicked, this, &labelCommit::pushButtonSendSQL_clicked);
    //connect(ui->pushButtonSearch, &QPushButton::clicked, this, &labelCommit::pushButtonSearch_clicked);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &labelCommit::tableCellDoubleClicked);
    //connect(ui->tableWidget_2, &QTableWidget::cellDoubleClicked, this, &labelCommit::table2CellDoubleClicked);
    qDebug() << QSqlDatabase::drivers();
    if (!db.isOpen()) {
        //qDebug()<<"Database Connect Failed";
        this->hide();
        delete this;
    } else {
        //qDebug()<<"database connected";
        typeList.clear();
        this->itemList = itemList;
        updateTable();
        this->show();
        QString sql = "SELECT * FROM types";
        QSqlQuery query(db);
        query.prepare(sql);
        query.exec();
        while (query.next()) {
            QString type = query.value("typename").toString();
            typeList.push_back(type);
        }
    }
}

/*void labelCommit::updateTable1() {
    QString date = ui->dateEdit->text();
    QString sql = "SELECT * FROM pictures WHERE date='" + date + "'";
    QSqlQuery query(db);
    query.exec(sql);
    QList<Item *> items;
    items.clear();
    while (query.next()) {
        items.push_back(new Item(query.value("date").toString(), query.value("href").toString(), query.value("description").toString(), query.value("type").toString()));
    }
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(items.length());
    int i = 0;
    for (Item *item : items) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(item->date));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(item->href));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(item->description));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(item->type));
        i++;
    }
}*/

void labelCommit::updateTable() {
    int i = 0;
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(itemList->length());
    for (const Item &item : *(this->itemList)) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(item.date));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(item.href));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(item.description));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(item.type));
        i++;
    }
}

void labelCommit::tabClicked(int i) {
    if (i == 2) {
        updateTable();
    }
}

labelCommit::~labelCommit() {
    delete ui;
}

void labelCommit::pushButtonCommitAll_clicked() {
    QSqlQuery query(db);
    query.prepare("INSERT INTO pictures (date,href,description,type) VALUES (:date,:href,:description,:type)");
    for (const Item &item : *(this->itemList)) {
        qDebug() << "23456543";
        query.bindValue(":date", item.date);
        query.bindValue(":href", item.href);
        query.bindValue(":description", item.description);
        query.bindValue(":type", item.type);
        query.exec();
    }
    itemList->clear();
    ui->tableWidget->clearContents();
}

void labelCommit::pushButtonCommit_clicked() {
    if (ui->tableWidget->rowCount() == 0) {
        return;
    }
    int i = ui->tableWidget->currentRow();
    QSqlQuery query(db);
    query.prepare("INSERT INTO pictures (date,href,description,type) VALUES (:date,:href,:description,:type)");
    const Item &item = (*itemList)[i];
    query.bindValue(":date", item.date);
    query.bindValue(":href", item.href);
    query.bindValue(":description", item.description);
    query.bindValue(":type", item.type);
    query.exec();
    itemList->removeAt(i);
    updateTable();
}

void labelCommit::pushButtonDelete_clicked() {
    if (ui->tableWidget->rowCount() == 0) {
        return;
    }
    int i = ui->tableWidget->currentRow();
    itemList->remove(i);
    updateTable();
}

/*void labelCommit::pushButtonSendSQL_clicked() {
    QString sql = ui->sqlEdit->text();
    QString warning = "";
    if (sql.contains("insert")) {
        warning += "insert ";
    }
    if (sql.contains("delete")) {
        warning += "delete ";
    }
    if (warning != "") {
        QMessageBox message(QMessageBox::Warning, "警告", "含有危险操作:" + warning + "\n是否继续？", QMessageBox::Yes | QMessageBox::No, NULL);
        if (message.exec() != QMessageBox::Yes) {
            return;
        }
    }
    if (sql.contains("delete") && !sql.contains("where")) {
        QMessageBox::critical(this, "错误", "操作不被允许");
        return;
    }
    QSqlQuery query(db);
    if (!query.exec(sql)) {
        QSqlError sqlerror = query.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        if (errortext == "") {
            errortext = "empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    }
    QList<Item> items;
    items.clear();
    while (query.next()) {
        items.push_back(Item(query.value("date").toString(), query.value("href").toString(), query.value("description").toString(), query.value("type").toString()));
    }
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(items.length());
    int i = 0;
    for (const Item &item : items) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(item.date));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(item.href));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(item.description));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(item.type));
        i++;
    }
}*/

void labelCommit::tableCellDoubleClicked(int row, int column) {
    (void)column;
    QString name = ui->tableWidget->item(row, 1)->text();
    auto newWindow = new DetailView(nullptr, db);
    newWindow->OpenImg(name);
    newWindow->show();
}
