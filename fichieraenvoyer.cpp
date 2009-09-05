#include "fichieraenvoyer.h"
#include "fenprincipale.h"

FichierAEnvoyer::FichierAEnvoyer(QString url, QString nom, QString demandeur) {
    this->url = url;
    this->nom = nom;
    this->demandeur = demandeur;
    tailleMorceauxFichiers = 262144; // 256 KiB

    fichier = new QFile(url);
    if(!fichier->open(QIODevice::ReadOnly)) {
        qDebug() << "FichierAEnvoyer.cpp]Erreur d'ouverture du fichier : " << url;
        return;
    }

    this->tailleDuFichier = fichier->size();
    this->nombreDePaquets = (tailleDuFichier + tailleMorceauxFichiers -1 ) / tailleMorceauxFichiers;
    this->bytesRestantsAEnvoyer = tailleDuFichier;
    this->bytesParPaquets = (bytesRestantsAEnvoyer + tailleMorceauxFichiers-1) /  tailleMorceauxFichiers;
    qDebug() << "[FichierAEnvoyer]Envoi de fichier] Le fichier fait " << tailleDuFichier << ", il y a " << nombreDePaquets << " paquets a envoyer";

    this->nombreDuPaquet = 1;
}

FichierAEnvoyer::~FichierAEnvoyer() {
    fichier->close();
    delete fichier;
    fichier = 0;

    qDebug() << "[FichierAEnvoyer]"<<nom<<" envoyé et fermé";
}


bool FichierAEnvoyer::envoyerMorceau(QTcpSocket *socket) {
    QByteArray paquet, morceaux;
    morceaux = fichier->read(this->tailleMorceauxFichiers);
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint32)0;
    out << (quint32)DT_FileData << (quint64)this->nombreDuPaquet << (quint64)this->nombreDePaquets << this->demandeur << this->nom;
    out << morceaux;
    out.device()->seek(0);

    out << (quint32) (paquet.size() - sizeof(quint32));
    socket->write(paquet);
    this->bytesRestantsAEnvoyer -= morceaux.size();

    qDebug() << "[FichierAEnvoyer]Envoi du morceau "<< nombreDuPaquet << " sur "<< nombreDePaquets << "; taille :" << morceaux.size();
    this->nombreDuPaquet++;

    if(this->nombreDuPaquet > this->nombreDePaquets)
        return false;
    else return true;
}
