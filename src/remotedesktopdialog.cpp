/*
 * SPDX-FileCopyrightText: 2018 Red Hat Inc
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 * SPDX-FileCopyrightText: 2018 Jan Grulich <jgrulich@redhat.com>
 */

#include "remotedesktopdialog.h"
#include "outputsmodel.h"

#include "utils.h"
#include <KLocalizedString>
#include <QLoggingCategory>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>
#include <QWindow>

Q_LOGGING_CATEGORY(XdgDesktopPortalKdeRemoteDesktopDialog, "xdp-kde-remote-desktop-dialog")

RemoteDesktopDialog::RemoteDesktopDialog(const QString &appName,
                                         RemoteDesktopPortal::DeviceTypes deviceTypes,
                                         bool screenSharingEnabled,
                                         bool multiple,
                                         QObject *parent)
    : QuickDialog(parent)
{
    auto model = new OutputsModel(this);

    QVariantMap props = {
        {"outputsModel", QVariant::fromValue<QObject *>(model)},
        {"withScreenSharing", screenSharingEnabled},
        {"withMultipleScreenSharing", multiple},
        {"withKeyboard", deviceTypes.testFlag(RemoteDesktopPortal::Keyboard)},
        {"withPointer", deviceTypes.testFlag(RemoteDesktopPortal::Pointer)},
        {"withTouch", deviceTypes.testFlag(RemoteDesktopPortal::TouchScreen)},
    };

    const QString applicationName = Utils::applicationName(appName);
    if (applicationName.isEmpty()) {
        props.insert(QStringLiteral("title"), i18n("Select what to share with the requesting application"));
    } else {
        props.insert(QStringLiteral("title"), i18n("Select what to share with %1", applicationName));
    }

    create(QStringLiteral("qrc:/RemoteDesktopDialog.qml"), props);
}

QList<quint32> RemoteDesktopDialog::selectedScreens() const
{
    OutputsModel *model = dynamic_cast<OutputsModel *>(m_theDialog->property("outputsModel").value<QObject *>());
    if (!model) {
        return {};
    }
    return model->selectedScreens();
}

RemoteDesktopPortal::DeviceTypes RemoteDesktopDialog::deviceTypes() const
{
    RemoteDesktopPortal::DeviceTypes types = RemoteDesktopPortal::None;
    if (m_theDialog->property("withKeyboard").toBool())
        types |= RemoteDesktopPortal::Keyboard;
    if (m_theDialog->property("withPointer").toBool())
        types |= RemoteDesktopPortal::Pointer;
    if (m_theDialog->property("withTouch").toBool())
        types |= RemoteDesktopPortal::TouchScreen;
    return types;
}
