#ifndef WINDOW_H
#define WINDOW_H

#include <QKeyEvent>
#include <QMainWindow>

class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr) :
            QMainWindow(parent) {
        this->setAttribute(Qt::WA_DeleteOnClose, true);
    };
    virtual ~Window(){};

protected:
    virtual void keyPressEvent(QKeyEvent *event) override {
        if (event->matches(QKeySequence::Close)) {
            this->close();
            this->deleteLater();
        }
        QMainWindow::keyPressEvent(event);
    }
};

#endif // WINDOW_H
