#include "dbinstance.h"
#include <QMessageBox>

dbInstance* dbInstance::instance = nullptr;

dbInstance::dbInstance(QString name,QString password)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("diary");
    db.setUserName(name);
    db.setPassword(password);
    db.setPort(3306);
    if(!db.open()){
        //qDebug()<<"Database Connect Failed";
        isOpen = false;
        QMessageBox::critical(nullptr, "ERROR", "Database Connection Failed");
    }else{
        isOpen = true;
    }
}

dbInstance* dbInstance::getInstance(QString name,QString password){
    if(instance == nullptr)
    {
        instance = new dbInstance(name,password);
    }
    else if(instance->db.userName()!=name)
    {
        instance->isOpen = false;
        instance->db.close();
        instance->db.setUserName(name);
        instance->db.setPassword(password);
        if(!instance->db.open()){
            //qDebug()<<"Database Connect Failed";
            QMessageBox::critical(nullptr, "ERROR", "Database Connection Failed");
            instance->isOpen = false;
        }else
        {
            instance->isOpen = true;
        }
    }
    return instance;
}
