// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#pragma once

#include <QUrl>
#include <QWidget>

class QNetworkReply;
class QNetworkAccessManager;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class LoadSpinner;
class QPushButton;
class QUrl;
class NotificationWidget;

class ImgUploaderBase : public QWidget
{
    Q_OBJECT
public:
    explicit ImgUploaderBase(const QPixmap& capture, QWidget* parent = nullptr);

    QString m_currentImageName;

    LoadSpinner* spinner();

    const QUrl& imageURL();
    void setImageURL(const QUrl&);

    void setErrorText(const QString &newErrorText);

    const QPixmap& pixmap();
    void setPixmap(const QPixmap&);

    NotificationWidget* notification();
//    virtual void deleteImage(const QString& fileName,
//                             const QString& deleteToken) = 0;
    virtual void upload() = 0;

signals:
    void uploadOk(const QUrl& url);
    void uploadFailed();
    void deleteOk();

public slots:
    void showPostUploadDialog();
    void showUploadErrorDialog();

private slots:
    void startDrag();
    void openURL();
    void copyURL();
    void copyImage();
//    void deleteCurrentImage();
    void saveScreenshotToFilesystem();

private:
    QPixmap m_pixmap;

    QVBoxLayout* m_vLayout;
    // loading
    QLabel* m_waitLabel;
    LoadSpinner* m_spinner;
    // uploaded
    QPushButton* m_openUrlButton;
    //QPushButton* m_openDeleteUrlButton;
    QPushButton* m_copyUrlButton;
    QPushButton* m_toClipboardButton;
    QPushButton* m_saveToFilesystemButton;
    QUrl m_imageURL;
    QString m_errorText;
    NotificationWidget* m_notification;
};
