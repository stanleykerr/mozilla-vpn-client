/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "localizer.h"
#include "constants.h"
#include "logger.h"
#include "settingsholder.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>

namespace {
Logger logger(LOG_MAIN, "Localizer");
Localizer *s_instance = nullptr;
} // namespace

// static
void Localizer::createInstance(SettingsHolder *settingsHolder)
{
    Q_ASSERT(settingsHolder);

    logger.log() << "Creating the localizer singleton";

    Q_ASSERT(!s_instance);
    s_instance = new Localizer(settingsHolder->parent());
    s_instance->initialize(settingsHolder->languageCode());
}

// static
Localizer *Localizer::instance()
{
    Q_ASSERT(s_instance);
    return s_instance;
}

Localizer::Localizer(QObject *parent) : QAbstractListModel(parent) {}

void Localizer::initialize(const QString &code)
{
    QLocale locale = QLocale::system();
    QString systemCode = locale.bcp47Name();

    m_code = code;
    if (code.isEmpty()) {
        m_code = systemCode;
    }

    loadLanguage(m_code);

    QCoreApplication::installTranslator(&m_translator);
    QDir dir(":/i18n");
    QStringList files = dir.entryList();
    for (const QString &file : files) {
        if (!file.endsWith(".qm")) {
            continue;
        }

        QStringList parts = file.split(".");
        Q_ASSERT(parts.length() == 2);

        parts = parts[0].split("_");
        Q_ASSERT(parts.length() == 2);

        QString code = parts.at(1);
        if (code != systemCode) {
            m_languages.append(code);
        }
    }
}

void Localizer::loadLanguage(const QString &code)
{
    logger.log() << "Loading language:" << code;
    if (loadLanguageInternal(code)) {
        return;
    }

    logger.log() << "Loading default language (fallback)";
    if (loadLanguageInternal("")) {
        return;
    }

    logger.log() << "Loading 'en' language(fallback 2)";
    loadLanguageInternal("en");
}

bool Localizer::loadLanguageInternal(const QString &code)
{
    QLocale locale = QLocale(code);
    if (code.isEmpty()) {
        locale = QLocale::system();
    }

    QLocale::setDefault(locale);

    if (!m_translator.load(locale, "mozillavpn", "_", ":/i18n")) {
        logger.log() << "Loading the locale failed."
                     << "code";
        return false;
    }

    return true;
}

QString Localizer::languageName(const QString &code) const
{
    QLocale locale(code);
    if (code.isEmpty()) {
        locale = QLocale::system();
    }

    if (locale.language() == QLocale::C) {
        return "English (US)";
    }

    return QLocale::languageToString(locale.language());
}

QString Localizer::localizedLanguageName(const QString &code) const
{
    QLocale locale(code);
    if (code.isEmpty()) {
        locale = QLocale::system();
    }

    if (locale.language() == QLocale::C) {
        return "English (US)";
    }

    return locale.nativeLanguageName();
}

QHash<int, QByteArray> Localizer::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LanguageRole] = "language";
    roles[LocalizedLanguageRole] = "localizedLanguage";
    roles[CodeRole] = "code";
    return roles;
}

int Localizer::rowCount(const QModelIndex &) const
{
    return m_languages.count();
}

QVariant Localizer::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case LanguageRole:
        return QVariant(languageName(m_languages.at(index.row())));

    case LocalizedLanguageRole:
        return QVariant(localizedLanguageName(m_languages.at(index.row())));

    case CodeRole:
        return QVariant(m_languages.at(index.row()));

    default:
        return QVariant();
    }
}

QString Localizer::localizeSubscriptionCurrencyValue() const
{
    return QLocale()
        .toCurrencyString(Constants::SUBSCRIPTION_CURRENCY_VALUE_USD,
                          QLocale(QLocale::English, QLocale::UnitedStates).currencySymbol());
}
