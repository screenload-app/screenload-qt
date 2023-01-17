// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jeremy Borgman

#ifndef SCREENLOAD_CACHEUTILS_H
#define SCREENLOAD_CACHEUTILS_H

class QString;
class QRect;

QString getCachePath();
QRect getLastRegion();
void setLastRegion(QRect const& newRegion);

#endif // SCREENLOAD_CACHEUTILS_H