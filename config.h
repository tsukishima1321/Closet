#pragma once
#include <QString>
class Config {
protected:
    static Config *instance;
    Config();
    ~Config();
    QString imgBase;
    QString remoteBase;

public:
    static Config *getInstance();
    QString getImgBase();
    QString getRemoteBase();
    void setImgBase(const QString &imgBase);
    void setRemoteBase(const QString &remoteBase);
};