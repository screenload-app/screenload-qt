// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "imguploaderbase.h"
#include "src/core/screenloaddaemon.h"
#include "src/utils/confighandler.h"
#include "src/utils/globalvalues.h"
#include "src/utils/screenshotsaver.h"
#include "src/widgets/imagelabel.h"
#include "src/widgets/loadspinner.h"
#include "src/widgets/notificationwidget.h"
#include <QApplication>
// FIXME #include <QBuffer>
#include <QClipboard>
#include <QCursor>
#include <QDesktopServices>
#include <QDrag>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMimeData>
#include <QNetworkAccessManager>
#include <QPushButton>
#include <QRect>
#include <QScreen>
#include <QShortcut>
#include <QTimer>
#include <QUrlQuery>
#include <QVBoxLayout>
#include <QMessageBox>

ImgUploaderBase::ImgUploaderBase(const QPixmap& capture, QWidget* parent)
  : QWidget(parent)
  , m_pixmap(capture)
{
    setWindowTitle(tr("Upload image"));
    setWindowIcon(QIcon(GlobalValues::iconPath()));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QRect position = frameGeometry();
    QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    position.moveCenter(screen->availableGeometry().center());
    move(position.topLeft());
#endif

    m_spinner = new LoadSpinner(this);
    m_spinner->setColor(ConfigHandler().uiColor());
    m_spinner->start();

    m_waitLabel = new QLabel(tr("Uploading Image"));
    m_waitLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_waitLabel->setCursor(QCursor(Qt::IBeamCursor));

    m_vLayout = new QVBoxLayout();
    setLayout(m_vLayout);

    m_vLayout->addWidget(m_spinner, 0, Qt::AlignHCenter);
    m_vLayout->addWidget(m_waitLabel);

    setAttribute(Qt::WA_DeleteOnClose);
}

LoadSpinner* ImgUploaderBase::spinner()
{
    return m_spinner;
}

const QUrl& ImgUploaderBase::imageURL()
{
    return m_imageURL;
}

void ImgUploaderBase::setImageURL(const QUrl& imageURL)
{
    m_imageURL = imageURL;
}

const QPixmap& ImgUploaderBase::pixmap()
{
    return m_pixmap;
}

void ImgUploaderBase::setPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
}

NotificationWidget* ImgUploaderBase::notification()
{
    return m_notification;
}

void ImgUploaderBase::setErrorText(const QString &errorText)
{
    m_errorText = errorText;
}

void ImgUploaderBase::showPostUploadDialog()
{
    m_waitLabel->deleteLater();
    spinner()->deleteLater();

    m_notification = new NotificationWidget();

    auto* imageLabel = new ImageLabel();
    imageLabel->setScreenshot(m_pixmap);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(imageLabel,
            &ImageLabel::dragInitiated,
            this,
            &ImgUploaderBase::startDrag);
    m_vLayout->addWidget(imageLabel);
    m_vLayout->addWidget(m_notification, 0);

    QHBoxLayout* hLayout = new QHBoxLayout();
    m_vLayout->addLayout(hLayout);

    m_copyUrlButton = new QPushButton(tr("Copy URL"));
    m_openUrlButton = new QPushButton(tr("Open URL"));
    m_toClipboardButton = new QPushButton(tr("Image to Clipboard."));
    m_saveToFilesystemButton = new QPushButton(tr("Save image"));

    hLayout->addWidget(m_copyUrlButton);
    hLayout->addWidget(m_openUrlButton);
    hLayout->addWidget(m_toClipboardButton);
    hLayout->addWidget(m_saveToFilesystemButton);

    connect(
      m_copyUrlButton, &QPushButton::clicked, this, &ImgUploaderBase::copyURL);
    connect(
      m_openUrlButton, &QPushButton::clicked, this, &ImgUploaderBase::openURL);
    connect(m_toClipboardButton,
            &QPushButton::clicked,
            this,
            &ImgUploaderBase::copyImage);

    QObject::connect(m_saveToFilesystemButton,
                     &QPushButton::clicked,
                     this,
                     &ImgUploaderBase::saveScreenshotToFilesystem);
}

void ImgUploaderBase::showUploadErrorDialog()
{
    m_waitLabel->deleteLater();
    spinner()->deleteLater();

    QIcon errorIcon = QMessageBox::standardIcon(QMessageBox::Critical);
    QPixmap pixmap = errorIcon.pixmap(errorIcon.actualSize(QSize(32, 32)));

    QLabel *iconLabel = new QLabel;
    iconLabel->setPixmap(pixmap);

    QLabel *textLabel = new QLabel(m_errorText);
    textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    textLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QHBoxLayout *errorHLayout = new QHBoxLayout;
    errorHLayout->addWidget(iconLabel, 0);
    errorHLayout->addWidget(textLabel, 1, Qt::AlignLeft);

    m_vLayout->addLayout(errorHLayout);

    //

    m_notification = new NotificationWidget();

    auto* imageLabel = new ImageLabel();
    imageLabel->setScreenshot(m_pixmap);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(imageLabel,
            &ImageLabel::dragInitiated,
            this,
            &ImgUploaderBase::startDrag);
    m_vLayout->addWidget(imageLabel);
    m_vLayout->addWidget(m_notification, 0);

    QHBoxLayout* buttonsHLayout = new QHBoxLayout();
    m_vLayout->addLayout(buttonsHLayout);

    m_toClipboardButton = new QPushButton(tr("Image to Clipboard."));
    m_saveToFilesystemButton = new QPushButton(tr("Save image"));

    buttonsHLayout->addWidget(m_toClipboardButton);
    buttonsHLayout->addWidget(m_saveToFilesystemButton);

    connect(m_toClipboardButton,
            &QPushButton::clicked,
            this,
            &ImgUploaderBase::copyImage);

    QObject::connect(m_saveToFilesystemButton,
                     &QPushButton::clicked,
                     this,
                     &ImgUploaderBase::saveScreenshotToFilesystem);
}

void ImgUploaderBase::startDrag()
{
    auto* mimeData = new QMimeData;
    mimeData->setUrls(QList<QUrl>{ m_imageURL });
    mimeData->setImageData(m_pixmap);

    auto* dragHandler = new QDrag(this);
    dragHandler->setMimeData(mimeData);
    dragHandler->setPixmap(m_pixmap.scaled(
      256, 256, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    dragHandler->exec();
}


void ImgUploaderBase::openURL()
{
    bool successful = QDesktopServices::openUrl(m_imageURL);
    if (!successful) {
        m_notification->showMessage(tr("Unable to open the URL."));
    }
}

void ImgUploaderBase::copyURL()
{
    ScreenLoadDaemon::copyToClipboard(m_imageURL.toString());
    m_notification->showMessage(tr("URL copied to clipboard."));
}

void ImgUploaderBase::copyImage()
{
    ScreenLoadDaemon::copyToClipboard(m_pixmap);
    m_notification->showMessage(tr("Screenshot copied to clipboard."));
}

//void ImgUploaderBase::deleteCurrentImage()
//{
//    History history;
//    HistoryFileName unpackFileName = history.unpackFileName(m_currentImageName);
//    deleteImage(unpackFileName.file, unpackFileName.token);
//}

void ImgUploaderBase::saveScreenshotToFilesystem()
{
    if (!saveToFilesystemGUI(m_pixmap)) {
        m_notification->showMessage(
          tr("Unable to save the screenshot to disk."));
        return;
    }
    m_notification->showMessage(tr("Screenshot saved."));
}
