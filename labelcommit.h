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
    class LabelCommit;
}

bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);

class LabelCommit : public QWidget {
    Q_OBJECT

public:
    LabelCommit(QWidget *parent, QMap<QString, Item>* itemMap, QSqlDatabase &db, QString fromDir);
    void tabClicked(int i);
    bool isRunning() const;

private:
    //QList<Item> *itemList;
    QMap<QString, Item>* itemMap;
    QList<QString> typeList;
    QSqlDatabase &db;
    QString fromDir;
    ~LabelCommit();
private slots:
    void pushButtonCommitAll_clicked();
    void pushButtonDelete_clicked();

private:
    //void updateTable1();
    void updateTable();
    Ui::LabelCommit *ui;
    QProcess *cmd;
};

#endif // LABELCOMMIT_H
