#include "fichieraenvoyer.h"
#include "fenprincipale.h"

FichierAEnvoyer::FichierAEnvoyer(QWidget* parent, QString url, QString nom, QString demandeur) : QDialog(parent) {
    this->setModal(false);
    this->setWindowTitle("Envoi de fichier");
    QGridLayout *layout = new QGridLayout(this);
    pause = false;
    stop = false;

    this->url = url;
    this->nom = nom;
    this->demandeur = demandeur;
    tailleMorceauxFichiers = 262144; // 256 KiB
    QLabel *lab_vitesse = new QLabel("[Vitesse inconnu]");

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

    progression = new QProgressBar(this);
    progression->setMaximum(tailleDuFichier);
    progression->setValue(tailleDuFichier);
    progression->setFormat("%p%");


    QPushButton *boutonFermer = new QPushButton("Fermer", this);
    boutonPause = new QPushButton("Pause", this);
    boutonPause->setCheckable(true);

    layout->addWidget(new QLabel("<strong>"+nom+"</strong> dans <br />"+url), 0, 0, 1, 0);
    layout->addWidget(lab_vitesse, 1, 0, 1, 0);
    layout->addWidget(progression, 2, 0, 1, 0);
    layout->addWidget(boutonPause, 3, 1);
    layout->addWidget(boutonFermer, 3, 2);

    this->setLayout(layout);
    this->show();

    connect(boutonPause, SIGNAL(released()), this, SLOT(pushPause()));
}

void FichierAEnvoyer::pushPause() {
    if(boutonPause->isChecked() == true) {
        pause = true;
    }
    else if(boutonPause->isChecked() == false) {
        pause = false;
    }
}

FichierAEnvoyer::~FichierAEnvoyer() {
    fichier->close();
    delete fichier;
    fichier = 0;

    qDebug() << "[FichierAEnvoyer]Fichier fermé";
}


bool FichierAEnvoyer::envoyerMorceau(QTcpSocket *socket) {
    if(!pause) {
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

        progression->setValue(bytesRestantsAEnvoyer);
        if(this->nombreDuPaquet > this->nombreDePaquets)
            return false;
        else return true;
    } else if(this->nombreDuPaquet > this->nombreDePaquets) return false; else return true;
}
