#ifndef IMAGEPREVIEWFORM_H
#define IMAGEPREVIEWFORM_H

#include <QModelIndex>
#include <QSqlRecord>
#include <QWidget>
#include "item.h"

namespace Ui {
    class imagePreviewForm;
}

class imagePreviewForm : public QWidget {
    Q_OBJECT

signals:
    void isClicked(QString href, int row);

public:
    Item item;
    QModelIndex index;
    explicit imagePreviewForm(QWidget *parent = nullptr);
    virtual ~imagePreviewForm() override;
    void setImg(QSqlRecord record, QImage *img, QModelIndex index);
    void hideElements();
    bool isAvailable() const;
    int getHeight();

private:
    Ui::imagePreviewForm *ui;
    QImage *img;
    bool available;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // IMAGEPREVIEWFORM_H
