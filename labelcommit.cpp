#include "labelcommit.h"
#include "ui_labelcommit.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QSqlError>

extern QString imgBase;

bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist) {
    toDir.replace("\\", "/");
    if (sourceDir == toDir) {
        return true;
    }
    if (!QFile::exists(sourceDir)) {
        return false;
    }
    QDir *createfile = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist) {
        if (coverFileIfExist) {
            createfile->remove(toDir);
        }
    } //end if

    if (!QFile::copy(sourceDir, toDir)) {
        return false;
    }
    return true;
}

labelCommit::labelCommit(QWidget *parent, QList<Item> *itemList, QSqlDatabase &db, QString fromDir) :
        QWidget(parent),
        db(db),
        fromDir(fromDir),
        ui(new Ui::labelCommit) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    connect(ui->pushButtonCommit, &QPushButton::clicked, this, &labelCommit::pushButtonCommit_clicked);
    connect(ui->pushButtonCommitAll, &QPushButton::clicked, this, &labelCommit::pushButtonCommitAll_clicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &labelCommit::pushButtonDelete_clicked);
    //qDebug() << QSqlDatabase::drivers();
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
        copyFileToPath(fromDir + "/" + item.href, imgBase + item.href, false);
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
    copyFileToPath(fromDir + item.href, imgBase, false);
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

/**/
