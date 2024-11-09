#include "dbinstance.h"
#include <QMessageBox>

DBInstance *DBInstance::instance = nullptr;

DBInstance::DBInstance(QString name, QString password) {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("diary");
    db.setUserName(name);
    db.setPassword(password);
    db.setPort(3306);
    if (!db.open()) {
        //qDebug()<<"Database Connect Failed";
        open = false;
        QMessageBox::critical(nullptr, "ERROR", "Database Connection Failed");
    } else {
        open = true;
    }
}

std::optional<DBInstance *> DBInstance::getInstance() {
    if (instance == nullptr) {
        return std::nullopt;
    }
    if (instance->open) {
        return instance;
    }
    return std::nullopt;
}

std::optional<DBInstance *> DBInstance::getInstance(QString name, QString password) {
    if (instance == nullptr) {
        instance = new DBInstance(name, password);
    } else if (instance->db.userName() != name || (instance->db.userName() == name && !instance->open)) {
        instance->open = false;
        instance->db.close();
        instance->db.setUserName(name);
        instance->db.setPassword(password);
        if (!instance->db.open()) {
            //qDebug()<<"Database Connect Failed";
            QMessageBox::critical(nullptr, "ERROR", "Database Connection Failed");
        } else {
            instance->open = true;
        }
    }
    //instance->db.userName()==name && instance->isOpen
    return instance;
}

std::optional<DBInstance *> DBInstance::getInstanceByName(QString name) {
    if (instance == nullptr) {
        return std::nullopt;
    }
    if (instance->db.userName() == name && instance->open) {
        return instance;
    }
    return std::nullopt;
}

bool DBInstance::isOpen() const {
    return open;
}

void DBInstance::close() {
    db.close();
    open = false;
}
