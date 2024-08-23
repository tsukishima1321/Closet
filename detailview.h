#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QMainWindow>
#include <QString>

namespace Ui {
class DetailView;
}

class DetailView : public QMainWindow
{
    Q_OBJECT

public:
    explicit DetailView(QWidget *parent, QSqlDatabase& db, QString base);
    void OpenImg(QString href);
    QString base;
    ~DetailView();

private:
    Ui::DetailView *ui;
    QSqlDatabase& db;
    void commitChange();
    void cancelChange();
};

#endif // DETAILVIEW_H
