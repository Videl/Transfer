#ifndef VWIDGET_H
#define VWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QDebug>

class Connexion;

class VWidget : public QWidget {
    Q_OBJECT

public:
    VWidget();
    void resizeEvent(QResizeEvent* event);

signals:
    void tailleChange();
};

#endif // VWIDGET_H
