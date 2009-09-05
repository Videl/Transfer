#ifndef FICHIERAENVOYER_H
#define FICHIERAENVOYER_H

#include <QtNetwork>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QDialog>
#include <QGridLayout>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QAbstractButton>


class FenPrincipale;

class FichierAEnvoyer : public QDialog {
    Q_OBJECT
public:
    FichierAEnvoyer(QWidget* parent, QString url, QString nom, QString demandeur);
    ~FichierAEnvoyer();

    // Renvoie true s'il reste quelque chose à envoyer
    // false sinon
    bool envoyerMorceau(QTcpSocket *socket);

private slots:
    void pushPause();
private:
    QFile *fichier;
    QString url;
    QString nom, demandeur;
    // nom = nom du fichier

    quint64 nombreDuPaquet, nombreDePaquets, tailleDuFichier;
    quint64 bytesRestantsAEnvoyer, bytesParPaquets;
    int tailleMorceauxFichiers;

    QProgressBar *progression;

    bool pause;
    QPushButton *boutonPause;
    bool stop;
};

#endif // FICHIERAENVOYER_H
