/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "macosmenubar.h"
#include "logger.h"
#include "mozillavpn.h"

#include <QMenuBar>

namespace {
Logger logger(LOG_MACOS, "MacOSManuBar");
}

// static
void MacOSMenuBar::initialize()
{
    logger.log() << "Creating menubar";

    MozillaVPN *vpn = MozillaVPN::instance();

    QMenuBar *menuBar = new QMenuBar(nullptr);

    //% "File"
    QMenu *fileMenu = menuBar->addMenu(qtTrId("menubar.file.title"));

    // Do not use qtTrId here!
    QAction *quit = fileMenu->addAction("quit", vpn->controller(), &Controller::quit);
    quit->setMenuRole(QAction::QuitRole);

    // Do not use qtTrId here!
    m_aboutAction = fileMenu->addAction("about.vpn", vpn, &MozillaVPN::requestAbout);
    m_aboutAction->setMenuRole(QAction::AboutRole);
    m_aboutAction->setVisible(vpn->state() == MozillaVPN::StateMain);

    // Do not use qtTrId here!
    m_preferencesAction = fileMenu->addAction("preferences", vpn, &MozillaVPN::requestSettings);
    m_preferencesAction->setMenuRole(QAction::PreferencesRole);
    m_preferencesAction->setVisible(vpn->state() == MozillaVPN::StateMain);

    //% "Close"
    QAction *close = fileMenu->addAction(qtTrId("menubar.file.close"),
                                         vpn->controller(),
                                         &Controller::quit);
    close->setShortcut(QKeySequence::Close);

    //% "Help"
    QMenu *helpMenu = menuBar->addMenu(qtTrId("menubar.help.title"));
    vpn->helpModel()->forEach([&](const QString &name, int id) {
        helpMenu->addAction(name, [help = vpn->helpModel(), id]() { help->open(id); });
    });
};

void MacOSMenuBar::controllerStateChanged()
{
    MozillaVPN *vpn = MozillaVPN::instance();
    m_preferencesAction->setVisible(vpn->state() == MozillaVPN::StateMain);
    m_aboutAction->setVisible(vpn->state() == MozillaVPN::StateMain);
}
