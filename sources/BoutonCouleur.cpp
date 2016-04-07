#include "BoutonCouleur.h"

BoutonCouleur::BoutonCouleur(QWidget *parent) :
    QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void BoutonCouleur::onClicked()
{
    QColor coul = QColorDialog::getColor(maCouleur);
    if( coul.isValid() )
        setCouleur(coul);
}

void BoutonCouleur::setCouleur(QColor coul)
{
    if(maCouleur != coul)
    {
        maCouleur = coul;
        emit couleurChangee(coul);
        update();
    }
}


void BoutonCouleur::paintEvent(QPaintEvent * ev)
{
    QPushButton::paintEvent(ev);
    QPainter painter(this);
    painter.setBrush( QBrush(maCouleur) );
    QSize size = rect().size();
    double p = 0.15;
    painter.drawRect(
        QRect(
            p * QPoint(size.width(), size.height()),
            (1 - p*2) * size
        )
    );
}


