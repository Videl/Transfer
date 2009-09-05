#include "fenoptions.h"

#define LINUX

#ifdef WINDOWS
#define PATHSEPARATOR "\\"
#endif

#ifdef LINUX
#define PATHSEPARATOR "/"
#endif


FenOptions::FenOptions(QWidget* parent) : QDialog(parent) {
    settings = new QSettings("Videled", "Transfer", parent);
    this->setWindowTitle("Edition des préférences");
    fenetre = qobject_cast<QMainWindow *>(parent);
    this->setModal(false);

    iniTab();
    voirOptions();
}

void FenOptions::sauvegarde() {
    settings->setValue("Fichier/Emplacement", urlFolder->text());
    settings->setValue("Fichier/Autodemande", asking_savefolder->checkState());

    settings->setValue("Connexion/Serveur", default_serv->text());
    settings->setValue("Connexion/Pseudo", default_pseudo->text());
    settings->setValue("Connexion/Autoconnexion", auto_log->checkState());
}

void FenOptions::voirOptions() {
    if(settings->value("nombreDUtilisation", 0).toInt() == 0) {
        settings->setValue("nombreDUtilisation", 1);
        settings->setValue("Taille/Largeur", 800);
        settings->setValue("Taille/Hauteur", 600);
        settings->setValue("Taille/Position", QPoint(0, 0));
        largeur = 800;
        hauteur = 600;

        settings->setValue("Connexion/Serveur", "videl.serveftp.net");
        default_serv->setText("videl.serveftp.net");

        qDebug() << "FenOptions]Initialisation";
        this->exec();
    } else {
        largeur = settings->value("Taille/Largeur");
        hauteur = settings->value("Taille/Hauteur");
        position = settings->value("Taille/Position", QPoint(0, 0));

        urlFolder->setText(settings->value("Fichier/Emplacement").toString());
        asking_savefolder->setCheckState((Qt::CheckState)(settings->value("Fichier/Autodemande").toInt()));

        default_serv->setText(settings->value("Connexion/Serveur", "videl.serveftp.net").toString());
        default_pseudo->setText(settings->value("Connexion/Pseudo").toString());
        auto_log->setCheckState((Qt::CheckState)(settings->value("Connexion/Autoconnexion", Qt::Unchecked).toInt()));


        qDebug() << "FenOptions]Chargement des options";
    }
}

void FenOptions::iniTab() {
    tab = new QTabWidget(this);

    QWidget *page1 = new QWidget(tab);
    QWidget *page2 = new QWidget(tab);

    tab->addTab(page1, "Connexion");
    tab->addTab(page2, "Téléchargement de Fichiers");

    // Page 1 : Connexion
    QFormLayout *form1 = new QFormLayout(tab);
    default_serv = new QLineEdit(tab);
    default_pseudo = new QLineEdit(tab);
    auto_log = new QCheckBox("Connexion automatique", tab);
    auto_log->setCheckState(Qt::Unchecked);

    form1->addRow("Serveur par défaut :", default_serv);
    form1->addRow("Pseudo à utiliser par défaut :", default_pseudo);
    form1->addRow(auto_log);

    page1->setLayout(form1);
    // Fin Page 1 : Connexion

    // Page 2 : Téléchargement de Fichiers

    QFormLayout *form2 = new QFormLayout(tab);
    boutonDossier = new QPushButton("Selectionner un dossier", tab);
    urlFolder = new QLineEdit(tab);
    asking_savefolder = new QCheckBox("Demander l'emplacement de sauvegarde pour chaque téléchargement", tab);
    asking_savefolder->setCheckState(Qt::Unchecked);

    form2->addRow(boutonDossier);
    form2->addRow(urlFolder);
    form2->addRow(asking_savefolder);

    page2->setLayout(form2);

    QHBoxLayout *lay = new QHBoxLayout();
    lay->addWidget(tab);
    this->setLayout(lay);

    connect(asking_savefolder, SIGNAL(stateChanged(int)), this, SLOT(refreshCheck_askingFolder(int)));
    connect(boutonDossier, SIGNAL(released()), this, SLOT(choisirDossierSav()));
}

void FenOptions::choisirDossierSav() {
    QString dossier = QFileDialog::getExistingDirectory(this, "Choisissez votre dossier de téléchargement");
    if(dossier.data()[dossier.size()-1] != QVariant(PATHSEPARATOR).toChar()) dossier = dossier + QVariant(PATHSEPARATOR).toString();
    //dossier = "\""+ dossier + "\"";
    urlFolder->setText(dossier);
}

void FenOptions::refreshCheck_askingFolder(int state) {
    if(state == Qt::Checked) {
        boutonDossier->setEnabled(false);
        urlFolder->setEnabled(false);
        asking_savefolder->setCheckState(Qt::Checked);
    } else if(state == Qt::Unchecked) {
        boutonDossier->setEnabled(true);
        urlFolder->setEnabled(true);
        asking_savefolder->setCheckState(Qt::Unchecked);
    }
}


void FenOptions::setDimensions(int largeur, int hauteur, QPoint pos) {
    settings->setValue("Taille/Largeur", largeur);
    settings->setValue("Taille/Hauteur", hauteur);
    pos.setY(pos.y() - 20);
    settings->setValue("Taille/Position", pos);
    // Dimensions de la fenêtre
}

int FenOptions::getLargeur() {
    return largeur.toInt();
}

int FenOptions::getHauteur() {
    return hauteur.toInt();
}

QPoint FenOptions::getPosition() {
    return position.toPoint();
}

QString FenOptions::getPseudo() {
    return default_pseudo->text();
}

QString FenOptions::getServeur() {
    return default_serv->text();
}

QString FenOptions::getSaveFolder() {
    return urlFolder->text();
}

Qt::CheckState FenOptions::getSaveAsking() {
    return asking_savefolder->checkState();
}
