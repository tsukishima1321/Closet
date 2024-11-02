#ifndef WELCOME_H
#define WELCOME_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSystemTrayIcon>

namespace Ui {
    class Welcome;
}

class Welcome : public QMainWindow {
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void labelingButton_clicked();
    void searchButton_clicked();
    void textButton_clicked();
    void typeEdit();
    void exportHtm();
    void exportDB();
    void setBaseDir();
    //void lineEditUpdate();
    void logInButton_clicked();
    void logOutButton_clicked();

private:
    Ui::Welcome *ui;
    void buildHtm(QSqlDatabase &db);
    void sqlDump(QSqlDatabase &db);
    QSystemTrayIcon *tray;
};

#endif // WELCOME_H
