#include "dwnldruploader.h"
#include "src/utils/confighandler.h"
#include "src/utils/filenamehandler.h"
#include "src/utils/history.h"
#include "src/widgets/loadspinner.h"
#include "src/widgets/notificationwidget.h"
#include <QBuffer>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QShortcut>
#include <QUrlQuery>
#include <QMessageBox>

DwnldrUploader::DwnldrUploader(const QPixmap& capture, QWidget* parent)
  : ImgUploaderBase(capture, parent)
{
    m_NetworkAM = new QNetworkAccessManager(this);
    m_httpServer = new HttpServer(this);

    connect(m_NetworkAM, &QNetworkAccessManager::finished, this, &DwnldrUploader::handleReply);
    connect(m_httpServer, &HttpServer::codeReceived, this, &DwnldrUploader::onCodeReceived);
}

void DwnldrUploader::onCodeReceived(const QString& code)
{
    QUrl url("https://download.ru/oauth/token");

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, "ScreenLoad");

    QString body = QString("grant_type=%1&code=%2&client_id=%3&client_secret=%4&redirect_uri=%5").arg(
                "authorization_code",
                code,
                "0524f0e89a3fd0912b1ed4484e21cde8c02e5e5625fe070ba65e5ff2deaf78e2",
                "7f090e18d58cf2983f071b3f5afb544e0aaba1e8ce80f83e9a9bfb9ee9b917f5",
                "http%3a%2f%2flocalhost%3a8087%2f");

    QByteArray byteArray = body.toUtf8();

    m_NetworkAM->post(request, byteArray);
}

void DwnldrUploader::handleReply(QNetworkReply* reply)
{
    QUrl url = reply->url();

    if (QUrl("https://download.ru/oauth/token") == url)
    {
        QByteArray responseBytes = reply->readAll();
        QString responseText(responseBytes);

        QJsonDocument response = QJsonDocument::fromJson(responseBytes);
        QJsonObject json = response.object();
        QString accessToken = json["access_token"].toString();


        QUrl uploadUrl("https://download.ru/f?locale=en&shared=true");

        QNetworkRequest request(uploadUrl);

        request.setHeader(QNetworkRequest::UserAgentHeader, "ScreenLoad");
        request.setRawHeader("Authorization",  QStringLiteral("Bearer %1").arg(accessToken).toUtf8());


        QString description = FileNameHandler().parsedPattern();

        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart imagePart;
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\"; filename=\"" + description +  ".png\""));

        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        pixmap().save(&buffer, "PNG");

        imagePart.setBody(byteArray);

        multiPart->append(imagePart);

        auto b = multiPart->boundary();
        QString bv(b);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + bv);

        m_NetworkAM->post(request, multiPart);
    }

    spinner()->deleteLater();
    emit uploadOk(imageURL());

//    spinner()->deleteLater();
//    m_currentImageName.clear();
//    if (reply->error() == QNetworkReply::NoError) {
//        QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
//        QJsonObject json = response.object();
//        QJsonObject data = json[QStringLiteral("data")].toObject();
//        setImageURL(data[QStringLiteral("link")].toString());

//        auto deleteToken = data[QStringLiteral("deletehash")].toString();

//        // save history
//        m_currentImageName = imageURL().toString();
//        int lastSlash = m_currentImageName.lastIndexOf("/");
//        if (lastSlash >= 0) {
//            m_currentImageName = m_currentImageName.mid(lastSlash + 1);
//        }

//        // save image to history
//        History history;
//        m_currentImageName =
//          history.packFileName("imgur", deleteToken, m_currentImageName);
//        history.save(pixmap(), m_currentImageName);

//        emit uploadOk(imageURL());
//    } else {
//        setInfoLabelText(reply->errorString());
//    }
//    new QShortcut(Qt::Key_Escape, this, SLOT(close()));
}

void DwnldrUploader::upload()
{
    //QMessageBox::information(nullptr, "Point1", "Point1", "ok");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("client_id", "0524f0e89a3fd0912b1ed4484e21cde8c02e5e5625fe070ba65e5ff2deaf78e2");
    urlQuery.addQueryItem("response_type", "code");
    urlQuery.addQueryItem("state", "downloadru");
    urlQuery.addQueryItem("redirect_uri", "http%3a%2f%2flocalhost%3a8087%2f");

    QUrl url(QStringLiteral("https://download.ru/oauth/authorize"));
    url.setQuery(urlQuery);

    QDesktopServices::openUrl(url);

//    QMessageBox::information(nullptr, "Point1", "Point1", "ok");

//    QByteArray byteArray;
//    QBuffer buffer(&byteArray);
//    pixmap().save(&buffer, "PNG");

//    QUrl url(QStringLiteral("https://google.com/"));
//    QNetworkRequest request(url);

//    m_NetworkAM->post(request, byteArray);
}

void DwnldrUploader::deleteImage(const QString& fileName,
                                const QString& deleteToken)
{
//    Q_UNUSED(fileName)
//    bool successful = QDesktopServices::openUrl(
//      QUrl(QStringLiteral("https://imgur.com/delete/%1").arg(deleteToken)));
//    if (!successful) {
//        notification()->showMessage(tr("Unable to open the URL."));
//    }

//    emit deleteOk();
}
