#ifndef IMAGEPREVIEWFORM_H
#define IMAGEPREVIEWFORM_H

#include <QModelIndex>
#include <QSqlRecord>
#include <QWidget>
#include "item.h"

namespace Ui {
    class ImgPreviewForm;
}

class ImgPreviewForm : public QWidget {
    Q_OBJECT

signals:
    void isClicked(QString href, int row);
    void checked(int row);

public:
    Item item;
    QModelIndex index;
    explicit ImgPreviewForm(QWidget *parent = nullptr);
    virtual ~ImgPreviewForm() override;
    void setImg(QSqlRecord record, QImage *img, QModelIndex index);
    void hideElements();
    void check();
    void uncheck();
    bool isAvailable() const;
    int getHeight() const;

private:
    Ui::ImgPreviewForm *ui;
    QImage *img;
    bool available;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    //virtual void paintEvent(QPaintEvent *event) override;
};

#endif // IMAGEPREVIEWFORM_H
