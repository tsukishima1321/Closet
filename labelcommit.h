#ifndef LABELCOMMIT_H
#define LABELCOMMIT_H

#include "item.h"
#include <QDebug>
#include <QList>
#include <QSqlQuery>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlDatabase>

namespace Ui {
    class labelCommit;
}

class labelCommit : public QWidget {
    Q_OBJECT

public:
    labelCommit(QWidget *parent, QList<Item> *itemList, QSqlDatabase &db);
    void tabClicked(int i);

private:
    QList<Item> *itemList;
    QList<QString> typeList;
    QSqlDatabase &db;
    ~labelCommit();
private slots:
    void pushButtonCommitAll_clicked();
    void pushButtonCommit_clicked();
    void pushButtonDelete_clicked();
    void tableCellDoubleClicked(int, int);

private:
    //void updateTable1();
    void updateTable();
    Ui::labelCommit *ui;
};

#endif // LABELCOMMIT_H
