#ifndef LABELCOMMIT_H
#define LABELCOMMIT_H

#include "item.h"
#include <QDebug>
#include <QDir>
#include <QList>
#include <QMap>
#include <QProcess>
#include <QSqlQuery>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlDatabase>

namespace Ui {
    class labelCommit;
}

bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);

class labelCommit : public QWidget {
    Q_OBJECT

public:
    labelCommit(QWidget *parent, QMap<QString, Item>* itemMap, QSqlDatabase &db, QString fromDir);
    void tabClicked(int i);
    bool isRunning() const;

private:
    //QList<Item> *itemList;
    QMap<QString, Item>* itemMap;
    QList<QString> typeList;
    QSqlDatabase &db;
    QString fromDir;
    ~labelCommit();
private slots:
    void pushButtonCommitAll_clicked();
    void pushButtonDelete_clicked();

private:
    //void updateTable1();
    void updateTable();
    Ui::labelCommit *ui;
    QProcess *cmd;
};

#endif // LABELCOMMIT_H
