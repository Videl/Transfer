#ifndef CONNEXION_H
#define CONNEXION_H

#include <QtGui>
#include <QtNetwork>
#include <QQueue>
//#include <QTime>


class FenConnexion;
class FenPrincipale;
class VWidget;
class VDownload;
class FenOptions;
class FichierAEnvoyer;


class Connexion : public QObject {
    Q_OBJECT
public:
    Connexion(FenPrincipale *fenPr, FenOptions* opt);
    void envoyerMessage(QString texte, int type = 1);
    void setFenConnexion(FenConnexion* fenCo);
    void envoyerInfoFile(QString nom, QString taille, QString typeMime);
    bool getConnected();
    void demanderFichier(QString fileName, QString owner);
    void envoyerFichier(QString fileUrl, QString nom, QString demandeur);
    void traiterMessage(quint32, QByteArray contenu);


private slots:
    void tentativeConnexion();
    void connecte();
    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError erreur);
    void donneesRecues();
    void envoyerMorceau();

private:
    bool isConnected;
    QTcpSocket *socket;
    FenConnexion *fenetreConnexion;
    FenPrincipale *fenetre;
    FenOptions *options;

    int tailleMessage;
    int tailleMorceauxFichiers;
    quint32 taille;

    QList<VDownload * > listeDL;
    QQueue<FichierAEnvoyer *> fichiersAEnvoyer;

    int tour;

};

#endif // CONNEXION_H
