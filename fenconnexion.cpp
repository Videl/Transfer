#include "fenconnexion.h"
#include "fenprincipale.h"
#include "connexion.h"
#include "fenoptions.h"

FenConnexion::FenConnexion(QWidget *parent, Connexion* connex, FenOptions* opt) : QDialog(parent), connexion(connex), options(opt) {
    fenetre = qobject_cast<FenPrincipale *>(parent);

    QGridLayout *layout = new QGridLayout;
    boutonConnexion = new QPushButton("Connexion");
    QPushButton *boutonAnnuler = new QPushButton("Fermer");

    QLabel *label1 = new QLabel("Vous pouvez vous connecter à n'importe quel serveur <strong>Videled</strong>.<br />Celui par défaut convient, mais il n'est pas tout le temps opérationnel.");
    serv_ip = new QLineEdit;
    serv_ip->setText(options->getServeur());
    serv_port = new QSpinBox;

    lab_statut = new QLabel("Vous n'avez pas encore essayé de vous connecter.");

    serv_port->setMinimum(1024);
    serv_port->setMaximum(65536);

    serv_port->setValue(50885);

    layout->addWidget(label1, 0, 0, 1, 5);
    layout->addWidget(serv_ip, 1, 0, 1, 4);
    layout->addWidget(serv_port, 1, 4, 1, 1);
    layout->addWidget(lab_statut, 4, 0);
    layout->addWidget(boutonAnnuler, 4, 3);
    layout->addWidget(boutonConnexion, 4, 4);

    this->setModal(false);
    this->setLayout(layout);

    connect(boutonAnnuler, SIGNAL(clicked()), this, SLOT(close()));
    connect(boutonConnexion, SIGNAL(clicked()), connexion, SLOT(tentativeConnexion()));
}

int FenConnexion::getServPort() {
    return serv_port->value();
}

QString FenConnexion::getServIp() {
    return serv_ip->text();
}

void FenConnexion::setLabelStatut(QString text) {
    lab_statut->setText(text);
}

void FenConnexion::switchState_boutonConnexion(bool state) {
    boutonConnexion->setEnabled(state);
}

void FenConnexion::switchState_serv_ip(bool state) {
    serv_ip->setEnabled(state);
}

void FenConnexion::switchState_serv_port(bool state) {
    serv_port->setEnabled(state);
}
