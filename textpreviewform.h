#ifndef TEXTPREVIEWFORM_H
#define TEXTPREVIEWFORM_H

#include <QWidget>

namespace Ui {
    class textPreviewForm;
}

class textPreviewForm : public QWidget {
    Q_OBJECT

signals:
    void isClicked(int id);

public:
    explicit textPreviewForm(QWidget *parent = nullptr);
    virtual ~textPreviewForm() override;
    void setText(QString text, QString date, int id);
    void hideElements();
    bool isAvailable() const;
    bool isCheck() const;
    int getHeight() const;
    int getId() const;
    QString getDate() const;
    void check();
    void uncheck();

private:
    Ui::textPreviewForm *ui;
    bool available;
    int id;

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // TEXTPREVIEWFORM_H
