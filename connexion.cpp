#include "connexion.h"
#include "fenprincipale.h"
#include "fenconnexion.h"
#include "vwidget.h"
#include "vdownload.h"
#include "fenoptions.h"
#include "fichieraenvoyer.h"

Connexion::Connexion(FenPrincipale *fenPr, FenOptions* opt) : fenetre(fenPr), options(opt){
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(connecte()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

    connect(socket, SIGNAL(readyRead()), this, SLOT(doneesRecues()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(envoyerMorceau()));

    tailleMessage = 0;
    tailleMorceauxFichiers = 262144; // 256 KiB
}

void Connexion::setFenConnexion(FenConnexion* fenCo) {
    fenetreConnexion = fenCo;
}

void Connexion::tentativeConnexion() {
    if(!fenetre->getPseudo().isEmpty()) {
        fenetre->addStringlisteMessages("<em>Tentative de connexion en cours...</em>");
        fenetreConnexion->switchState_boutonConnexion(false);

        socket->abort();
        socket->connectToHost(fenetreConnexion->getServIp(), fenetreConnexion->getServPort());

    } else fenetre->addStringlisteMessages("<em>Vous devez utiliser un pseudo !</em>");
}

bool Connexion::getConnected() {
    return this->isConnected;
}

void Connexion::envoyerInfoFile(QString nom, QString taille, QString typeMime) {
    int type = DT_InfoFile;
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    out << (quint32)0;
    out << (quint32)type << nom << taille << typeMime;
    out.device()->seek(0);
    out << (quint32) (paquet.size() - sizeof(quint32));

    socket->write(paquet);
    socket->flush();
}

void Connexion::demanderFichier(QString fileName, QString owner) {
    int type = DT_WantFile;
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    out << (quint32)0;
    out << (quint32)type << fileName << owner;
    out.device()->seek(0);
    out << (quint32) (paquet.size() - sizeof(quint32));

    socket->write(paquet);
    socket->flush();
}

void Connexion::envoyerMessage(QString texte, int type) {
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    out << (quint32)0;
    out << (quint32)type << texte;
    out.device()->seek(0);
    out << (quint32) (paquet.size() - sizeof(quint32));

    socket->write(paquet);
    socket->flush();
    //qDebug() << "[Connexion]Envoi de paquet type:" << (DataType)type;
}


void Connexion::envoyerFichier(QString fileUrl, QString nom, QString demandeur) {
    //nom = nomDuFichier

    /*QFile *theFile = new QFile(fileUrl);
    if(!theFile->open(QIODevice::ReadOnly))
        qDebug() << "[Connexion]Envoi de fichier] Ouverture raté";

    quint64 tailleDuFichier = theFile->size();
    quint64 nombreDePaquets = (tailleDuFichier + tailleMorceauxFichiers -1 ) / tailleMorceauxFichiers;
    quint64 bytesRestantsAEnvoyer = tailleDuFichier;
    quint64 bytesParPaquets = (bytesRestantsAEnvoyer + tailleMorceauxFichiers-1) /  tailleMorceauxFichiers;
    qDebug() << "[Connexion]Envoi de fichier] Le fichier fait " << tailleDuFichier << ", il y a " << nombreDePaquets << " paquets a envoyer";

    for(quint64 a_numeroDuPaquet = 1; a_numeroDuPaquet <= nombreDePaquets; a_numeroDuPaquet++) {
        
        QByteArray paquet, mesBytes;
        mesBytes = theFile->read(tailleMorceauxFichiers);

        QDataStream out(&paquet, QIODevice::WriteOnly);
        out << (quint32)0;
        out << (quint32)DT_FileData << (quint64)a_numeroDuPaquet << (quint64)nombreDePaquets << demandeur << nom;
        out << mesBytes;
        out.device()->seek(0);
        out << (quint32) (paquet.size() - sizeof(quint32));

        //qDebug() << "[Connexion]Envoi de fichier] BytesToWrite"<< socket->bytesToWrite();

        socket->write(paquet);
        //socket->waitForBytesWritten((int)paquet.size());

        //socket->flush();

        bytesRestantsAEnvoyer -= mesBytes.size();
        //qDebug() << "[Connexion]Envoi de fichier] Envoyé partie numero " << a_numeroDuPaquet << "; taille du paquet :"<< paquet.size();
    }
    theFile->close();
    delete theFile;
    theFile = NULL;*/
    fichiersAEnvoyer << new FichierAEnvoyer(fileUrl, nom, demandeur);
    this->envoyerMorceau();
}

void Connexion::envoyerMorceau() {
    while(socket->bytesToWrite() <= 0 && !fichiersAEnvoyer.isEmpty()) {
        if(!fichiersAEnvoyer.head()->envoyerMorceau(this->socket)) {
            // Si le fichier est terminé, on le retire de la file
            fichiersAEnvoyer.head()->~FichierAEnvoyer();
            fichiersAEnvoyer.dequeue();
        }
    }
}

void Connexion::erreurSocket(QAbstractSocket::SocketError erreur) {
    switch(erreur) // On affiche un message différent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            fenetre->addStringlisteMessages(tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            fenetre->addStringlisteMessages(tr("<em>Le serveur a refusé la connexion. Vérifiez si le serveur est bien lancé. Vérifiez aussi l'IP et le port.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            fenetre->addStringlisteMessages(tr("<em>Le serveur a coupé la connexion.</em>"));
            break;
        default:
            fenetre->addStringlisteMessages(tr("<em>ERREUR : ") + socket->errorString() + tr("</em>"));
    }
    fenetreConnexion->switchState_boutonConnexion(true);
}

void Connexion::deconnecte() {
    this->isConnected = false;
    socket->abort();
    fenetreConnexion->setLabelStatut("Vous êtes déconnecté.");

    fenetreConnexion->switchState_serv_ip(true);
    fenetreConnexion->switchState_serv_port(true);
    fenetreConnexion->switchState_boutonConnexion(true);
    fenetre->setStatusLabel("Déconnecté");
}

void Connexion::connecte() {
    this->isConnected = true;
    fenetre->addStringlisteMessages(tr("<em>Connexion réussie !</em>"));
    envoyerMessage(fenetre->getPseudo(), DT_Pseudo);
    envoyerMessage("", DT_Transferable);
    fenetreConnexion->setLabelStatut("Vous êtes connecté.");

    fenetreConnexion->switchState_boutonConnexion(true);
    fenetreConnexion->switchState_serv_ip(true);
    fenetreConnexion->switchState_serv_port(true);
    fenetre->setStatusLabel("Connecté");
}


void Connexion::doneesRecues() {
    QDataStream in(socket);
    //qDebug() << "Recu";
    int tour = 0;
    do{
        tour++;
        if (taille == 0){
            if (socket->bytesAvailable() < (int)sizeof(quint32))
                 return;
            in >> taille;
        }

        if (socket->bytesAvailable() < taille)
            return;

        // Si on arrive jusqu'à cette ligne, on peut récupérer le message entier
        quint32 typeMessage;
        in >> typeMessage;

        qDebug() << "Connexion]Taille:" << taille;
        QByteArray contenuMessage = socket->read(in.device()->size());


        traiterMessage(typeMessage, contenuMessage);

        taille = 0;
        //qDebug() << "[Connexion]donneesRecues()] reçu type "<< typeMessage;
        qDebug() << "Tour" << tour<< "!";
    }while(socket->bytesAvailable());

/*
    // On traite le type de message
    if(style == DT_Chat) {
        QVariant test;
        in >> messageRecu >> test;
        fenetre->addStringlisteMessages(messageRecu);
       // qDebug() << "[Connexion] reçu message : " << messageRecu;
    }
    else if(style == DT_Pseudo) {
        in >> messageRecu;
        fenetre->actualisationListePseudos(messageRecu);
        //qDebug() << "[Connexion] reçu pseudo : "<< messageRecu;
    }
    else if(style == DT_IsWriting) {
        in >> messageRecu;
        qDebug() << "[Connexion]"<< messageRecu <<" écrit ou arrête";
        //fenetre->actualisationTyping(messageRecu, style);
        // On ne fait rien, l'actualisation des pseudos est une liste des pseudos envoyé par le serveur = moins de problèmes

    } else if(style == DT_StoppedWriting) {
        //fenetre->actualisationTyping(messageRecu, style);
        // On ne fait rien, l'actualisation des pseudos est une liste des pseudos envoyé par le serveur = moins de problèmes

    }
    else if(style == DT_InfoFileResult) {
        in >> messageRecu;
        //qDebug() << "[Connexion] on reçoit les infos d'un fichier";
        QString reponse;
        in  >> reponse;
        fenetre->updateWhereNameIs(messageRecu, "", "", reponse);
        fenetre->setMessage(messageRecu+" : " + reponse, 10000);
    } else if(style == DT_InfoFile) {
        in >> messageRecu;
        //qDebug() << "[Connexion] quelqu'un a envoyé un fichier";
        //fenetre->nouveauFichierAjouteReseau(messageRecu);
        QString un, de, tr, qu;
        in >> un >> de >> tr;
        in >> qu;
        //qDebug() << "[Connexion] " << "messageRecu->" << messageRecu << "un->"<< un << ";" << "de->"<< de << ";" << "tr->"<< tr << ";" << "qu->"<< qu;
        // il faut une extraction en plus sinon il y a un décalage dans les paquets
        fenetre->nouveauFichierAjouteReseau(messageRecu, un, de, tr);
    } else if (style == DT_AskingForFile) {
        in >> messageRecu;
        //messageRecu = celui qui veux le fichier
        QString fileName;
        in >> fileName;
        qDebug() << "[Connexion][AskingForFile]" << messageRecu << " vous demande le fichier "<< fileName;

        fenetre->prepEnvoiFichier(fileName, messageRecu);
    } else if(style == DT_FileData) {
        quint64 nombreDuPaquet = 0, nombreDePaquets;
        QString nomFichier;
        QByteArray lesBytes;
        in >> nombreDuPaquet >> nombreDePaquets >> nomFichier >> lesBytes;
        qDebug() << "[Connexion]Reception de morceau]Recu morceau " << nombreDuPaquet << " sur " << nombreDePaquets << " taille: "<< lesBytes.size();

        if(nombreDuPaquet == 1) {
            //qDebug() << "[Connexion]Reception de fichier]Création du fichier : "<< nomFichier;
            //qDebug() << "[Connexion]Reception de fichier]Le fichier est composé de "<<nombreDePaquets;

            listeDL << new VDownload(fenetre, nomFichier, nombreDePaquets, options);
        }

        int liste = 0;
        for(int i = 0; i < listeDL.size(); i++)
            if(nomFichier == listeDL[i]->getNomFichier())
                liste = i;

        //if(!listeDL[liste]->getFile()) return;
        //qDebug() << "[Connexion] Emplacement et nom du fichier : ["<<liste<<"] "<< listeDL[liste];

        //qDebug() << "[Connexion]Réception de morceaux]Morceaux numéros"<< nombreDuPaquet <<"sur"<< nombreDePaquets<<".";

        //fichier->write(lesBytes);
        listeDL[liste]->addBytes(lesBytes);

        if(nombreDuPaquet >= nombreDePaquets) { // Fichier enregistré
            listeDL[liste]->terminate();
            //fichier->close();
            //delete fichier;

            qDebug() << "[Connexion]Réception de fichier]Fichier enregistré";
        }

    } else { //fenetre->addStringlisteMessages(messageRecu);
        //qDebug() << "[Connexion]Connais pas le type de message : "<< style;
        in.skipRawData(this->tailleMessage - sizeof(this->tailleMessage));
    }*/

    // On remet la taille du message à 0 pour pouvoir recevoir de futurs messages

    //}while(socket->bytesAvailable());
    this->envoyerMorceau();

    /*if(style != DT_FileData) {
        if(!in.atEnd()) {
            qDebug() << "[CLI] IN non vide ( style ="<< style<<")";
            while(!in.atEnd()) {
                QVariant test;
                in >> test;
                qDebug() << "[CLI] Resultat de IN :" << test.toString();
            }
        }
    }*/

}

void Connexion::traiterMessage(quint32 type, QByteArray contenu) {
    QDataStream in(contenu);
    QString messageRecu;
    switch(type)
    {
        case DT_Chat:
        {
            QVariant test;
            in >> messageRecu >> test;
            fenetre->addStringlisteMessages(messageRecu);
            break;
        }
        case DT_Pseudo:
        {
            in >> messageRecu;
            fenetre->actualisationListePseudos(messageRecu);
            break;
        }
        case DT_IsWriting:
        {
            in >> messageRecu;
            qDebug() << "[Connexion]"<< messageRecu <<" écrit ou arrête";
            //fenetre->actualisationTyping(messageRecu, style);
            // On ne fait rien, l'actualisation des pseudos est une liste des pseudos envoyé par le serveur = moins de problèmes
            break;
        }
        case DT_StoppedWriting:
        {
            //fenetre->actualisationTyping(messageRecu, style);
            // On ne fait rien, l'actualisation des pseudos est une liste des pseudos envoyé par le serveur = moins de problèmes
            break;
        }
        case DT_InfoFileResult:
        {
            in >> messageRecu;
            //qDebug() << "[Connexion] on reçoit les infos d'un fichier";
            QString reponse;
            in  >> reponse;
            fenetre->updateWhereNameIs(messageRecu, "", "", reponse);
            fenetre->setMessage(messageRecu+" : " + reponse, 10000);

            break;
        }
        case DT_InfoFile:
        {
            in >> messageRecu;
            //qDebug() << "[Connexion] quelqu'un a envoyé un fichier";
            //fenetre->nouveauFichierAjouteReseau(messageRecu);
            QString un, de, tr, qu;
            in >> un >> de >> tr;
            in >> qu;
            //qDebug() << "[Connexion] " << "messageRecu->" << messageRecu << "un->"<< un << ";" << "de->"<< de << ";" << "tr->"<< tr << ";" << "qu->"<< qu;
            // il faut une extraction en plus sinon il y a un décalage dans les paquets
            fenetre->nouveauFichierAjouteReseau(messageRecu, un, de, tr);

            break;
        }
        case DT_AskingForFile:
        {
            in >> messageRecu;
            //messageRecu = celui qui veux le fichier
            QString fileName;
            in >> fileName;
            qDebug() << "[Connexion][AskingForFile]" << messageRecu << " vous demande le fichier "<< fileName;

            fenetre->prepEnvoiFichier(fileName, messageRecu);

            break;
        }
        case DT_FileData:
        {
            quint64 nombreDuPaquet = 0, nombreDePaquets;
            QString nomFichier;
            QByteArray lesBytes;
            in >> nombreDuPaquet >> nombreDePaquets >> nomFichier >> lesBytes;
            qDebug() << "[Connexion]Reception de morceau]Recu morceau " << nombreDuPaquet << " sur " << nombreDePaquets << " taille: "<< lesBytes.size();

            if(nombreDuPaquet == 1) {
                /*qDebug() << "[Connexion]Reception de fichier]Création du fichier : "<< nomFichier;
                qDebug() << "[Connexion]Reception de fichier]Le fichier est composé de "<<nombreDePaquets;*/

                fenetre->nouvelleFenetreFichier(new VDownload(fenetre, nomFichier, nombreDePaquets, options));
            }

            int liste = 0;
            for(int i = 0; i < listeDL.size(); i++)
                if(nomFichier == fenetre->getListeTelechargement()[i]->getNomFichier())
                    liste = i;

            //if(!listeDL[liste]->getFile()) return;
            //qDebug() << "[Connexion] Emplacement et nom du fichier : ["<<liste<<"] "<< listeDL[liste];

            //qDebug() << "[Connexion]Réception de morceaux]Morceaux numéros"<< nombreDuPaquet <<"sur"<< nombreDePaquets<<".";

            //fichier->write(lesBytes);
            fenetre->getListeTelechargement()[liste]->addBytes(lesBytes);

            if(nombreDuPaquet >= nombreDePaquets) { // Fichier enregistré
                fenetre->getListeTelechargement()[liste]->terminate();
                /*fichier->close();
                delete fichier;*/

                qDebug() << "[Connexion]Réception de fichier]Fichier enregistré";
            }
            break;
        }
        default:
        {
            in.skipRawData(this->tailleMessage - sizeof(this->tailleMessage));
        }
    }
}

