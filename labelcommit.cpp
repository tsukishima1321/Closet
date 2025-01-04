#include "labelcommit.h"
#include "ui_labelcommit.h"
#include <QCloseEvent>
#include <QCryptographicHash>
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
    } // end if

    if (!QFile::copy(sourceDir, toDir)) {
        return false;
    }
    return true;
}

LabelCommit::LabelCommit(QWidget *parent, QMap<QString, std::tuple<Item, bool>> *itemMap, QSqlDatabase &db, QString fromDir) :
        QWidget(parent),
        db(db),
        fromDir(fromDir),
        ui(new Ui::LabelCommit), cmd(nullptr) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    connect(ui->pushButtonCommitAll, &QPushButton::clicked, this, &LabelCommit::pushButtonCommitAll_clicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &LabelCommit::pushButtonDelete_clicked);
    // qDebug() << QSqlDatabase::drivers();
    if (!db.isOpen()) {
        // qDebug()<<"Database Connect Failed";
        this->hide();
        delete this;
    } else {
        // qDebug()<<"database connected";
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

void LabelCommit::updateTable() {
    int i = 0;
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(itemMap->count());
    for (const auto &itemTuple : *(this->itemMap)) {
        const Item &item = std::get<0>(itemTuple);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(item.date));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(item.href));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(item.description));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(item.type));
        i++;
    }
}

void LabelCommit::tabClicked(int i) {
    if (i == 2) {
        updateTable();
    }
}

LabelCommit::~LabelCommit() {
    if (cmd) {
        delete cmd;
    }
    delete ui;
}

void LabelCommit::pushButtonCommitAll_clicked() {
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
    QSqlQuery query(db);
    query.prepare("INSERT INTO pictures (date,href,description,type) VALUES (:date,:href,:description,:type)");
    for (const auto &itemTuple : *(this->itemMap)) {
        const Item &item = std::get<0>(itemTuple);
        QString source = fromDir + "/" + item.href;
        QFile file(source);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "错误", "文件打开失败\n" + source);
            continue;
        }
        QByteArray ba = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
        QString md5 = ba.toHex();
        QString target = imgBase + md5 + item.href.right(item.href.size() - item.href.lastIndexOf('.'));
        query.bindValue(":date", item.date);
        query.bindValue(":href", md5 + item.href.right(item.href.size() - item.href.lastIndexOf('.')));
        query.bindValue(":description", item.description);
        query.bindValue(":type", item.type);
        query.exec();
        auto res = copyFileToPath(source, target, false);
        if (!res) {
            QMessageBox::warning(this, "错误", "文件复制失败，可能图片已存在\n" + source + "\n" + target);
        }
        if(std::get<1>(itemTuple)) {
            paras.append(target);
        }
        done++;
        ui->progressBar->setValue(static_cast<int>(100.0 * done / n));
        this->update();
    }
    cmd->start("python", paras);
    cmd->waitForStarted();
    itemMap->clear();
    ui->progressBar->setValue(100);
    ui->tableWidget->clearContents();
}

void LabelCommit::pushButtonDelete_clicked() {
    if (ui->tableWidget->rowCount() == 0) {
        return;
    }
    int i = ui->tableWidget->currentRow();
    itemMap->remove(ui->tableWidget->item(i, 1)->text());
    updateTable();
}

bool LabelCommit::isRunning() const {
    if (cmd) {
        if (cmd->state() == QProcess::Running) {
            return true;
        }
    }
    return false;
}
