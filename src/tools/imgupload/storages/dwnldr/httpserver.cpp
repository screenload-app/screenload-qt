#include "httpserver.h"
#include <QDateTime>

HttpServer::HttpServer(QObject *parent)
    : QObject(parent)
{
   m_tcpServer = new QTcpServer(this);
   connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
   m_tcpServer->listen(QHostAddress::Any, 8087);
}

HttpServer::~HttpServer()
{
    delete m_tcpServer;
}

void HttpServer::onNewConnection()
{
    QTcpSocket* connection = m_tcpServer->nextPendingConnection();
    int descriptor = connection->socketDescriptor();
    m_connections[descriptor] = connection;
    connect(m_connections[descriptor], SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void HttpServer::onReadyRead()
{
    QTcpSocket* connection = (QTcpSocket*)sender();

    QByteArray lineBytes = connection->readLine();
    QString firstLine = QString(lineBytes);

    int startIndex = 4; // GET_
    int spaceIndex = firstLine.indexOf(" ");

    firstLine = firstLine.remove(startIndex, spaceIndex - startIndex);

    QUrl url(firstLine);
    QUrlQuery query(url);

    int descriptor = connection->socketDescriptor();

    if (query.hasQueryItem("code"))
    {
        QString code = query.queryItemValue("code");

        QTextStream os(connection);

        os << "HTTP/1.0 200 OK\r\n"
              "Content-Type: text/html; charset=\"utf-8\"\r\n"
              "\r\n"
              "<h1>Successful Authorization!</h1>";

        emit codeReceived(code);
    }

    connection->close();
    m_connections.remove(descriptor);

    // TODO emit...
}
