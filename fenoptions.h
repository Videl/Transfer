#ifndef FENOPTIONS_H
#define FENOPTIONS_H

#include <QDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QMainWindow>
#include <QPoint>
#include <QTabWidget>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QChar>
#include <QFile>

class FenOptions : public QDialog {
    Q_OBJECT
public:
    FenOptions(QWidget *parent);
    void setDimensions(int largeur, int hauteur, QPoint pos);

    // Accesseurs
    int getHauteur();
    int getLargeur();
    QPoint getPosition();
    QString getPseudo();
    QString getServeur();
    QString getSaveFolder();
    Qt::CheckState getSaveAsking();

    void iniTab();

    void sauvegarde();

private:
    void voirOptions();

private slots:
    void refreshCheck_askingFolder(int state);
    void choisirDossierSav();

private:    
    QSettings *settings;

    QMainWindow *fenetre;
    QTabWidget *tab;

    QVariant hauteur;
    QVariant largeur;
    QVariant position;

    // Widget du Tab
    // Connexion
    QLineEdit *default_serv;
    QLineEdit *default_pseudo;
    QCheckBox *auto_log;

    // Téléchargements de fichiers
    QPushButton *boutonDossier;
    QLineEdit *urlFolder;
    QCheckBox *asking_savefolder;
};

#endif // FENOPTIONS_H
