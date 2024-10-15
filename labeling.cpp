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

#include "dbinstance.h"
#include "labelcommit.h"
#include "labeling.h"
#include "login.h"
#include "typeeditmenu.h"
#include "ui_labeling.h"

LabelWindow::LabelWindow(QWidget *parent) :
        Window(parent),
        ui(new Ui::LabelWindow),
        isLogedIn(false) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    itemList.clear();
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tabWidget->setTabText(0, "录入记录");
    ui->tabWidget->setTabText(1, "记录预览");
    ui->tabWidget->setTabText(2, "记录提交");
    ui->zoomInButton->setIcon(QIcon(":/pic/zoomIn.png"));
    ui->zoomOutButton->setIcon(QIcon(":/pic/zoomOut.png"));
    ui->zoomResetButton->setIcon(QIcon(":/pic/reset.png"));
    ui->freshTypeButton->setIcon(QIcon(":/pic/refresh.png"));
    ui->freshDateButton->setIcon(QIcon(":/pic/refresh.png"));
    ui->toolButtonNext->setIcon(QIcon(":/pic/step-forward.png"));
    ui->toolButtonLast->setIcon(QIcon(":/pic/step-back.png"));
    ui->dateEdit->setDate(QDate::currentDate());
    //链接来自gui的信号
    connect(ui->zoomInButton, &QPushButton::clicked, ui->imageViewPort, &imageView::slot_zoomIn);
    connect(ui->zoomOutButton, &QPushButton::clicked, ui->imageViewPort, &imageView::slot_zoomOut);
    connect(ui->zoomResetButton, &QPushButton::clicked, ui->imageViewPort, &imageView::slot_reset);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &LabelWindow::pushButtonAdd_clicked);
    connect(ui->toolButtonNext, &QPushButton::clicked, this, &::LabelWindow::pushButtonNext_clicked);
    connect(ui->toolButtonLast, &QPushButton::clicked, this, &::LabelWindow::pushButtonLast_clicked);
    connect(ui->lineEditPath, &QLineEdit::returnPressed, this, &LabelWindow::pushButtonStart_clicked);
    connect(ui->pushButtonFinish, &QPushButton::clicked, this, &LabelWindow::pushButtonFinish_clicked);
    connect(ui->pushButtonFinish_2, &QPushButton::clicked, this, &LabelWindow::pushButtonFinish_clicked);
    connect(ui->pushButtonAddType, &QPushButton::clicked, this, &LabelWindow::pushButtonAddType_clicked);
    connect(ui->freshTypeButton, &QPushButton::clicked, this, &LabelWindow::freshButton_clicked);
    connect(ui->freshDateButton, &QPushButton::clicked, this, &LabelWindow::freshDateButton_clicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &LabelWindow::pushButtonDelete_clicked);
    connect(ui->radioButtonScale, &QRadioButton::clicked, this, [this]() { ui->imageViewPort->setWheelMode(imageView::WheelMode::Scale); });
    connect(ui->radioButtonScroll, &QRadioButton::clicked, this, [this]() { ui->imageViewPort->setWheelMode(imageView::WheelMode::Scroll); });
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, [this](int i) {
        if (i == 2) {
            pushButtonFinish_clicked();
        } else if (i == 1) {
            updateTable();
        }
    });
    dbVisitor = QSqlDatabase::addDatabase("QMYSQL", "typeEdit");
    dbVisitor.setHostName("127.0.0.1");
    dbVisitor.setDatabaseName("diary");
    dbVisitor.setUserName("visitor");
    dbVisitor.setPassword("password");
    dbVisitor.setPort(3306);
    if (!dbVisitor.open()) {
        // qDebug()<<"Database Connect Failed";
        QMessageBox::critical(this, "ERROR", "Database Connection Failed");
        // delete this;
    }
    updateTypes();
    currentImgIndex = 0;
}

void LabelWindow::updateTypes() {
    //更新分类列表
    ui->comboBoxType->clear();
    // qDebug()<<"database connected";
    QString sql = "SELECT * FROM types";
    QSqlQuery query(dbVisitor);
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
    currentImgIndex++;
    if (currentImgIndex < ImageCount) {
        QString ImageName = imagePath + dir[currentImgIndex];
        ui->imageViewPort->loadImage(ImageName);
        ui->labelName->setText(dir[currentImgIndex]);
    } else {
        currentImgIndex = ImageCount - 1;
    }
}

void LabelWindow::pushButtonLast_clicked() {
    currentImgIndex--;
    if (currentImgIndex >= 0) {
        QString ImageName = imagePath + dir[currentImgIndex];
        ui->imageViewPort->loadImage(ImageName);
        ui->labelName->setText(dir[currentImgIndex]);
    } else {
        currentImgIndex = 0;
    }
}

void LabelWindow::pushButtonStart_clicked() {
    imagePath = ui->lineEditPath->text(); //文件夹路径
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
    if (ImageCount > 0) {
        QString ImageName = imagePath + dir[0];
        ui->imageViewPort->loadImage(ImageName);
        ui->imageViewPort->update();
        ui->labelName->setText(dir[0]);
    }
    currentImgIndex = 0;
}

void LabelWindow::updateTable() {
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

void LabelWindow::pushButtonAdd_clicked() {
    QString date = ui->dateEdit->text();
    QString name = ui->labelName->text();
    QString des = ui->plainTextEdit->toPlainText();
    itemList.push_back(Item(date, name, des, ui->comboBoxType->currentText()));
}

void LabelWindow::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Save)) {
        this->pushButtonAdd_clicked();
        this->pushButtonNext_clicked();
    }
    Window::keyPressEvent(event);
}

void LabelWindow::freshButton_clicked() { updateTypes(); }

void LabelWindow::freshDateButton_clicked() {
    ui->dateEdit->setDate(QDate::currentDate());
}

void LabelWindow::pushButtonDelete_clicked() {
    if (ui->tableWidget->rowCount() == 0) {
        return;
    }
    int i = ui->tableWidget->currentRow();
    itemList.remove(i);
    updateTable();
}

void LabelWindow::pushButtonFinish_clicked() {
    if (isLogedIn) {
        ui->tabWidget->setCurrentIndex(2);
        return;
    }
    std::optional<dbInstance *> instance = dbInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new login(this);
        labelCommit *newWindow = nullptr;
        connect(loginWindow, &login::loginRes, this, [&newWindow, this](QSqlDatabase &db) { newWindow = new labelCommit(nullptr, &itemList, db, ui->lineEditPath->text()); });
        auto res = loginWindow->exec();
        if (res == QDialog::Accepted) {
            isLogedIn = true;
            connect(ui->tabWidget, &QTabWidget::tabBarClicked, newWindow, &labelCommit::tabClicked);
            ui->tabWidget->removeTab(2);
            ui->tabWidget->addTab(newWindow, "记录提交");
            ui->tabWidget->setCurrentIndex(2);
        }
    } else {
        labelCommit *newWindow = nullptr;
        isLogedIn = true;
        newWindow = new labelCommit(nullptr, &itemList, (*instance)->db, ui->lineEditPath->text());
        connect(ui->tabWidget, &QTabWidget::tabBarClicked, newWindow, &labelCommit::tabClicked);
        ui->tabWidget->removeTab(2);
        ui->tabWidget->addTab(newWindow, "记录提交");
        ui->tabWidget->setCurrentIndex(2);
    }
}

void LabelWindow::pushButtonAddType_clicked() {
    std::optional<dbInstance *> instance = dbInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new login(this);
        typeEditMenu *newWindow = nullptr;
        connect(loginWindow, &login::loginRes, this, [&newWindow](QSqlDatabase &db) { newWindow = new typeEditMenu(nullptr, db); });
        auto res = loginWindow->exec();
        if (res == QDialog::Accepted) {
            newWindow->show();
        }
    } else {
        auto newWindow = new typeEditMenu(nullptr, (*instance)->db);
        newWindow->show();
    }
}
