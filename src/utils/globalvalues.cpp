// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "globalvalues.h"
#include <QApplication>
#include <QFontMetrics>

int GlobalValues::buttonBaseSize()
{
    //return QApplication::fontMetrics().lineSpacing() * 2.2;
    return 40;
}

QString GlobalValues::versionInfo()
{
    return QStringLiteral("ScreenLoad " APP_VERSION " (" SCREENLOAD_GIT_HASH ")"
                          "\nCompiled with Qt " QT_VERSION_STR);
}

QString GlobalValues::iconPath()
{
#if USE_MONOCHROME_ICON
    return QString(":img/app/screenload.monochrome.svg");
#else
    return { ":img/app/screenload.svg" };
#endif
}

QString GlobalValues::iconPathPNG()
{
#if USE_MONOCHROME_ICON
    return QString(":img/app/screenload.monochrome.png");
#else
    return { ":img/app/screenload.png" };
#endif
}
