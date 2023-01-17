// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "screenloaddbusadapter.h"
#include "src/core/screenloaddaemon.h"

ScreenLoadDBusAdapter::ScreenLoadDBusAdapter(QObject* parent)
  : QDBusAbstractAdaptor(parent)
{}

ScreenLoadDBusAdapter::~ScreenLoadDBusAdapter() = default;

void ScreenLoadDBusAdapter::attachScreenshotToClipboard(const QByteArray& data)
{
    ScreenLoadDaemon::instance()->attachScreenshotToClipboard(data);
}

void ScreenLoadDBusAdapter::attachTextToClipboard(QString text,
                                                 QString notification)
{
    ScreenLoadDaemon::instance()->attachTextToClipboard(text, notification);
}

void ScreenLoadDBusAdapter::attachPin(const QByteArray& data)
{
    ScreenLoadDaemon::instance()->attachPin(data);
}
