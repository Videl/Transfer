#include "fenprincipale.h"
#include "fenconnexion.h"
#include "vwidget.h"
#include "connexion.h"
#include "vdownload.h"
#include "fenoptions.h"

FenPrincipale::FenPrincipale(QWidget *parent) : QMainWindow(parent) {
    zoneCentrale = new VWidget();
    /*zoneCentrale->setMinimumWidth(800);
    zoneCentrale->setMinimumHeight(600);*/

    options = new FenOptions(this);
    options->hide();

    connexion = new Connexion(this, options);
    fenetreConnexion = new FenConnexion(this, connexion, options);
    connexion->setFenConnexion(fenetreConnexion);

    layout = new QVBoxLayout(zoneCentrale);
    hbox = new QHBoxLayout;
    ch_splitter = new QSplitter(Qt::Vertical, zoneCentrale);
    splitter = new QSplitter(zoneCentrale);
    modele = new QStandardItemModel(0, 4, zoneCentrale);
    vue = new QTableView(zoneCentrale);
    ch_listePseudos = new QStringListModel(zoneCentrale);
    ch_vuePseudos = new QListView(zoneCentrale);
    ch_listeMessages = new QTextEdit(zoneCentrale);
    ch_listeMessages->setMaximumHeight(300);
    ch_vuePseudos->setMaximumWidth(200);
    ch_pseudo = new QLineEdit(zoneCentrale);
    ch_pseudo->setText(options->getPseudo());
    ch_pseudo->setMaximumWidth(150);
    ch_message = new QLineEdit(zoneCentrale);

    for(int row = 0; row < 1; ++row) {
         for (int column = 0; column < 5; ++column) {
             QStandardItem *item = new QStandardItem(QString("Ligne %0 Col %1").arg(0).arg(column));
             modele->setItem(row, column, item);
         }
     }
    modele->removeRow(0);

    vue->verticalHeader()->hide();
    vue->setShowGrid(false);
    vue->setSelectionBehavior(QAbstractItemView::SelectRows);
    vue->setEditTriggers(QAbstractItemView::NoEditTriggers);


    modele->setHeaderData(0, Qt::Horizontal, "Nom du fichier");
    modele->setHeaderData(1, Qt::Horizontal, "Propriétaire");
    modele->setHeaderData(2, Qt::Horizontal, "Taille");
    modele->setHeaderData(3, Qt::Horizontal, "Type");
    modele->setHeaderData(4, Qt::Horizontal, "Valide");


    iniMenu();
    iniActions();
    iniBarreEtat();

    vue->setModel(modele);
    ch_vuePseudos->setModel(ch_listePseudos);

    splitter->addWidget(ch_listeMessages);
    splitter->addWidget(ch_vuePseudos);

    ch_splitter->addWidget(vue);
    ch_splitter->addWidget(splitter);

    layout->addWidget(ch_splitter);

    hbox->addWidget(ch_pseudo);
    hbox->addWidget(ch_message);

    layout->addLayout(hbox);
    zoneCentrale->setLayout(layout);
    setCentralWidget(zoneCentrale);


    connect(ch_message, SIGNAL(returnPressed()), this, SLOT(prepMessage()));
    connect(ch_message, SIGNAL(textChanged(QString)), this, SLOT(ecriture()));
    isWriting = false;

    //QTimer *timer = new QTimer(zoneCentrale);
    //connect(timer, SIGNAL(timeout()), this, SLOT(updateSize()));
    //timer->start(50);
    //QTimer::singleShot(1000, this, SLOT(reglerTaille()));
    reglerTaille();

    connect(zoneCentrale, SIGNAL(tailleChange()), this, SLOT(updateSize()));

    connect(vue, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(prepDLFile(QModelIndex)));
}

void FenPrincipale::reglerTaille() {
    resize(options->getLargeur(), options->getHauteur());
    move(options->getPosition());

    this->updateSize();
}

void FenPrincipale::closeEvent(QCloseEvent* event) {
    options->setDimensions(this->width(), this->height(), this->pos());
    options->sauvegarde();

    event->accept();
}

void FenPrincipale::prepDLFile(QModelIndex item) {
    // Demande de fichier à quelqu'un (owner)
    QString owner, fileName;
    owner = modele->item(item.row(), 1)->text();
    fileName = modele->item(item.row(), 0)->text();
    if(owner != ch_pseudo->text()) {
        connexion->demanderFichier(fileName, owner);
        //qDebug() << "Demande fichier envoyé";
    }
}

void FenPrincipale::prepEnvoiFichier(QString fileName, QString demandeur) {
    QString url, nom;
    //qDebug() << "[FenPrincipale]" << "Fichier a trouver :" << fileName;
    for(int i = 0; i < listeFichiers.size(); i++) {
        if(fileName == listeFichiers[i]->text()) {
            url = urlFichiers[i];
            nom = listeFichiers[i]->text();
        }
    }

    connexion->envoyerFichier(url, nom, demandeur);
}

void FenPrincipale::nouveauFichierAjouteReseau(QString nom, QString proprio, QString typeMime, QString taille) {
    if(proprio != ch_pseudo->text()) {
        modele->appendRow(this->creerEntree(nom, proprio, taille, typeMime, "Validé"));
        urlFichiers.append("not owned");
        qDebug() << "Nouveau fichier dans le réseau :"<< nom << proprio << typeMime << taille;
    }
}


void FenPrincipale::ajouterFichier()  {
    if(!ch_pseudo->text().isEmpty()) {
        if(connexion->getConnected()) {
            QString fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString());
            if(!fichier.isEmpty()) {
                QFileInfo *info = new QFileInfo(fichier);
                if(info->exists()) {
                    urlFichiers.append(fichier);

                    QList<QStandardItem *> liste = creerEntree(info->fileName(), ch_pseudo->text(), conversionTaille(info->size(), 2), "Inconnu", "Non demandé");

                    modele->appendRow(liste);

                    if(connexion->getConnected()) {
                        connexion->envoyerInfoFile(info->fileName(), conversionTaille(info->size(), 2), "Inconnu");
                        liste[4]->setText(tr("En Attente"));
                    }

                } else this->addStringlisteMessages("<em>Le fichier n'existe pas.</em>");
            }
        } else this->addStringlisteMessages("<em>Vous devez être connecté pour ajouter un fichier.</em>");
    } else this->addStringlisteMessages("<em>Vous devez avoir un pseudo pour ajouter un fichier.</em>");
}

void FenPrincipale::ecriture() {
    if(ch_message->text().isEmpty()) {
        if(isWriting == true) {
            connexion->envoyerMessage(ch_pseudo->text(), DT_StoppedWriting);

            isWriting = false;
        }
    }else { // ch_message.text() n'est pas vide
        if(isWriting == false) {
            connexion->envoyerMessage(ch_pseudo->text(), DT_IsWriting);
            isWriting = true;
        }
    }
}

void FenPrincipale::nouvelleFenetreFichier(VDownload* fenDL) {
    listeDeTelechargement << fenDL;
}

QList<VDownload *> FenPrincipale::getListeTelechargement() {
    return listeDeTelechargement;
}

void FenPrincipale::actualisationListePseudos(QString liste) {
    QStringList trie = liste.split("$");
    trie.removeLast();
    QStringList ancienne = listePseudos;

    listePseudos.clear();
    listePseudos << trie;
    //modele->setStringList(listePseudos);
    ch_listePseudos->setStringList(listePseudos);

    ancienne << trie;
}

void FenPrincipale::prepMessage() {
    QString message = ch_message->text();
    if(!message.isEmpty()) {
        connexion->envoyerMessage(message);
        ch_message->clear();
        ch_message->setFocus();
    }
}

FenPrincipale::~FenPrincipale() {
}

void FenPrincipale::iniMenu() {
    menuFichier = menuBar()->addMenu("&Fichier");
    menuEdition = menuBar()->addMenu("E&dition");
    menuConnexion = menuBar()->addMenu("&Connexion");
    menuAPropos = menuBar()->addMenu("A&ide");
}

void FenPrincipale::iniActions() {    
    QAction *actionAddFile = menuFichier->addAction("Ajouter un fichier");
    connect(actionAddFile, SIGNAL(triggered()), this, SLOT(ajouterFichier()));
    actionAddFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

    QAction *actionQuitter = menuFichier->addAction("&Quitter");
    connect(actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));
    actionQuitter->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));

    QAction *actionOuvrir = menuConnexion->addAction("&Option de connexion");
    connect(actionOuvrir, SIGNAL(triggered()), fenetreConnexion, SLOT(show()));

    QAction *actionOptions = menuEdition->addAction("&Préférences");
    connect(actionOptions, SIGNAL(triggered()), options, SLOT(show()));

}

void FenPrincipale::iniBarreEtat() {
    barreEtat = statusBar();
    QPushButton* boutonQuick = new QPushButton("Quick Connect");
    label_status = new QLabel("Déconnecté");
    barreEtat->insertPermanentWidget(0, label_status);
    barreEtat->insertPermanentWidget(1, boutonQuick);

    connect(boutonQuick, SIGNAL(clicked()), connexion, SLOT(tentativeConnexion()));
}

void FenPrincipale::setMessage(QString string, int temps) {
    barreEtat->showMessage(string, temps);
}

void FenPrincipale::setStatusLabel(QString texte) {
    this->label_status->setText(texte);
}

QString FenPrincipale::conversionTaille(unsigned int size, int prec) {
    double a, b, c;
    QString taille;

    a = (double)size;
    b = (double)1048576;
    c = a / b;
    if(c > 1024) { // Gio
        b = (double)1073741824;
        c = a / b;        
        taille.setNum(c, 'f', prec);
        taille = taille + " Gio";
    } else if(c < 1) { // Kio
        b = (double)1024;
        c = a / b;
        taille.setNum(c, 'f', prec);
        taille = taille + " Kio";
    } else {
        taille.setNum(c, 'f', prec);
        taille = taille + " Mio";
    }

    return taille;
}

void FenPrincipale::updateWhereNameIs(QString nom, QString size, QString typeMime, QString valide) {
    for(int i = 0; i < listeFichiers.size(); i++) {
        if(listeFichiers[i]->text() == nom) {
            QModelIndex modelIndex = modele->indexFromItem(listeFichiers[i]);
            modele->item(modelIndex.row(), 4)->setText(valide);
            return;
        }
    }
}

QList<QStandardItem *> FenPrincipale::creerEntree(QString nom, QString proprio, QString taille, QString typeMime, QString statut) {
    QStandardItem *item1 = new QStandardItem(nom);
    listeFichiers.append(item1);
    QStandardItem *item2 = new QStandardItem(proprio);
    QStandardItem *item3 = new QStandardItem(taille);
    QStandardItem *item4 = new QStandardItem(typeMime);
    QStandardItem *item5 = new QStandardItem(statut);

    QList<QStandardItem *> items;
    items.append(item1);
    items.append(item2);
    items.append(item3);
    items.append(item4);
    items.append(item5);

    return items;
}


void FenPrincipale::updateSize() {
    QVariant taille = vue->width() / 5;

    for(int row = 0; row < 5; row++)
         vue->setColumnWidth(row, (taille.toInt()));
    vue->horizontalScrollBar()->setVisible(false);

    //options->setDimensions(vue->width(), vue->height());

    //qDebug() << "[FenPrincipale]Largeur "<< vue->width() << "; Hauteur :" << vue->height();
}

QString FenPrincipale::getPseudo() {
    return ch_pseudo->text();
}
void FenPrincipale::setPseudo(QString string) {
    ch_pseudo->setText(string);
}

void FenPrincipale::addStringlisteMessages(QString string) {
    ch_listeMessages->append(string);
}
