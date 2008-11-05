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

#ifndef WICD_NETWORKINTERFACE_H
#define WICD_NETWORKINTERFACE_H


#include <solid/control/ifaces/networkinterface.h>

#include <QtCore/qobject.h>
#include <QVariantMap>

class QDBusInterface;

class WicdNetworkInterfacePrivate;

class WicdNetworkInterface : public QObject, virtual public Solid::Control::Ifaces::NetworkInterface
{
Q_OBJECT
Q_INTERFACES(Solid::Control::Ifaces::NetworkInterface)
public:
    WicdNetworkInterface(const QString &name);
    virtual ~WicdNetworkInterface();
    QString interfaceName() const;
    QString driver() const;
    Solid::Control::IPv4Config ipV4Config() const;
    QString uni() const;
    bool isActive() const;
    Solid::Control::NetworkInterface::Type type() const;
    Solid::Control::NetworkInterface::ConnectionState connectionState() const;
    int designSpeed() const;
    Solid::Control::NetworkInterface::Capabilities capabilities() const;

    virtual bool activateConnection(const QString & connectionUni, const QVariantMap & connectionParameters);
    virtual bool deactivateConnection();
Q_SIGNALS:
    void ipDetailsChanged();
    void connectionStateChanged(int state);
private:
    WicdNetworkInterfacePrivate *d;
};

#endif
