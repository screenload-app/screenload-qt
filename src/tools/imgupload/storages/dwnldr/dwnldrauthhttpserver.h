#pragma once

#include <QObject>
#include <QTNetwork>

class QTcpServer;
class QTcpSocket;

class DwnldrAuthHttpServer : public QObject
{
    Q_OBJECT
public:
    explicit DwnldrAuthHttpServer(const int port, QObject *parent);

signals:
    void codeReceived(const QString& code);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *m_tcpServer;
};
