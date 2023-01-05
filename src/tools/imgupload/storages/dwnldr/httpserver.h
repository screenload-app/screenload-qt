#pragma once

#include <QObject>
#include <QTNetwork>

class QTcpServer;
class QTcpSocket;

class HttpServer : public QObject
{
    Q_OBJECT
public:
    HttpServer(QObject *parent);
    ~HttpServer();

signals:
    void codeReceived(const QString& code);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *m_tcpServer;
    QMap<int, QTcpSocket*> m_connections;
};
