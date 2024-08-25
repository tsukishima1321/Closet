#ifndef DBINSTANCE_H
#define DBINSTANCE_H

#include <QSqlQuery>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <optional>

class dbInstance {
public:
    static std::optional<dbInstance *> getInstance(QString name, QString password);
    static std::optional<dbInstance *> getInstanceByName(QString name);
    static std::optional<dbInstance *> getInstance();
    QSqlDatabase db;
    void close();
    bool isOpen() const;

protected:
    static dbInstance *instance;
    bool open;
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
