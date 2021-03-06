/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SERVERCOUNTRY_H
#define SERVERCOUNTRY_H

#include "servercity.h"

#include <QList>
#include <QString>

class ServerData;
class QJsonObject;
class QStringList;

class ServerCountry final
{
public:
    [[nodiscard]] bool fromJson(QJsonObject &obj);

    const QString &name() const { return m_name; }

    const QString &code() const { return m_code; }

    const QList<ServerCity> &cities() const { return m_cities; }

    const QList<Server> getServers(const ServerData &data) const;

private:
    QString m_name;
    QString m_code;

    QList<ServerCity> m_cities;
};

#endif // SERVERCOUNTRY_H
