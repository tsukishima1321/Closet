#ifndef WELCOME_H
#define WELCOME_H

#include "qsqldatabase.h"
#include <QMainWindow>

namespace Ui {
    class Welcome;
}

class Welcome : public QMainWindow {
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

private slots:
    void labelingButton_clicked();
    void searchButton_clicked();
    void typeEditButton_clicked();
    void buildHtmButton_clicked();
    void lineEditUpdate();
    void logInButton_clicked();
    void logOutButton_clicked();

private:
    Ui::Welcome *ui;
    void buildHtm(QSqlDatabase &db);
};

#endif // WELCOME_H
