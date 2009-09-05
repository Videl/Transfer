#include <QtGui/QApplication>
#include "fenprincipale.h"
#include "fenconnexion.h"
#include <QGtkStyle>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    a.setStyle(new QGtkStyle);
    FenPrincipale w;
    w.show();
    return a.exec();
}
