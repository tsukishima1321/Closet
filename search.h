#ifndef SEARCH_H
#define SEARCH_H

#include "itemmodel.h"
#include "window.h"
#include <QLayout>
#include <QList>
#include <QMainWindow>
#include <QRunnable>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <imagepreviewform.h>
#include <span>

namespace imgViewConstants {
    constexpr int pageSize = 20;
}
// constexpr int pageSizeTable = 100;

namespace Ui {
    class Search;
}

class Search : public Window {
    Q_OBJECT

public:
    explicit Search(QWidget *parent, QSqlDatabase &db);
    ~Search();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::Search *ui;
    int currentPage;
    int currentColumnCount;
    int pageSizeTable;
    QHBoxLayout *hBoxLayout;
    QList<QVBoxLayout *> vBoxLayouts;

    QSqlDatabase &db;
    itemModel queryModel;
    itemModel countModel;
    imagePreviewForm *preViewList;
    std::span<imagePreviewForm> preViewListSpan;
    void searchButton_clicked();
    void updateSearch();
    imagePreviewForm *addImgItem(QSqlRecord record, QModelIndex index);
    void openDetailMenu(QString href, int row);
    void updateImgView();
    void updateTableView();
    void locateImg();
    void tableCellDoubleClicked(const QModelIndex &index);
    void deleteButton_clicked();
    void exportButton_clicked();
    void sendSQL(QString text);
};

class imgLoader : public QObject, public QRunnable {
    Q_OBJECT
public:
    imgLoader(imagePreviewForm *target, QImageReader *reader, QSqlRecord record, QModelIndex index);
    void run() override;

private:
    imagePreviewForm *target;
    QImageReader *reader;
    QSqlRecord record;
    QModelIndex index;

signals:
    void loadReady();
};

#endif // SEARCH_H
