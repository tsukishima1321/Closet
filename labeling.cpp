#include <QImage>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QInputDialog>
#include "login.h"
#include "labeling.h"
#include "imageViewWidget.h"
#include "ui_labeling.h"

LabelWindow::LabelWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LabelWindow)
{
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    itemList.clear();
    ui->setupUi(this);
    //链接来自gui的信号
    connect(ui->pushButtonAdd,&QPushButton::clicked,this,&LabelWindow::pushButtonAdd_clicked);
    connect(ui->pushButtonNext,&QPushButton::clicked,this,&LabelWindow::pushButtonNext_clicked);
    connect(ui->pushButtonLast,&QPushButton::clicked,this,&LabelWindow::pushButtonLast_clicked);
    connect(ui->pushButtonStart,&QPushButton::clicked,this,&LabelWindow::pushButtonStart_clicked);
    connect(ui->pushButtonFinish,&QPushButton::clicked,this,&LabelWindow::pushButtonFinish_clicked);
    connect(ui->pushButtonAddType,&QPushButton::clicked,this,&LabelWindow::pushButtonAddType_clicked);
    updateTypes();
}

void LabelWindow::updateTypes(){
    //更新分类列表
    ui->comboBoxType->clear();
    db = QSqlDatabase::addDatabase("QMYSQL","typeEdit");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("diary");
    db.setUserName("visitor");
    db.setPassword("password");
    db.setPort(3306);
    if(!db.open()){
        //qDebug()<<"Database Connect Failed";
        QMessageBox::critical(this, "ERROR", "Database Connection Failed");
        delete this;
    }else{
        qDebug()<<"database connected";
        QString sql = "SELECT * FROM types";
        QSqlQuery query(db);
        query.prepare(sql);
        query.exec();
        while (query.next()) {
            QString type=query.value("typename").toString();
            ui->comboBoxType->addItem(type);
            qDebug()<<type;
        }
    }
}

LabelWindow::~LabelWindow()
{
    delete ui;
}

void LabelWindow::pushButtonNext_clicked(){//NEXT
    flag++;
    if (flag<ImageCount){
        QString ImageName  = imagePath+dir[flag];
        ui->imageView->loadImage(ImageName);
        ui->imageView->update();
        ui->labelName->setText(dir[flag]);
    }else{
        flag=ImageCount-1;
    }
}

void LabelWindow::pushButtonLast_clicked()//LAST
{
    flag--;
    if (flag>=0){
        QString ImageName  = imagePath+dir[flag];
        ui->imageView->loadImage(ImageName);
        ui->imageView->update();
        ui->labelName->setText(dir[flag]);
    }else{
        flag=0;
    }
}

void LabelWindow::pushButtonStart_clicked()
{
    imagePath = ui->lineEdit_1->text();//文件夹路径
    if(imagePath.back()!='/'){
        imagePath+='/';
    }
    dir.setPath(imagePath);
    QStringList ImageList;
    ImageList << "*.bmp" << "*.jpg" << "*.png";//向字符串列表添加图片类型
    dir.setNameFilters(ImageList);//获得文件夹下图片的名字
    ImageCount = dir.count();//获得dir里名字的个数，也表示文件夹下图片的个数
    QString ImageName = imagePath+dir[0];
    ui->imageView->loadImage(ImageName);
    ui->imageView->update();
    ui->labelName->setText(dir[0]);
    flag=0;
}

void LabelWindow::pushButtonAdd_clicked()//ADD
{
    QString date=ui->lineEditDate->text();
    QString name=ui->labelName->text();
    QString des=ui->lineEditDes->text();
    QString out="		<A href=\""+name+"\">"+des+"</A>&nbsp;——"+date+"<BR>"+"\n";
    QString output="";
    ui->plainTextEdit->setPlainText(output);
    itemList.push_back(Item(date,name,des,ui->comboBoxType->currentText()));
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(itemList.length());
    int i=0;
    for(const Item& item : itemList){
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(item.date));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(item.href));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(item.description));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(item.type));
        i++;
    }
}

void LabelWindow::pushButtonFinish_clicked()//FINISH
{
    QFile file(imagePath+"imglist.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text|QIODevice::Append))
        {
            QTextStream in(&file);
            in<<ui->plainTextEdit->toPlainText();
            file.close();
        }
    login* lg=new login(nullptr,&itemList);
    lg->show();
}


void LabelWindow::pushButtonAddType_clicked()
{
    bool qRed = false;
    QString newType = QInputDialog::getText(this, "新建分类","新分类名称：", QLineEdit::Normal, "", &qRed);
    if (qRed && !newType.isEmpty())
    {
        QSqlQuery query(db);
        query.prepare("INSERT INTO types VALUES (null,:type)");
        query.bindValue(":type",newType);
        if(!query.exec()){
            QSqlError sqlerror= query.lastError();
            qDebug()<<sqlerror.nativeErrorCode();
            QString errortext=sqlerror.text();
            qDebug()<<errortext;
            QMessageBox::critical(this, "错误", errortext);
        }else{
            updateTypes();
        }

    }

}
