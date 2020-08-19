#ifndef USERDATA_H
#define USERDATA_H

#include "devicedata.h"

#include <QObject>
#include <QString>
#include <QStringList>

class QJsonObject;
class QSettings;

class UserData final : public QObject
{
public:
    static UserData *fromJson(QJsonObject &obj);

    static UserData *fromSettings(QSettings &settings);

    void writeSettings(QSettings &settings);

    bool hasDevice(const QString &deviceName) const;

    bool hasPrivateKeyDevice(const QString &deviceName) const;

    const DeviceData *device(const QString &deviceName) const;

    void addDevice(const DeviceData &deviceData);

    void removeDevice(const QString &deviceName);

    const QList<DeviceData> &devices() const { return m_devices; }

private:
    UserData() = default;

private:
    QString m_avatar;
    QString m_displayName;
    QString m_email;
    QList<DeviceData> m_devices;
    uint32_t m_maxDevices;
};

#endif // USERDATA_H