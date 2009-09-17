#include "vdownload.h"
#include "connexion.h"
#include "fenconnexion.h"
#include "fenprincipale.h"
#include "vwidget.h"
#include "fenoptions.h"

VDownload::VDownload(QWidget* parent, QString nom, int nbPaquet, FenOptions *opt) : QDialog(parent), options(opt) {
    this->setModal(false);
    this->setWindowTitle("Téléchargement");
    QGridLayout *layout = new QGridLayout(this);
    nomDuFichier = nom;

    QString dossier;
    if(options->getSaveAsking() == Qt::Unchecked)
        dossier = options->getSaveFolder();
    else dossier = QFileDialog::getExistingDirectory(this, "Choisissez votre dossier de téléchargement");



    urlStockage = dossier + nom;

    tailleMorceauxFichiers = (1024*64); // 256 KiB
    fichier = new QFile(urlStockage);
    if(!fichier->open(QIODevice::WriteOnly))
        qDebug() << "[VDownload]Création du fichier]Erreur lors de la création du fichier " << urlStockage;

    lab_nomDuFichier = new QLabel("Création de "+nom+" dans \n"+options->getSaveFolder());
    lab_vitesse = new QLabel;
    num_vitesse = new QVariant(0);

    timerDL = new QTime;
    timerDL->start();
    lastTime = timerDL->elapsed();


    lab_vitesse->setText("Vitesse de téléchargement : "+num_vitesse->toString()+" Kio/s");
    this->tailleFichierEu = new QVariant(0);
    this->tailleFichierFinal = new QVariant(calculerTaille(nbPaquet));


    progression = new QProgressBar(this);
    progression->setMaximum(this->tailleFichierFinal->toInt());
    progression->setValue(this->tailleFichierEu->toInt());
    progression->setFormat("%p%");

    QPushButton *boutonFermer = new QPushButton("Fermer", this);
    layout->addWidget(lab_nomDuFichier, 0, 0, 1, 0);
    layout->addWidget(lab_vitesse, 1, 0, 1, 0);
    layout->addWidget(progression, 2, 0, 1, 0);
    layout->addWidget(boutonFermer, 3, 2);

    this->setLayout(layout);

    connect(boutonFermer, SIGNAL(released()), this, SLOT(deleteLater()));
    this->show();
}

void VDownload::addBytes(QByteArray bytes) {
    if(this->fichier)
        fichier->write(bytes);

    progression->setValue(fichier->size());

    int newTime = timerDL->elapsed() - lastTime;
    int dataTr = bytes.size();
    if(newTime >= 1) {
        num_vitesse->setValue(dataTr / newTime);
        lab_vitesse->setText("Vitesse de téléchargement : "+ num_vitesse->toString() +" Kio/s");
        progression->setFormat("%p% - "+num_vitesse->toString()+" Kio/s");
    }
    lastTime = timerDL->elapsed();
}

void VDownload::terminate() {
    this->fichier->close();
    delete this->fichier;
    fichier = 0;

    qDebug() << "[VDownload]Terminate()]Terminated]";
    progression->setValue(progression->maximum());
}

QFile* VDownload::getFile() {
    return this->fichier;
}

QString VDownload::getNomFichier() {
    return this->nomDuFichier;
}

unsigned int VDownload::calculerTaille(int nbPaquet) {
    unsigned int taille;
    taille = nbPaquet * tailleMorceauxFichiers;
    taille = taille - tailleMorceauxFichiers + 1;
    return taille;
}
