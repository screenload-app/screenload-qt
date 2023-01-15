#include "dwnldrauthhttpserver.h"
#include <QDateTime>
#include <QResource>
#include <QUrl>
#include <QUrlQuery>
#include <QString>

DwnldrAuthHttpServer::DwnldrAuthHttpServer(const int port, QObject *parent)
    : QObject(parent)
{
   m_tcpServer = new QTcpServer(this);
   connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
   m_tcpServer->listen(QHostAddress::LocalHost, port);
}

void DwnldrAuthHttpServer::onNewConnection()
{
    QTcpSocket* connection = m_tcpServer->nextPendingConnection();
    connect(connection, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

static const QString readResourceText(const QString& resourceFileName)
{
    QResource resource(resourceFileName);
    QByteArray bytes = resource.uncompressedData();
    const QString text(bytes);

    return text;
}

static void writeHttpResponse(QTcpSocket* connection, const QString& statusCode, const QString& contentType, const QString& text)
{
    QTextStream textStream(connection);
    textStream.setCodec("utf-8");

    textStream << "HTTP/1.0 " << statusCode << "\r\n"
                  "Content-Type: " << contentType << "; charset=\"utf-8\"\r\n"
                  "\r\n"
                  << text.toUtf8();
}

void DwnldrAuthHttpServer::onReadyRead()
{
    QTcpSocket* connection = (QTcpSocket*)sender();

    QByteArray lineBytes = connection->readLine();
    QString firstLine = QString(lineBytes);

    int startIndex = 4; // GET_
    int spaceIndex = firstLine.indexOf(" ", startIndex);

    firstLine = firstLine.mid(startIndex, spaceIndex - startIndex);

    QUrl url(firstLine);

    QString path = url.path();

    if (0 == QString::compare(path, "/", Qt::CaseInsensitive))
    {
        QUrlQuery query(url);

        if (query.hasQueryItem("code"))
        {
            const QString resourceText = readResourceText(":/files/SuccessfulAuthorization.html");

            const QString responseText = QString(resourceText).arg(
                        tr("Successful Authorization!"),
                        tr("Successful Authorization!"),
                        tr("You have successfully authorize <strong class=\"text-info\">ScreenLoad</strong> to use your account!"),
                        tr("This window will be closed automatically within 5 seconds."));

            writeHttpResponse(connection, "200 OK", "text/html", responseText);

            QString authorizationCode = query.queryItemValue("code");
            emit authorizationCodeReceived(authorizationCode);
        }
    }
    else if (0 == QString::compare(path, "/main.css", Qt::CaseInsensitive))
    {
        const QString responseText = readResourceText(":/files/main.css");
        writeHttpResponse(connection, "200 OK", "text/css", responseText);
    }

    connection->close();
}
