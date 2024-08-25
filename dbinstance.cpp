#include "dbinstance.h"
#include <QMessageBox>

dbInstance *dbInstance::instance = nullptr;

dbInstance::dbInstance(QString name, QString password) {
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

std::optional<dbInstance *> dbInstance::getInstance() {
    if (instance == nullptr) {
        return std::nullopt;
    }
    if (instance->open) {
        return instance;
    }
    return std::nullopt;
}

std::optional<dbInstance *> dbInstance::getInstance(QString name, QString password) {
    if (instance == nullptr) {
        instance = new dbInstance(name, password);
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

std::optional<dbInstance *> dbInstance::getInstanceByName(QString name) {
    if (instance == nullptr) {
        return std::nullopt;
    }
    if (instance->db.userName() == name && instance->open) {
        return instance;
    }
    return std::nullopt;
}

bool dbInstance::isOpen() const {
    return open;
}

void dbInstance::close() {
    db.close();
    open = false;
}
