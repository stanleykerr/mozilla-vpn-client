/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "server.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>

bool Server::fromJson(QJsonObject &obj)
{
    // Reset.
    m_hostname = "";

    QJsonValue hostname = obj.take("hostname");
    if (!hostname.isString()) {
        return false;
    }

    QJsonValue ipv4AddrIn = obj.take("ipv4_addr_in");
    if (!ipv4AddrIn.isString()) {
        return false;
    }

    QJsonValue ipv4Gateway = obj.take("ipv4_gateway");
    if (!ipv4Gateway.isString()) {
        return false;
    }

    QJsonValue ipv6AddrIn = obj.take("ipv6_addr_in");
    // If this object comes from the IOS migration, the ipv6_addr_in is missing.

    QJsonValue ipv6Gateway = obj.take("ipv6_gateway");
    if (!ipv6Gateway.isString()) {
        return false;
    }

    QJsonValue publicKey = obj.take("public_key");
    if (!publicKey.isString()) {
        return false;
    }

    QJsonValue weight = obj.take("weight");
    if (!weight.isDouble()) {
        return false;
    }

    QJsonValue portRanges = obj.take("port_ranges");
    if (!portRanges.isArray()) {
        return false;
    }

    QList<QPair<uint32_t, uint32_t>> prList;
    QJsonArray portRangesArray = portRanges.toArray();
    for (QJsonValue portRangeValue : portRangesArray) {
        if (!portRangeValue.isArray()) {
            return false;
        }

        QJsonArray port = portRangeValue.toArray();
        if (port.count() != 2) {
            return false;
        }

        QJsonValue a = port.at(0);
        if (!a.isDouble()) {
            return false;
        }

        QJsonValue b = port.at(1);
        if (!b.isDouble()) {
            return false;
        }

        prList.append(QPair<uint32_t, uint32_t>(a.toInt(), b.toInt()));
    }

    m_hostname = hostname.toString();
    m_ipv4AddrIn = ipv4AddrIn.toString();
    m_ipv4Gateway = ipv4Gateway.toString();
    m_ipv6AddrIn = ipv6AddrIn.toString();
    m_ipv6Gateway = ipv6Gateway.toString();
    m_portRanges.swap(prList);
    m_publicKey = publicKey.toString();
    m_weight = weight.toInt();

    return true;
}

// static
const Server &Server::weightChooser(const QList<Server> &servers)
{
    Q_ASSERT(!servers.isEmpty());

    uint32_t weightSum = 0;

    for (const Server &server : servers) {
        weightSum += server.weight();
    }

    quint32 r = QRandomGenerator::global()->generate() % (weightSum + 1);

    for (const Server &server : servers) {
        if (server.weight() >= r) {
            return server;
        }

        r -= server.weight();
    }

    // This should not happen.
    Q_ASSERT(false);
    return servers[0];
}

uint32_t Server::choosePort() const
{
    if (m_portRanges.isEmpty()) {
        return 0;
    }

    quint32 r = QRandomGenerator::global()->generate() % m_portRanges.length();
    const QPair<uint32_t, uint32_t> &ports = m_portRanges.at(r);

    if (ports.first == ports.second) {
        return ports.first;
    }

    Q_ASSERT(ports.first < ports.second);
    return ports.first + (QRandomGenerator::global()->generate() % (ports.second - ports.first));
}
