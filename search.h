#ifndef SEARCH_H
#define SEARCH_H

#include "flowlayout.h"
#include <QMainWindow>
#include <QSqlDatabase>

constexpr int pageSize = 20;
constexpr int pageSizeTable = 100;

namespace Ui {
    class Search;
}

class Search : public QMainWindow {
    Q_OBJECT

public:
    explicit Search(QWidget *parent, QSqlDatabase &db);
    ~Search();

protected:
    //virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::Search *ui;
    int currentPage;
    FlowLayout *flowLayout;
    QSqlDatabase &db;
    QString currentConditon;
    QStringList hrefList;
    void searchButton_clicked();
    void updateSearch();
    void addImgItem(QString href, QString des);
    void openDetailMenu(QString href);
    void tableCellDoubleClicked(int, int);
    void deleteButton_clicked();
    void sendSQL();
};

#endif // SEARCH_H
