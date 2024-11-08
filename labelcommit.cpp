#include "labelcommit.h"
#include "ui_labelcommit.h"
#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
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

labelCommit::labelCommit(QWidget *parent, QMap<QString, Item>* itemMap, QSqlDatabase &db, QString fromDir) :
        QWidget(parent),
        db(db),
        fromDir(fromDir),
        ui(new Ui::labelCommit), cmd(nullptr) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
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
        this->itemMap = itemMap;
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
    ui->tableWidget->setRowCount(itemMap->count());
    for (const Item &item : *(this->itemMap)) {
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
    if (cmd) {
        delete cmd;
    }
    delete ui;
}

void labelCommit::pushButtonCommitAll_clicked() {
    if (cmd) {
        cmd->close();
        cmd->waitForFinished();
        delete cmd;
    }
    cmd = new QProcess(this);
    connect(cmd, &QProcess::readyReadStandardOutput, this, [this]() { ui->textEdit->append(QString::fromLocal8Bit(cmd->readAllStandardOutput())); });
    connect(cmd, &QProcess::started, this, [this]() { ui->textEdit->setStyleSheet("border: 3px solid red"); });
    connect(cmd, &QProcess::finished, this, [this]() { ui->textEdit->setStyleSheet("border: 3px solid green"); });
    cmd->setReadChannel(QProcess::StandardOutput);

    ui->progressBar->setValue(0);
    int n = itemMap->count();
    int done = 0;
    QStringList paras;
    paras << "doOcr.py"
          << db.userName() << db.password();
    for (const Item &item : *(this->itemMap)) {
        paras.append(fromDir + "/" + item.href);
    }

    cmd->start("python", paras);
    qDebug() << cmd->state();
    cmd->waitForStarted();
    QSqlQuery query(db);
    query.prepare("INSERT INTO pictures (date,href,description,type) VALUES (:date,:href,:description,:type)");
    for (const Item &item : *(this->itemMap)) {
        query.bindValue(":date", item.date);
        query.bindValue(":href", item.href);
        query.bindValue(":description", item.description);
        query.bindValue(":type", item.type);
        query.exec();
        copyFileToPath(fromDir + "/" + item.href, imgBase + item.href, false);
        done++;
        ui->progressBar->setValue(static_cast<int>(100.0 * done / n));
        this->update();
    }
    itemMap->clear();
    ui->progressBar->setValue(100);
    ui->tableWidget->clearContents();
}

void labelCommit::pushButtonDelete_clicked() {
    if (ui->tableWidget->rowCount() == 0) {
        return;
    }
    int i = ui->tableWidget->currentRow();
    itemMap->remove(ui->tableWidget->item(i, 1)->text());
    updateTable();
}

bool labelCommit::isRunning() const {
    if (cmd) {
        if (cmd->state() == QProcess::Running) {
            return true;
        }
    }
    return false;
}
