#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui {
    class TextView;
}

class TextView : public QMainWindow {
    Q_OBJECT
public:
    explicit TextView(QWidget *parent, QSqlDatabase &db);
    ~TextView();

private:
    Ui::TextView *ui;
    QSqlDatabase &db;
};

#endif // TEXTVIEW_H