#include "controller.h"
#include "mozillavpn.h"
#include "server.h"

#ifdef __linux__
#include "platforms/linux/linuxcontroller.h"
#else
#include "platforms/dummy/dummycontroller.h"
#endif

#include <QDebug>
#include <QTimer>

Controller::Controller()
{
#ifdef __linux__
    m_impl.reset(new LinuxController());
#else
    m_impl.reset(new DummyController());
#endif

    connect(m_impl.get(), &ControllerImpl::connected, this, &Controller::connected);
    connect(m_impl.get(), &ControllerImpl::disconnected, this, &Controller::disconnected);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Controller::timeUpdated);
}

void Controller::setVPN(MozillaVPN *vpn)
{
    Q_ASSERT(!m_vpn);
    m_vpn = vpn;
}

void Controller::activate()
{
    qDebug() << "Activation";

    if (m_state != StateOff && m_state != StateSwitching) {
        qDebug() << "Already disconnected";
        return;
    }

    QList<Server> servers = m_vpn->getServers();
    Q_ASSERT(!servers.isEmpty());

    Server selectedServer = Server::weightChooser(servers);

    const Device *device = m_vpn->deviceModel()->currentDevice();

    if (m_state == StateOff) {
        m_state = StateConnecting;
        emit stateChanged();

        m_timer->stop();
    }

    m_impl->activate(selectedServer, device, m_vpn->keys());
}

void Controller::deactivate()
{
    qDebug() << "Deactivation";

    if (m_state != StateOn && m_state != StateSwitching) {
        qDebug() << "Already disconnected";
        return;
    }

    Q_ASSERT(m_state == StateOn || m_state == StateSwitching);

    if (m_state == StateOn) {
        m_state = StateDisconnecting;
        emit stateChanged();
    }

    m_timer->stop();

    QList<Server> servers = m_vpn->getServers();
    Q_ASSERT(!servers.isEmpty());

    Server selectedServer = Server::weightChooser(servers);

    const Device *device = m_vpn->deviceModel()->currentDevice();

    m_impl->deactivate(selectedServer, device, m_vpn->keys());
}

void Controller::connected() {
    qDebug() << "Connected";

    Q_ASSERT(m_state == StateConnecting || m_state == StateSwitching);
    m_state = StateOn;
    emit stateChanged();

    m_time = 0;
    emit timeChanged();

    if (m_quitting) {
        disconnect();
        return;
    }

    Q_ASSERT(!m_timer->isActive());
    m_timer->start(1000);
}

void Controller::disconnected() {
    qDebug() << "Disconnected";

    Q_ASSERT(m_state == StateDisconnecting || m_state == StateConnecting
             || m_state == StateSwitching);

    // No need to continue if we are about to quit.
    if (m_quitting) {
        emit readyToQuit();
        return;
    }

    if (m_state == StateSwitching) {
        m_vpn->changeServer(m_switchingCountryCode, m_switchingCity);
        activate();
        return;
    }

    m_state = StateOff;
    emit stateChanged();
}

void Controller::timeUpdated() {
    Q_ASSERT(m_state == StateOn);

    ++m_time;
    emit timeChanged();
}

void Controller::changeServer(const QString &countryCode, const QString &city)
{
    Q_ASSERT(m_state == StateOn || m_state == StateOff);

    if (m_vpn->currentServer()->countryCode() == countryCode
        && m_vpn->currentServer()->city() == city) {
        qDebug() << "No server change needed";
        return;
    }

    if (m_state == StateOff) {
        qDebug() << "Change server";
        m_vpn->changeServer(countryCode, city);
        return;
    }

    m_timer->stop();

    qDebug() << "Switching to a different server";

    m_state = StateSwitching;
    emit stateChanged();

    m_switchingCountryCode = countryCode;
    m_switchingCity = city;

    deactivate();
}

void Controller::quit()
{
    qDebug() << "Quitting";

    m_quitting = true;

    if (m_state == StateOff) {
        emit readyToQuit();
        return;
    }

    if (m_state == StateOn) {
        deactivate();
        return;
    }
}