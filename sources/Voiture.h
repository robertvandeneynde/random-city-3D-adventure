#ifndef VOITURE_H
#define VOITURE_H

#include "ObjetScene.h"

#include <QVector2D>
#include <QPoint>
#include <list>

class Ville;

class Voiture : public virtual ObjetScene
{
    public:
        Voiture();
        QVector2D getVitesse() { return m_vitesse; }
    protected:
        virtual void deplacer();
        virtual Ville* getVille() = 0;
    private :
        QPoint m_endroitVille;
    protected:
		QVector2D m_vitesse; //Unit/Frame
        double m_vitesseAngle;
    private :
        static const double POURCENTAGE_FROTTEMENT; //Pourcentage par frame
};

#endif // VOITURE_H
