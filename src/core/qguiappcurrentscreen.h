// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Yuriy Puchkov <yuriy.puchkov@namecheap.com>

#ifndef SCREENLOAD_QGUIAPPCURRENTSCREEN_H
#define SCREENLOAD_QGUIAPPCURRENTSCREEN_H

#include <QPoint>

class QScreen;

class QGuiAppCurrentScreen
{
public:
    explicit QGuiAppCurrentScreen();
    QScreen* currentScreen();
    QScreen* currentScreen(const QPoint& pos);

private:
    QScreen* screenAt(const QPoint& pos);

    // class members
private:
    QScreen* m_currentScreen;
};

#endif // SCREENLOAD_QGUIAPPCURRENTSCREEN_H
