#include "screenloaddaemon.h"

#include "abstractlogger.h"
#include "confighandler.h"
#include "screenload.h"
#include "pinwidget.h"
#include "screenshotsaver.h"
#include "src/utils/globalvalues.h"
#include "src/widgets/capture/capturewidget.h"
#include "src/widgets/trayicon.h"
#include <QApplication>
#include <QClipboard>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QRect>
#include <QTimer>
#include <QUrl>

#ifdef Q_OS_WIN
#include "src/core/globalshortcutfilter.h"
#endif

/**
 * @brief A way of accessing the screenload daemon both from the daemon itself,
 * and from subcommands.
 *
 * The daemon is necessary in order to:
 * - Host the system tray,
 * - Listen for hotkey events that will trigger captures,
 * - Host pinned screenshot widgets,
 * - Host the clipboard on X11, where the clipboard gets lost once screenload
 *   quits.
 *
 * If the `autoCloseIdleDaemon` option is true, the daemon will close as soon as
 * it is not needed to host pinned screenshots and the clipboard. On Windows,
 * this option is disabled and the daemon always persists, because the system
 * tray is currently the only way to interact with screenload there.
 *
 * Both the daemon and non-daemon screenload processes use the same public API,
 * which is implemented as static methods. In the daemon process, this class is
 * also instantiated as a singleton, so it can listen to D-Bus calls via the
 * sigslot mechanism. The instantiation is done by calling `start` (this must be
 * done only in the daemon process). Any instance (as opposed to static) members
 * can only be used if the current process is a daemon.
 *
 * @note The daemon will be automatically launched where necessary, via D-Bus.
 * This applies only to Linux.
 */
ScreenLoadDaemon::ScreenLoadDaemon()
  : m_persist(false)
  , m_hostingClipboard(false)
  , m_clipboardSignalBlocked(false)
  , m_trayIcon(nullptr)
  , m_networkCheckUpdates(nullptr)
  , m_showCheckAppUpdateStatus(false)
  , m_appLatestVersion(QStringLiteral(APP_VERSION).replace("v", ""))
{
    connect(
      QApplication::clipboard(), &QClipboard::dataChanged, this, [this]() {
          if (!m_hostingClipboard || m_clipboardSignalBlocked) {
              m_clipboardSignalBlocked = false;
              return;
          }
          m_hostingClipboard = false;
          quitIfIdle();
      });
#ifdef Q_OS_WIN
    m_persist = true;
#else
    m_persist = !ConfigHandler().autoCloseIdleDaemon();
    connect(ConfigHandler::getInstance(),
            &ConfigHandler::fileChanged,
            this,
            [this]() {
                ConfigHandler config;
                if (config.disabledTrayIcon()) {
                    enableTrayIcon(false);
                } else {
                    enableTrayIcon(true);
                }
                m_persist = !config.autoCloseIdleDaemon();
            });
#endif
    if (ConfigHandler().checkForUpdates()) {
        getLatestAvailableVersion();
    }
}

void ScreenLoadDaemon::start()
{
    if (!m_instance) {
        m_instance = new ScreenLoadDaemon();
        // Tray icon needs ScreenLoadDaemon::instance() to be non-null
        m_instance->initTrayIcon();
        qApp->setQuitOnLastWindowClosed(false);
    }
}

void ScreenLoadDaemon::createPin(QPixmap capture, QRect geometry)
{
    if (instance()) {
        instance()->attachPin(capture, geometry);
        return;
    }

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << capture;
    stream << geometry;
    QDBusMessage m = createMethodCall(QStringLiteral("attachPin"));
    m << data;
    call(m);
}

void ScreenLoadDaemon::copyToClipboard(QPixmap capture)
{
    if (instance()) {
        instance()->attachScreenshotToClipboard(capture);
        return;
    }

    QDBusMessage m =
      createMethodCall(QStringLiteral("attachScreenshotToClipboard"));

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << capture;

    m << data;
    call(m);
}

void ScreenLoadDaemon::copyToClipboard(QString text, QString notification)
{
    if (instance()) {
        instance()->attachTextToClipboard(text, notification);
        return;
    }
    auto m = createMethodCall(QStringLiteral("attachTextToClipboard"));

    m << text << notification;

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    checkDBusConnection(sessionBus);
    sessionBus.call(m);
}

/**
 * @brief Is this instance of screenload hosting any windows as a daemon?
 */
bool ScreenLoadDaemon::isThisInstanceHostingWidgets()
{
    return instance() && !instance()->m_widgets.isEmpty();
}

void ScreenLoadDaemon::sendTrayNotification(const QString& text,
                                           const QString& title,
                                           const int timeout)
{
    if (m_trayIcon) {
        m_trayIcon->showMessage(
          title, text, QIcon(GlobalValues::iconPath()), timeout);
    }
}

void ScreenLoadDaemon::showUpdateNotificationIfAvailable(CaptureWidget* widget)
{
    if (!m_appLatestUrl.isEmpty() &&
        ConfigHandler().ignoreUpdateToVersion().compare(m_appLatestVersion) <
          0) {
        widget->showAppUpdateNotification(m_appLatestVersion, m_appLatestUrl);
    }
}

void ScreenLoadDaemon::getLatestAvailableVersion()
{
    // This features is required for MacOS and Windows user and for Linux users
    // who installed ScreenLoad not from the repository.
    m_networkCheckUpdates = new QNetworkAccessManager(this);
    QNetworkRequest requestCheckUpdates(QUrl(SCREENLOAD_APP_VERSION_URL));
    connect(m_networkCheckUpdates,
            &QNetworkAccessManager::finished,
            this,
            &ScreenLoadDaemon::handleReplyCheckUpdates);
    m_networkCheckUpdates->get(requestCheckUpdates);

    // check for updates each 24 hours
    QTimer::singleShot(1000 * 60 * 60 * 24, [this]() {
        if (ConfigHandler().checkForUpdates()) {
            this->getLatestAvailableVersion();
        }
    });
}

void ScreenLoadDaemon::checkForUpdates()
{
    if (m_appLatestUrl.isEmpty()) {
        m_showCheckAppUpdateStatus = true;
        getLatestAvailableVersion();
    } else {
        QDesktopServices::openUrl(QUrl(m_appLatestUrl));
    }
}

/**
 * @brief Return the daemon instance.
 *
 * If this instance of screenload is the daemon, a singleton instance of
 * `ScreenLoadDaemon` is returned. As a side effect`start` will called if it
 * wasn't called earlier. If this instance of screenload is not the daemon,
 * `nullptr` is returned.
 *
 * This strategy is used because the daemon needs to receive signals from D-Bus,
 * for which an instance of a `QObject` is required. The singleton serves as
 * that object.
 */
ScreenLoadDaemon* ScreenLoadDaemon::instance()
{
    // Because we don't use DBus on MacOS, each instance of screenload is its own
    // mini-daemon, responsible for hosting its own persistent widgets (e.g.
    // pins).
#if defined(Q_OS_MACOS)
    start();
#endif
    return m_instance;
}

/**
 * @brief Quit the daemon if it has nothing to do and the 'persist' flag is not
 * set.
 */
void ScreenLoadDaemon::quitIfIdle()
{
    if (m_persist) {
        return;
    }
    if (!m_hostingClipboard && m_widgets.isEmpty()) {
        qApp->exit(0);
    }
}

// SERVICE METHODS

void ScreenLoadDaemon::attachPin(QPixmap pixmap, QRect geometry)
{
    auto* pinWidget = new PinWidget(pixmap, geometry);
    m_widgets.append(pinWidget);
    connect(pinWidget, &QObject::destroyed, this, [=]() {
        m_widgets.removeOne(pinWidget);
        quitIfIdle();
    });

    pinWidget->show();
    pinWidget->activateWindow();
}

void ScreenLoadDaemon::attachScreenshotToClipboard(QPixmap pixmap)
{
    m_hostingClipboard = true;
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->blockSignals(true);
    // This variable is necessary because the signal doesn't get blocked on
    // windows for some reason
    m_clipboardSignalBlocked = true;
    saveToClipboard(pixmap);
    clipboard->blockSignals(false);
}

// D-BUS ADAPTER METHODS

void ScreenLoadDaemon::attachPin(const QByteArray& data)
{
    QDataStream stream(data);
    QPixmap pixmap;
    QRect geometry;

    stream >> pixmap;
    stream >> geometry;

    attachPin(pixmap, geometry);
}

void ScreenLoadDaemon::attachScreenshotToClipboard(const QByteArray& screenshot)
{
    QDataStream stream(screenshot);
    QPixmap p;
    stream >> p;

    attachScreenshotToClipboard(p);
}

void ScreenLoadDaemon::attachTextToClipboard(QString text, QString notification)
{
    // Must send notification before clipboard modification on linux
    if (!notification.isEmpty()) {
        AbstractLogger::info() << notification;
    }

    m_hostingClipboard = true;
    QClipboard* clipboard = QApplication::clipboard();

    clipboard->blockSignals(true);
    // This variable is necessary because the signal doesn't get blocked on
    // windows for some reason
    m_clipboardSignalBlocked = true;
    clipboard->setText(text);
    clipboard->blockSignals(false);
}

void ScreenLoadDaemon::initTrayIcon()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    if (!ConfigHandler().disabledTrayIcon()) {
        enableTrayIcon(true);
    }
#elif defined(Q_OS_WIN)
    enableTrayIcon(true);

    GlobalShortcutFilter* nativeFilter = new GlobalShortcutFilter(this);
    qApp->installNativeEventFilter(nativeFilter);
    connect(nativeFilter, &GlobalShortcutFilter::printPressed, this, [this]() {
        ScreenLoad::instance()->gui();
    });
#endif
}

void ScreenLoadDaemon::enableTrayIcon(bool enable)
{
    if (enable) {
        if (m_trayIcon == nullptr) {
            m_trayIcon = new TrayIcon();
        } else {
            m_trayIcon->show();
            return;
        }
    } else if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

// MarketKernel CheckUpdates
void ScreenLoadDaemon::handleReplyCheckUpdates(QNetworkReply* reply)
{
    if (!ConfigHandler().checkForUpdates())
        return;

    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
        QJsonObject json = response.object();
        m_appLatestVersion = json["tag_name"].toString().replace("v", "");

        QVersionNumber appLatestVersion = QVersionNumber::fromString(m_appLatestVersion);

        if (ScreenLoad::instance()->getVersion() < appLatestVersion)
        {
            emit newVersionAvailable(appLatestVersion);
            m_appLatestUrl = json["html_url"].toString();
            QString newVersion =
              tr("New version %1 is available").arg(m_appLatestVersion);
            if (m_showCheckAppUpdateStatus)
            {
                sendTrayNotification(newVersion, "ScreenLoad");
                QDesktopServices::openUrl(QUrl(m_appLatestUrl));
            }

        }
        else if (m_showCheckAppUpdateStatus)
        {
            sendTrayNotification(tr("You have the latest version"),
                                 "ScreenLoad");
        }
    }
    else {
        qWarning() << "Failed to get information about the latest version. "
                   << reply->errorString();
        if (m_showCheckAppUpdateStatus)
        {
            if (ScreenLoadDaemon::instance()) {
                ScreenLoadDaemon::instance()->sendTrayNotification(
                  tr("Failed to get information about the latest version."),
                  "Screenload");
            }
        }
    }
    m_showCheckAppUpdateStatus = false;
}

QDBusMessage ScreenLoadDaemon::createMethodCall(QString method)
{
    QDBusMessage m =
      QDBusMessage::createMethodCall(QStringLiteral("ru.screenload.ScreenLoad"),
                                     QStringLiteral("/"),
                                     QLatin1String(""),
                                     method);
    return m;
}

void ScreenLoadDaemon::checkDBusConnection(const QDBusConnection& connection)
{
    if (!connection.isConnected()) {
        AbstractLogger::error() << tr("Unable to connect via DBus");
        qApp->exit(1);
    }
}

void ScreenLoadDaemon::call(const QDBusMessage& m)
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    checkDBusConnection(sessionBus);
    sessionBus.call(m);
}

// STATIC ATTRIBUTES
ScreenLoadDaemon* ScreenLoadDaemon::m_instance = nullptr;
