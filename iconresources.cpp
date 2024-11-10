#include "iconresources.h"

IconResources *IconResources::_instance = nullptr;

IconResources::IconResources() {
    icons["download"] = QIcon(":/pic/icons/download.png");
    icons["refresh"] = QIcon(":/pic/icons/refresh.png");
    icons["reset"] = QIcon(":/pic/icons/reset.png");
    icons["square"] = QIcon(":/pic/icons/square.png");
    icons["square-check"] = QIcon(":/pic/icons/square-check.png");
    icons["square-x"] = QIcon(":/pic/icons/square-x.png");
    icons["step-back"] = QIcon(":/pic/icons/step-back.png");
    icons["step-forward"] = QIcon(":/pic/icons/step-forward.png");
    icons["trash"] = QIcon(":/pic/icons/trash.png");
    icons["zoom-in"] = QIcon(":/pic/icons/zoom-in.png");
    icons["zoom-out"] = QIcon(":/pic/icons/zoom-out.png");
    icons["main-icon"] = QIcon(":/pic/icons/icon.ico");
    icons["new-file"] = QIcon(":/pic/icons/file-plus.png");
    icons["combine"] = QIcon(":/pic/icons/combine.png");
    icons["rotate-left"] = QIcon(":/pic/icons/rotate-ccw.png");
    icons["rotate-right"] = QIcon(":/pic/icons/rotate-cw.png");
}

const QMap<QString, QIcon> &IconResources::getIcons() {
    if (!_instance) {
        _instance = new IconResources;
    }
    return _instance->icons;
}
