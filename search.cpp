#include "search.h"
#include "ui_search.h"

Search::Search(QWidget *parent) : QMainWindow(parent),
                                  ui(new Ui::Search) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

Search::~Search() {
    delete ui;
}
