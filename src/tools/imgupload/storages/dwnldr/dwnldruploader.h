#pragma once

#include "src/tools/imgupload/storages/imguploaderbase.h"
#include <QUrl>
#include <QWidget>
#include "dwnldrauthhttpserver.h"

class QNetworkReply;
class QNetworkAccessManager;
class QUrl;

constexpr char* kDwnldrLocalHostUrl = "http://localhost/";
constexpr int kDwnldrLocalPort = 8087;

constexpr char* kDwnldrUserAgentTemplate = "ScreenLoad/%1 %2";

constexpr char* kDwnldrOAuthTokenUrl = "https://download.ru/oauth/token";

constexpr char* kDwnldrClientId = "0524f0e89a3fd0912b1ed4484e21cde8c02e5e5625fe070ba65e5ff2deaf78e2";
constexpr char* kDwnldrClientSecret = "7f090e18d58cf2983f071b3f5afb544e0aaba1e8ce80f83e9a9bfb9ee9b917f5";

class DwnldrUploader : public ImgUploaderBase
{
    Q_OBJECT
public:
    explicit DwnldrUploader(const QPixmap& capture, QWidget* parent = nullptr);

private slots:
    void onAuthorizationCodeReceived(const QString& authorizationCode);
    void handleReply(QNetworkReply* reply);

private:
    void upload();
    void deleteImage(const QString& fileName, const QString& deleteToken);
    void authorizeViaBrowser();
    void requestAccessToken(const QString& authorizationCode);
    void uploadFile(const QString& accessToken);

private:
    QNetworkAccessManager* m_networkAM;
    DwnldrAuthHttpServer *m_authHttpServer;
};
