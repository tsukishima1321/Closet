#ifndef SEARCH_H
#define SEARCH_H

#include <QLayout>
#include <QList>
#include <QMainWindow>
#include <QRunnable>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <imagepreviewform.h>

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
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::Search *ui;
    int currentPage;
    int currentColumnCount;
    QHBoxLayout *hBoxLayout;
    QList<QVBoxLayout *> vBoxLayouts;

    QSqlDatabase &db;
    QString currentConditon;
    QStringList hrefList;
    QList<imagePreviewForm *> preViewList;
    void searchButton_clicked();
    void updateSearch();
    imagePreviewForm *addImgItem(QString href, QString des);
    void openDetailMenu(QString href);
    void updateImgView(QSqlQuery &query);
    void updateTableView(QSqlQuery &query);
    void locateImg();
    void tableCellDoubleClicked(int, int);
    void deleteButton_clicked();
    void sendSQL();
};

class imgLoader : public QObject, public QRunnable {
    Q_OBJECT
public:
    imgLoader(imagePreviewForm *target, QImageReader *reader, QString href, QString des);
    imagePreviewForm *target;
    QImageReader *reader;
    QString href;
    QString des;
    void run() override;
signals:
    void loadReady();
};

#endif // SEARCH_H
