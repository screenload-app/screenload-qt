#pragma once

#include "src/tools/imgupload/storages/imguploaderbase.h"
#include <QUrl>
#include <QWidget>
#include "httpserver.h"

class QNetworkReply;
class QNetworkAccessManager;
class QUrl;

class DwnldrUploader : public ImgUploaderBase
{
    Q_OBJECT
public:
    explicit DwnldrUploader(const QPixmap& capture, QWidget* parent = nullptr);
    void deleteImage(const QString& fileName, const QString& deleteToken);

private slots:
    void onCodeReceived(const QString& code);
    void handleReply(QNetworkReply* reply);

private:
    void upload();

private:
    QNetworkAccessManager* m_NetworkAM;
    HttpServer *m_httpServer;
};
