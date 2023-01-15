#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class QTcpServer;
class QTcpSocket;

class DwnldrAuthHttpServer : public QObject
{
    Q_OBJECT
public:
    explicit DwnldrAuthHttpServer(const int port, QObject *parent);

signals:
    void authorizationCodeReceived(const QString& authorizationCode);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *m_tcpServer;
};
