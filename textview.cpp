#include "textview.h"
#include "ui_textview.h"

TextView::TextView(QWidget *parent, QSqlDatabase &db) :
    QMainWindow(parent),
    ui(new Ui::TextView),
    db(db)
{
    ui->setupUi(this);
}

TextView::~TextView()
{
    delete ui;
}