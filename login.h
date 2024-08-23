#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QList>
#include <QString>
#include "item.h"

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent, QList<Item> *itemList);
    QList<Item>* itemList;
    ~login();

private slots:
    void on_pushButton_clicked();

    void on_lineEditName_returnPressed();

private:
    Ui::login *ui;
};

#endif // LOGIN_H
