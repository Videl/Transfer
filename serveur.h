#ifndef SERVEUR_H
#define SERVEUR_H

#include <QObject>
#include <QtNetwork>

class Serveur : public QObject
{
    Q_OBJECT
public:
    Serveur();

public slots:
    void nouvelleConnexion();
    void demarrageReseau();
    void arreterServeur();
    void deconnexion();

private:
    QTcpServer *serveur;
    QList<QTcpSocket *> clients;

};

#endif // SERVEUR_H
