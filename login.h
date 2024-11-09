#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <optional>

namespace Ui {
    class Login;
}

class Login : public QDialog {
    Q_OBJECT

signals:
    void loginRes(QSqlDatabase &db);

public:
    explicit Login(QWidget *parent);
    ~Login();

private slots:
    void pushButton_clicked();
    void lineEditName_returnPressed();

private:
    Ui::Login *ui;
    void submit(QSqlDatabase &db);
};

#endif // LOGIN_H
