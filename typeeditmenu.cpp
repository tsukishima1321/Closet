#include "typeeditmenu.h"
#include "ui_typeeditmenu.h"
#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

typeEditMenu::typeEditMenu(QWidget *parent, QSqlDatabase& db) :
    QWidget(parent),
    ui(new Ui::typeEditMenu),
    db(db)
{
    ui->setupUi(this);
    ui->pushButtonDelete->hide();
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(ui->pushButtonAdd,&QPushButton::clicked,this,&typeEditMenu::addType);
    connect(ui->pushButtonDelete,&QPushButton::clicked,this,&typeEditMenu::deleteType);
    connect(ui->listWidget,&QListWidget::itemClicked,this,&typeEditMenu::listWidget_itemClicked);
    updateTypes();
}

void typeEditMenu::addType()
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

void typeEditMenu::deleteType()
{
        QSqlQuery query(db);
        query.prepare("DELETE FROM types WHERE typename=:type");
        query.bindValue(":type",itemSelected);
        if(!query.exec()){
            QSqlError sqlerror= query.lastError();
            qDebug()<<sqlerror.nativeErrorCode();
            QString errortext=sqlerror.text();
            qDebug()<<errortext;
            QMessageBox::critical(this, "错误", errortext);
        }else{
            updateTypes();
            ui->pushButtonDelete->hide();
        }
}


void typeEditMenu::updateTypes()
{
    emit typeUpdateEvent();
    ui->listWidget->clear();
    QString sql = "SELECT * FROM types";
    QSqlQuery query(db);
    query.prepare(sql);
    query.exec();
    while (query.next()) {
        QString type=query.value("typename").toString();
        ui->listWidget->addItem(type);
    }
}

typeEditMenu::~typeEditMenu()
{
    delete ui;
}

void typeEditMenu::listWidget_itemClicked(QListWidgetItem *item)
{
    itemSelected = item->text();
    ui->pushButtonDelete->show();
}

