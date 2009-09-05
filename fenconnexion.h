#ifndef FENCONNEXION_H
#define FENCONNEXION_H

#include <QtNetwork>
#include <QtGui>

#include <QDialog>

class FenPrincipale;
class Connexion;
class FenOptions;

class FenConnexion : public QDialog {
    Q_OBJECT

public:
    FenConnexion(QWidget *parent, Connexion* connex, FenOptions *opt);
    void setLabelStatut(QString text);
    QString getServIp();
    int getServPort();
    void switchState_boutonConnexion(bool state);
    void switchState_serv_ip(bool state);
    void switchState_serv_port(bool state);





private:
    QLineEdit *serv_ip;
    QSpinBox *serv_port;
    QLabel *lab_statut;    
    QPushButton *boutonConnexion;

    FenPrincipale *fenetre;
    Connexion *connexion;
    FenOptions *options;

};

#endif // FENCONNEXION_H
