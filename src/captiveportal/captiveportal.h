/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CAPTIVEPORTAL_H
#define CAPTIVEPORTAL_H

#include <QStringList>

constexpr int32_t CAPTIVEPORTAL_LOOKUPTIMER = 5000;

constexpr const char *CAPTIVEPORTAL_HOST = "detectportal.firefox.com";
constexpr const char *CAPTIVEPORTAL_URL_IPV4 = "http://%1/success.txt";
constexpr const char *CAPTIVEPORTAL_URL_IPV6 = "http://[%1]/success.txt";

constexpr const char *CAPTIVEPORTAL_REQUEST_CONTENT = "success";

class CaptivePortal final
{
public:
    [[nodiscard]] bool fromJson(const QByteArray &json);

    [[nodiscard]] bool fromSettings();

    void writeSettings();

    const QStringList &ipv4Addresses() const { return m_ipv4Addresses; }
    const QStringList &ipv6Addresses() const { return m_ipv6Addresses; }

private:
    QStringList m_ipv4Addresses;
    QStringList m_ipv6Addresses;
};

#endif // CAPTIVEPORTAL_H
