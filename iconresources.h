#ifndef ICONRESOURCES_H
#define ICONRESOURCES_H

#include <QIcon>
#include <QMap>

class IconResources {
public:
    static const QMap<QString, QIcon> &getIcons();
    QMap<QString, QIcon> icons;

protected:
    IconResources();
    static IconResources *_instance;
};

#endif // ICONRESOURCES_H
