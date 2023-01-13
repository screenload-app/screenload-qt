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

static QString getPercentEncodedRedirectUrl()
{
    QUrl redirectUrl(kDwnldrLocalHostUrl);
    redirectUrl.setPort(kDwnldrLocalPort);
    QString redirectUrlValue = redirectUrl.toString();

    QByteArray encodedUrlBytes = QUrl::toPercentEncoding(redirectUrlValue);
    QString encodedUrl = QString::fromUtf8(encodedUrlBytes);

    return encodedUrl;
}

static QString getUserAgent()
{
    QString userAgent = QString(kDwnldrUserAgentTemplate).arg(
                "3.0",
                "Qt");

    return userAgent;
}

static QUrl getUploadUrl()
{
    QUrl uploadUrl("https://download.ru/f");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("locale", "en");

    ConfigHandler configHandler;

    if (configHandler.dwnldrShareImage())
        urlQuery.addQueryItem("shared", "true");

//    if (configHandler.dwnldrUseAnonymousAccess())
//        urlQuery.addQueryItem("anonym_key", kDwnldrAnonymousKey);

    uploadUrl.setQuery(urlQuery);

    return uploadUrl;
}

DwnldrUploader::DwnldrUploader(const QPixmap& capture, QWidget* parent)
  : ImgUploaderBase(capture, parent)
{
    m_networkAM = new QNetworkAccessManager(this);
    m_authHttpServer = new DwnldrAuthHttpServer(kDwnldrLocalPort, this);

    connect(m_networkAM, &QNetworkAccessManager::finished, this, &DwnldrUploader::handleReply);
    connect(m_authHttpServer, &DwnldrAuthHttpServer::authorizationCodeReceived, this, &DwnldrUploader::onAuthorizationCodeReceived);

    new QShortcut(Qt::Key_Escape, this, SLOT(close()));
}

void DwnldrUploader::onAuthorizationCodeReceived(const QString& authorizationCode)
{
    requestAccessToken(authorizationCode);
}

void DwnldrUploader::handleReply(QNetworkReply* reply)
{
    QUrl url = reply->url();

    QByteArray responseBytes = reply->readAll();
    QJsonDocument response = QJsonDocument::fromJson(responseBytes);
    QJsonObject json = response.object();

    if (0 == QUrl(kDwnldrOAuthTokenUrl).path().compare(url.path())) // сервер вернул access_token или ошибку.
    {
        QString error = json["error"].toString();

        if (!error.isNull())
        {
            QString errorDescription = json["error_description"].toString();

            if (errorDescription.isNull())
                errorDescription = error;

            setInfoLabelText(errorDescription);
        }
        else
        {
            QString accessToken = json["access_token"].toString();

            // Сохраняем accessToken.
            ConfigHandler().setDwnldrAccessToken(accessToken);

            uploadFile(accessToken);
            return;
        }
    }
    else // сервер вернул результат загрузки файла или ошибку.
    {
        QString errorMessage = json["message"].toString();

        if (!errorMessage.isNull())
        {
            if (0 == QString::compare("unauthorized_access", errorMessage, Qt::CaseInsensitive))
            {
                authorizeViaBrowser();
                return;
            }
            else
                setInfoLabelText(errorMessage);
        }
        else
        {
            // TODO: Убрать хардкод.
            //QString imagePath = json.take("object")["secure_url"].toString();
            QString imagePath = QString("/f/%1").arg(json.take("object")["id"].toString());

            QUrl imageUrl(imagePath);
            imageUrl.setScheme("https");
            imageUrl.setHost("download.ru");

            setImageURL(imageUrl);

            // save image to history
            QString fileName = imagePath.replace("/", "$") + ".png";
            History history;
            m_currentImageName = history.packFileName(kDwnldrStorageName, nullptr, fileName);
            history.save(pixmap(), m_currentImageName);

            emit uploadOk(imageURL());
        }
    }

    spinner()->deleteLater();
}

void DwnldrUploader::upload()
{
    ConfigHandler configHandler;

    QString accessToken = configHandler.dwnldrAccessToken();

    if (accessToken.isEmpty())
        authorizeViaBrowser();
    else
        uploadFile(accessToken);
}

void DwnldrUploader::authorizeViaBrowser()
{
    QUrl url(QStringLiteral("https://download.ru/oauth/authorize"));

    // Открываем браузер для получения authorizationCode.
    QString encodedRedirectUrl = getPercentEncodedRedirectUrl();

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("client_id", kDwnldrClientId);
    urlQuery.addQueryItem("response_type", "code");
    urlQuery.addQueryItem("state", "downloadru");
    urlQuery.addQueryItem("redirect_uri", encodedRedirectUrl);

    url.setQuery(urlQuery);

    QDesktopServices::openUrl(url);
}

void DwnldrUploader::requestAccessToken(const QString& authorizationCode)
{
    // Инициируем запрос access_token.
    QUrl oAuthTokenUrl(kDwnldrOAuthTokenUrl);

    QNetworkRequest request(oAuthTokenUrl);

    QString userAgent = getUserAgent();

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);

    QString encodedRedirectUrl = getPercentEncodedRedirectUrl();

    QString body = QString("grant_type=%1&code=%2&client_id=%3&client_secret=%4&redirect_uri=%5").arg(
                "authorization_code",
                authorizationCode,
                kDwnldrClientId,
                kDwnldrClientSecret,
                encodedRedirectUrl);

    QByteArray byteArray = body.toUtf8();

    m_networkAM->post(request, byteArray);
}

// Выгрузка файла на сервер download.ru.
void DwnldrUploader::uploadFile(const QString& accessToken)
{
    QUrl uploadUrl = getUploadUrl();

    QNetworkRequest request(uploadUrl);

    QString userAgent = getUserAgent();

    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    request.setRawHeader("Authorization",  QStringLiteral("Bearer %1").arg(accessToken).toUtf8());

    QString description = FileNameHandler().parsedPattern();

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"filename\"; filename=\"" + description +  ".png\""));

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap().save(&buffer, "PNG");

    imagePart.setBody(byteArray);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->append(imagePart);

    QString uuidValue = QUuid::createUuid().toString().remove("-");
    QString boundaryName = QString("----------%1").arg(uuidValue);
    QByteArray boundaryNameBytes = boundaryName.toUtf8();

    multiPart->setBoundary(boundaryNameBytes);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundaryName);

    QNetworkReply *reply = m_networkAM->post(request, multiPart);
    multiPart->setParent(reply); // delete the multiPart with the reply
}

void DwnldrUploader::deleteImage(const QString& fileName,
                                const QString& deleteToken)
{
    notification()->showMessage(tr("Unable to open the URL."));
    emit deleteOk();
}
