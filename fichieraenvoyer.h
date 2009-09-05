#ifndef FICHIERAENVOYER_H
#define FICHIERAENVOYER_H

#include <QtNetwork>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QDataStream>

class FenPrincipale;

class FichierAEnvoyer
{
public:
    FichierAEnvoyer(QString url, QString nom, QString demandeur);
    ~FichierAEnvoyer();

    // Renvoie true s'il reste quelque chose à envoyer
    // false sinon
    bool envoyerMorceau(QTcpSocket *socket);

private:
    QFile *fichier;
    QString url;
    QString nom, demandeur;
    // nom = nom du fichier

    quint64 nombreDuPaquet, nombreDePaquets, tailleDuFichier;
    quint64 bytesRestantsAEnvoyer, bytesParPaquets;
    int tailleMorceauxFichiers;
};

#endif // FICHIERAENVOYER_H
