#include "qtsinglecoreapplication\QtSingleApplication"
#include "welcome.h"
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QtSingleApplication a(argc, argv);
    if (a.isRunning()) {
        QMessageBox::warning(NULL, "提示", "程序已在运行中");
        a.sendMessage("", 0);
        return 0;
    }

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    Welcome w;
    QObject::connect(&a, &QtSingleApplication::messageReceived, &w, [&w](const QString &mes) {
        (void)mes;
        w.show();
        w.raise();
        w.activateWindow();
    });
    w.show();

    return a.exec();
}
