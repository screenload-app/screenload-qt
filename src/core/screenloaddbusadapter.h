// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#pragma once

#include <QtDBus/QDBusAbstractAdaptor>

class ScreenLoadDBusAdapter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "ru.screenload.ScreenLoad")

public:
    explicit ScreenLoadDBusAdapter(QObject* parent = nullptr);
    virtual ~ScreenLoadDBusAdapter();

public slots:
    Q_NOREPLY void attachScreenshotToClipboard(const QByteArray& data);
    Q_NOREPLY void attachTextToClipboard(QString text, QString notification);
    Q_NOREPLY void attachPin(const QByteArray& data);
};
