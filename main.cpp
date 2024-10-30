#include "welcome.h"
#include <QApplication>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QString path = QDir::temp().absoluteFilePath("SingleApp.lock");
    QLockFile lockFile(path);

    bool isLock = lockFile.isLocked();
    (void)isLock;
    if (!lockFile.tryLock(100)) {
        QMessageBox::warning(NULL, "提示", "程序已在运行中");
        return 0;
    }

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    Welcome w;
    w.show();

    return a.exec();
}
