#ifndef TEXTPREVIEWFORM_H
#define TEXTPREVIEWFORM_H

#include <QWidget>

namespace Ui {
    class TextPreviewForm;
}

class TextPreviewForm : public QWidget {
    Q_OBJECT

signals:
    void isClicked(int id);

public:
    explicit TextPreviewForm(QWidget *parent = nullptr);
    virtual ~TextPreviewForm() override;
    void setText(QString text, QString date, int id);
    void hideElements();
    bool isAvailable() const;
    bool isCheck() const;
    int getHeight() const;
    int getId() const;
    QString getDate() const;
    void check();
    void uncheck();
    void setOmit(bool omit);

private:
    Ui::TextPreviewForm *ui;
    bool available;
    int id;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // TEXTPREVIEWFORM_H
