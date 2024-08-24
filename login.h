#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlDatabase>

namespace Ui {
    class login;
}

class login : public QDialog {
    Q_OBJECT

signals:
    void loginRes(QSqlDatabase &db);

public:
    explicit login(QWidget *parent);
    ~login();

private slots:
    void pushButton_clicked();
    void lineEditName_returnPressed();

private:
    Ui::login *ui;
    void submit(QSqlDatabase &db);
};

#endif // LOGIN_H
