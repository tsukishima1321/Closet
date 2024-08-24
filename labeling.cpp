#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QImage>
#include <QInputDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>

#include "imageviewwidget.h"
#include "labelcommit.h"
#include "labeling.h"
#include "login.h"
#include "typeeditmenu.h"
#include "ui_labeling.h"

LabelWindow::LabelWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::LabelWindow), isLogedIn(false) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    itemList.clear();
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "录入记录");
    ui->tabWidget->setTabText(1, "记录预览");
    ui->tabWidget->setTabText(2, "记录提交");
    ui->freshTypeButton->setIcon(QIcon(":/pic/refresh.png"));
    //链接来自gui的信号
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &LabelWindow::pushButtonAdd_clicked);
    connect(ui->pushButtonNext, &QPushButton::clicked, this, &LabelWindow::pushButtonNext_clicked);
    connect(ui->pushButtonLast, &QPushButton::clicked, this, &LabelWindow::pushButtonLast_clicked);
    connect(ui->pushButtonStart, &QPushButton::clicked, this, &LabelWindow::pushButtonStart_clicked);
    connect(ui->pushButtonFinish, &QPushButton::clicked, this, &LabelWindow::pushButtonFinish_clicked);
    connect(ui->pushButtonFinish_2, &QPushButton::clicked, this, &LabelWindow::pushButtonFinish_clicked);
    connect(ui->pushButtonAddType, &QPushButton::clicked, this, &LabelWindow::pushButtonAddType_clicked);
    connect(ui->freshTypeButton, &QPushButton::clicked, this, &LabelWindow::freshButton_clicked);
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, [this](int i) {
        if (i == 2) {
            pushButtonFinish_clicked();
        }
    });
    db = QSqlDatabase::addDatabase("QMYSQL", "typeEdit");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("diary");
    db.setUserName("visitor");
    db.setPassword("password");
    db.setPort(3306);
    if (!db.open()) {
        // qDebug()<<"Database Connect Failed";
        QMessageBox::critical(this, "ERROR", "Database Connection Failed");
        // delete this;
    }
    updateTypes();
}

void LabelWindow::updateTypes() {
    //更新分类列表
    ui->comboBoxType->clear();
    // qDebug()<<"database connected";
    QString sql = "SELECT * FROM types";
    QSqlQuery query(db);
    query.prepare(sql);
    query.exec();
    while (query.next()) {
        QString type = query.value("typename").toString();
        ui->comboBoxType->addItem(type);
        // qDebug()<<type;
    }
}

LabelWindow::~LabelWindow() { delete ui; }

void LabelWindow::pushButtonNext_clicked() {
    flag++;
    if (flag < ImageCount) {
        QString ImageName = imagePath + dir[flag];
        ui->imageView->loadImage(ImageName);
        ui->imageView->update();
        ui->labelName->setText(dir[flag]);
    } else {
        flag = ImageCount - 1;
    }
}

void LabelWindow::pushButtonLast_clicked() {
    flag--;
    if (flag >= 0) {
        QString ImageName = imagePath + dir[flag];
        ui->imageView->loadImage(ImageName);
        ui->imageView->update();
        ui->labelName->setText(dir[flag]);
    } else {
        flag = 0;
    }
}

void LabelWindow::pushButtonStart_clicked() {
    imagePath = ui->lineEdit_1->text(); //文件夹路径
    if (imagePath.back() != '/') {
        imagePath += '/';
    }
    dir.setPath(imagePath);
    QStringList ImageList;
    ImageList << "*.bmp"
              << "*.jpg"
              << "*.png";          //向字符串列表添加图片类型
    dir.setNameFilters(ImageList); //获得文件夹下图片的名字
    ImageCount = dir.count();      //获得dir里名字的个数，也表示文件夹下图片的个数
    QString ImageName = imagePath + dir[0];
    ui->imageView->loadImage(ImageName);
    ui->imageView->update();
    ui->labelName->setText(dir[0]);
    flag = 0;
}

void LabelWindow::pushButtonAdd_clicked() {
    QString date = ui->lineEditDate->text();
    QString name = ui->labelName->text();
    QString des = ui->lineEditDes->text();
    itemList.push_back(Item(date, name, des, ui->comboBoxType->currentText()));
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(itemList.length());
    int i = 0;
    for (const Item &item : itemList) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(item.date));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(item.href));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(item.description));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(item.type));
        i++;
    }
}

void LabelWindow::freshButton_clicked() { updateTypes(); }

void LabelWindow::pushButtonFinish_clicked() {
    if (isLogedIn) {
        ui->tabWidget->setCurrentIndex(2);
        return;
    }
    auto loginWindow = new login(this);
    labelCommit *newWindow = nullptr;
    connect(loginWindow, &login::loginRes, this,
            [&newWindow, this](QSqlDatabase &db) {
                newWindow = new labelCommit(nullptr, &itemList, db);
            });
    auto res = loginWindow->exec();
    if (res == QDialog::Accepted) {
        isLogedIn = true;
        ui->tabWidget->removeTab(2);
        ui->tabWidget->addTab(newWindow, "记录提交");
        ui->tabWidget->setCurrentIndex(2);
    }
}

void LabelWindow::pushButtonAddType_clicked() {
    auto loginWindow = new login(this);
    typeEditMenu *newWindow = nullptr;
    connect(loginWindow, &login::loginRes, this, [&newWindow](QSqlDatabase &db) {
        newWindow = new typeEditMenu(nullptr, db);
    });
    auto res = loginWindow->exec();
    if (res == QDialog::Accepted) {
        newWindow->show();
    }
}
