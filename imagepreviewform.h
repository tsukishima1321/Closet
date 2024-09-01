#ifndef IMAGEPREVIEWFORM_H
#define IMAGEPREVIEWFORM_H

#include <QWidget>

namespace Ui {
    class imagePreviewForm;
}

class imagePreviewForm : public QWidget {
    Q_OBJECT

signals:
    void isClicked(QString href);

public:
    QString href;
    explicit imagePreviewForm(QWidget *parent = nullptr);
    virtual ~imagePreviewForm() override;
    void setImg(QString href, std::shared_ptr<QImage> img, QString des);
    void hideElements();
    bool isAvailable() const;
    int getHeight();

private:
    Ui::imagePreviewForm *ui;
    std::shared_ptr<QImage> img;
    bool available;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // IMAGEPREVIEWFORM_H
