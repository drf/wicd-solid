/*  This file is part of the KDE project
    Copyright (C) 2008 Dario Freddi <drf54321@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "wirelessnetworkinterface.h"

#include "wirelessaccesspoint.h"

#include "wicddbusinterface.h"

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QProcess>

#include <KDebug>

class WicdWirelessNetworkInterface::Private
{
public:
    Private() {};

    void recacheInformation();

    QMap<int, QString> getAccessPointsWithId();

    int bitrate;
    int current_network;
    QString driver;
};

void WicdWirelessNetworkInterface::Private::recacheInformation()
{
    QDBusReply< QString > bitrater = WicdDbusInterface::instance()->wireless().call("GetCurrentBitrate");
    QDBusReply< int > networkr = WicdDbusInterface::instance()->wireless().call("GetCurrentNetworkID");
    QDBusReply< QString > driverr = WicdDbusInterface::instance()->daemon().call("GetWPADriver");

    bitrate = bitrater.value().split(' ').at(0).toInt() * 1000;
    current_network = networkr.value();
    driver = driverr.value();
}

QMap<int, QString> WicdWirelessNetworkInterface::Private::getAccessPointsWithId()
{
    QDBusReply< int > networks = WicdDbusInterface::instance()->wireless().call("GetNumberOfNetworks");
    QMap<int, QString> retlist;

    for (int i = 0; i < networks.value(); ++i) {
        QDBusReply< QString > r = WicdDbusInterface::instance()->wireless().call("GetWirelessProperty", i, "bssid");
        retlist[i] = r;
    }

    return retlist;
}

WicdWirelessNetworkInterface::WicdWirelessNetworkInterface(const QString &objectPath)
        : WicdNetworkInterface(objectPath)
        , d(new Private())
{
    d->recacheInformation();
}

WicdWirelessNetworkInterface::~WicdWirelessNetworkInterface()
{
    delete d;
}

Solid::Control::NetworkInterface::Type WicdWirelessNetworkInterface::type() const
{
    return Solid::Control::NetworkInterface::Ieee80211;
}

QString WicdWirelessNetworkInterface::driver() const
{
    return d->driver;
}

int WicdWirelessNetworkInterface::bitRate() const
{
    return d->bitrate;
}

Solid::Control::WirelessNetworkInterface::Capabilities WicdWirelessNetworkInterface::wirelessCapabilities() const
{

}

Solid::Control::WirelessNetworkInterface::OperationMode WicdWirelessNetworkInterface::mode() const
{

}

MacAddressList WicdWirelessNetworkInterface::accessPoints() const
{
    return d->getAccessPointsWithId().values();
}

QString WicdWirelessNetworkInterface::activeAccessPoint() const
{
    return d->getAccessPointsWithId()[d->current_network];
}

QString WicdWirelessNetworkInterface::hardwareAddress() const
{
    // Let's parse ifconfig here

    QProcess ifconfig;

    ifconfig.start(QString("ifconfig %1").arg(uni()));
    ifconfig.waitForFinished();

    QString result = ifconfig.readAllStandardOutput();

    QStringList lines = result.split('\n');

    return lines.at(0).split("HWaddr ").at(1);
}

QObject * WicdWirelessNetworkInterface::createAccessPoint(const QString & uni)
{
    QMap<int, QString> aps = d->getAccessPointsWithId();

    if (!aps.values().contains(uni)) {
        kDebug() << "Requested a non existent AP";
    }

    int network = aps.key(uni);

    return new WicdAccessPoint(network);
}

#include "wirelessnetworkinterface.moc"
