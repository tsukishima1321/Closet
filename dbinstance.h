#ifndef DBINSTANCE_H
#define DBINSTANCE_H

#include <QSqlQuery>
#include <QString>
#include <QtSql/QSqlDatabase>

class dbInstance {
public:
    static dbInstance *getInstance(QString name, QString password);
    static dbInstance *getInstanceByName(QString name);
    QSqlDatabase db;
    bool isOpen;

protected:
    static dbInstance *instance;
    explicit dbInstance(QString name, QString password);
    ~dbInstance();
    class dbGarbo {
    public:
        ~dbGarbo() {
            if (dbInstance::instance) {
                delete dbInstance::instance;
            }
        }
    };
    static dbGarbo garbo;
};

#endif // DBINSTANCE_H
