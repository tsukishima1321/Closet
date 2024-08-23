#ifndef SEARCH_H
#define SEARCH_H

#include <QMainWindow>

namespace Ui {
class Search;
}

class Search : public QMainWindow
{
    Q_OBJECT

public:
    explicit Search(QWidget *parent = nullptr);
    ~Search();

private:
    Ui::Search *ui;
};

#endif // SEARCH_H
