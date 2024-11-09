#ifndef DBINSTANCE_H
#define DBINSTANCE_H

#include <QSqlQuery>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <optional>

class DBInstance {
public:
    static std::optional<DBInstance *> getInstance(QString name, QString password);
    static std::optional<DBInstance *> getInstanceByName(QString name);
    static std::optional<DBInstance *> getInstance();
    QSqlDatabase db;
    void close();
    bool isOpen() const;

protected:
    static DBInstance *instance;
    bool open;
    explicit DBInstance(QString name, QString password);
    ~DBInstance();

    //ensure db connection is properly closed when the program exit
    class dbGarbo {
    public:
        ~dbGarbo() {
            if (DBInstance::instance) {
                delete DBInstance::instance;
            }
        }
    };
    static dbGarbo garbo;
};

#endif // DBINSTANCE_H
