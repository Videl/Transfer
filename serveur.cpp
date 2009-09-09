#include "serveur.h"

Serveur::Serveur()
{
}

void Serveur::demarrageReseau() {
    serveur = new QTcpServer(this);
    if(!serveur->listen(QHostAdress::Any, 50886)) {
        qDebug() << "Serveur pas démarré";
    }else { qDebug() << "Serveur démarré"; }

    connect(serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
}

void Serveur::nouvelleConnexion() {
    QTcpSocket *newClient = new QTcpSocket(serveur->nextPendingConnection());
    clients << newClient;
}
