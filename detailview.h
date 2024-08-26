#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QString>
#include <QtSql/QSqlDatabase>

namespace Ui {
    class DetailView;
}

class DetailView : public QMainWindow {
    Q_OBJECT

public:
    explicit DetailView(QWidget *parent, QSqlDatabase &db);
    void OpenImg(QString href);
    ~DetailView();

private slots:
    void ocrMenuButton_clicked();

private:
    Ui::DetailView *ui;
    QSqlDatabase &db;
    QString current;
    void commitChange();
    void cancelChange();
    void typeMenu();
    void updateTypes();
};

#endif // DETAILVIEW_H
