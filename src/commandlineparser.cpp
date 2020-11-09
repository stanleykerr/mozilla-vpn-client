/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "commandlineparser.h"
#include "command.h"
#include "commands/commandui.h"
#include "logger.h"
#include "mozillavpn.h"
#include "settingsholder.h"

constexpr const char *CLP_DEFAULT_COMMAND = "ui";

namespace {
Logger logger(LOG_MAIN, "CommandLineParser");
}

void CommandLineParser::parse(int argc, char *argv[])
{
    Q_ASSERT(argc >= 1);

    QStringList tokens;
    for (int i = 0; i < argc; ++i) {
        tokens.append(QString(argv[i]));
    }

    QList<Option *> options;

    Option helpOption("h", "help", "Displays help on commandline options");
    options.append(&helpOption);

    Option versionOption("v", "version", "Displays version information.");
    options.append(&versionOption);

    parse(tokens, options, true);

    if (helpOption.m_set) {
        showHelp(argv[0], options, true);
        exit(0);
    }

    if (versionOption.m_set) {
        QTextStream stream(stdout);
        stream << argv[0] << " " << APP_VERSION << Qt::endl;
        exit(0);
    }

    if (tokens.isEmpty()) {
        tokens.append(CLP_DEFAULT_COMMAND);
    }

    for (Command *command : Command::s_commands) {
        if (command->name() == tokens[0]) {
            tokens[0] = QString("%1 %2").arg(argv[0]).arg(tokens[0]);
            exit(command->run(tokens));
        }
    }

    QTextStream stream(stderr);
    stream << argv[0] << ": '" << tokens[0] << "' is not a valid command. See '" << argv[0]
           << " --help'." << Qt::endl;
    exit(1);
}

void CommandLineParser::parse(QStringList &tokens, QList<Option *> &options, bool hasCommands)
{
    Q_ASSERT(!tokens.isEmpty());

    QString app = tokens.takeFirst();

    if (!parseOptions(tokens, options)) {
        Q_ASSERT(!tokens.isEmpty());
        unknownOption(tokens[0], app, options, hasCommands);
        Q_UNREACHABLE();
    }
}

// static
bool CommandLineParser::parseOptions(QStringList &tokens, QList<Option *> &options)
{
    while (!tokens.isEmpty()) {
        if (!tokens[0].startsWith("-")) {
            return true;
        }

        QString opt = tokens[0];

        bool shortOption = true;
        opt.remove(0, 1);

        if (opt.startsWith("-")) {
            opt.remove(0, 1);
            shortOption = false;
        }

        if (!parseOption(opt, shortOption, options)) {
            return false;
        }

        tokens.removeFirst();
    }

    return true;
}

// static
bool CommandLineParser::parseOption(const QString &option,
                                    bool shortOption,
                                    QList<Option *> &options)
{
    for (Option *o : options) {
        if (shortOption && o->m_short == option) {
            o->m_set = true;
            return true;
        }

        if (!shortOption && o->m_long == option) {
            o->m_set = true;
            return true;
        }
    }

    return false;
}

//static
void CommandLineParser::unknownOption(const QString &option,
                                      const QString &app,
                                      QList<Option *> &options,
                                      bool hasCommands)
{
    QTextStream stream(stderr);
    stream << "unknown option: " << option << Qt::endl;
    showHelp(app, options, hasCommands);
    exit(1);
}

// static
void CommandLineParser::showHelp(const QString &app,
                                 const QList<Option *> &options,
                                 bool hasCommands)
{
    QTextStream stream(stdout);
    stream << "usage: " << app;

    for (const Option *o : options) {
        stream << " [-" << o->m_short << " | --" << o->m_long << "]";
    }

    if (hasCommands) {
        stream << " <commands> [<args>]";
    }

    stream << Qt::endl;

    if (!hasCommands) {
        return;
    }

    stream << Qt::endl;
    stream << "List of the commands:" << Qt::endl;

    for (Command *command : Command::s_commands) {
        stream << "  " << command->name();

        for (int i = command->name().length(); i < 20; ++i) {
            stream << " ";
        }

        stream << command->description() << Qt::endl;
    }
}