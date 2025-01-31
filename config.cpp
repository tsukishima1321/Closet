#include "config.h"

Config *Config::instance = nullptr;

Config::Config() {
    imgBase = "D:/Z/Pictures/";
    remoteBase = "http://localhost:1096/";
}

Config::~Config() {}

Config *Config::getInstance() {
    if (instance == nullptr) {
        instance = new Config();
    }
    return instance;
}

QString Config::getImgBase() {
    return imgBase;
}   

QString Config::getRemoteBase() {
    return remoteBase;
}

void Config::setImgBase(const QString &imgBase) {
    this->imgBase = imgBase;
}

void Config::setRemoteBase(const QString &remoteBase) {
    this->remoteBase = remoteBase;
}