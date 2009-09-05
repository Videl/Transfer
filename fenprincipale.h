#ifndef FENPRINCIPALE_H
#define FENPRINCIPALE_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QSize>
#include <QCloseEvent>
#include <QString>
#include <QStringListModel>
#include <QList>
#include <QStandardItem>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTableView>
#include <QListView>
#include <QTextEdit>
#include <QLineEdit>
#include <QMenu>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QtNetwork>
#include <QSettings>

enum DataType {
    DT_Chat = 1,
    DT_Pseudo = 2,
    DT_Admin = 3,
    DT_IsWriting = 4,
    DT_StoppedWriting = 5,
    DT_Nothing = 6,
    DT_Transferable = 7,
    DT_NotTransferable = 8,
    DT_InfoFile = 9,
    DT_InfoFileResult = 10,
    DT_WantFile = 11,
    DT_AskingForFile = 12,
    DT_FileData = 13

};

class FenConnexion;
class VWidget;
class Connexion;
class VDownload;
class FenOptions;

class FenPrincipale : public QMainWindow
{
    Q_OBJECT

public:
    FenPrincipale(QWidget *parent = 0);
    ~FenPrincipale();

    // Re-implémentation
    void closeEvent(QCloseEvent* event);

    // Initialisation des menus, actions, toolbar
    void iniMenu();
    void iniActions();
    void iniBarreEtat();

    // Ini de la fenêtre
    void reglerTaille();

    // Accesseurs et mutateurs
    QString getPseudo();
    void setPseudo(QString string);
    void setStatusLabel(const QString texte);
    void setMessage(const QString string, const int temps = 5000);
    QList<QStandardItem *> creerEntree(QString nom, QString proprio, QString taille, QString typeMime, QString statut);
    void addStringlisteMessages(QString string);

    // Actualisation et affichage
    void actualisationListePseudos(QString liste);
    QString conversionTaille(unsigned int size, int prec);

    // Actualisation fichier
    void updateWhereNameIs(QString nom, QString size, QString typeMime, QString valide);
    void nouveauFichierAjouteReseau(QString nom, QString proprio, QString typeMime, QString taille);

    void prepEnvoiFichier(QString fileName, QString demandeur);
    void nouvelleFenetreFichier(VDownload* fenDL);
    QList<VDownload *> getListeTelechargement();

public slots:

private slots:
    void prepDLFile(QModelIndex item);
    void prepMessage();

    void updateSize();

    void ecriture();
    void ajouterFichier();

private:
    VWidget *zoneCentrale;
    QVBoxLayout *layout;
    QHBoxLayout *hbox;
    QSplitter *splitter;
    QSplitter *ch_splitter;

    QStandardItemModel *modele;
    QTableView *vue;
    QList<QStandardItem *> listeFichiers;
    QList<QString> urlFichiers;

    QStringListModel *ch_listePseudos;
    QStringList listePseudos;
    QListView *ch_vuePseudos;
    QTextEdit *ch_listeMessages;
    QLineEdit *ch_pseudo;
    QLineEdit *ch_message;
    bool isWriting;

    FenConnexion *fenetreConnexion;
    Connexion *connexion;
    FenOptions *options;

    QMenu *menuFichier;
    QMenu *menuConnexion;
    QMenu *menuEdition;
    QMenu *menuAPropos;
    QStatusBar *barreEtat;
    QLabel *label_status;

    QList<VDownload * > listeDeTelechargement;

};

#endif // FENPRINCIPALE_H
