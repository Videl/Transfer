#ifndef VDOWNLOAD_H
#define VDOWNLOAD_H

#include <QDialog>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVariant>
#include <QGridLayout>
#include <QDataStream>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTime>
#include <QtNetwork/QTcpSocket>
#include <QFileDialog>

class FenPrincipale;
class Connexion;
class FenPrincipale;
class VWidget;
class FenOptions;


class VDownload : public QDialog {
public:
    VDownload(QWidget* parent, QString nom, int nbPaquet, FenOptions* opt);

    QString getNomFichier();
    void addBytes(QByteArray bytes);
    QFile* getFile();
    void terminate();


private:
    unsigned int calculerTaille(int nbPaquet);

    QLabel *lab_nomDuFichier;
    QLabel *lab_vitesse;
    QVariant *num_vitesse;
    QVariant *tailleFichierEu;
    QVariant *tailleFichierFinal;
    QProgressBar *progression;

    QString nomDuFichier;
    QString urlStockage;
    QFile *fichier;
    int tailleMorceauxFichiers;

    QTime *timerDL;
    int lastTime;

    FenOptions *options;
};

#endif // VDOWNLOAD_H
