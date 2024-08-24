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

private:
    Ui::DetailView *ui;
    QSqlDatabase &db;
    void commitChange();
    void cancelChange();
    void typeMenu();
    void updateTypes();
};

#endif // DETAILVIEW_H
