#ifndef LABELCOMMIT_H
#define LABELCOMMIT_H

#include <QWidget>
#include "imageviewwidget.h"
#include "item.h"
#include <QList>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QString>

namespace Ui {
class labelCommit;
}

class labelCommit : public QWidget
{
    Q_OBJECT

public:
    labelCommit(QWidget *parent,QList<Item>* itemList,QString name,QString password);
private:
    static labelCommit* _instance;
    ImageViewWidget* imageView;
    QList<Item>* itemList;
    QList<QString> typeList;
    QSqlDatabase& db;
    ~labelCommit();
private slots:
    void pushButtonSearch_clicked();
    void pushButtonCommitAll_clicked();
    void pushButtonCommit_clicked();
    void pushButtonDelete_clicked();
    void pushButtonBuild_clicked();
    void pushButtonSendSQL_clicked();
    void table1CellDoubleClicked(int,int);
    void table2CellDoubleClicked(int,int);
private:
    void updateTable1();
    void updateTable2();
    Ui::labelCommit *ui;
};

#endif // LABELCOMMIT_H
