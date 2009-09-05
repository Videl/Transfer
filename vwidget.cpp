#include "vwidget.h"
#include "connexion.h"

VWidget::VWidget()
{
}

void VWidget::resizeEvent(QResizeEvent* even) {
    even->accept();
    emit this->tailleChange();
}
