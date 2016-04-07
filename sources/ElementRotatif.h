#ifndef ELEMENTROTATIF_H
#define ELEMENTROTATIF_H

#include "VertexArray.h"
#include <QVector3D>
#include <QGLWidget>

class ElementRotatif
{
    public:
        ElementRotatif();
        virtual ~ElementRotatif();

        VertexArray * donnees;
        QVector3D coord;
        double angle; //Degres
        QVector3D axeRotation;

        double vitesse() { return m_vitesseRotation; }
        void setVitesse(double);
        void setVitesseMax(double);
        void augmenterVitesse(double);
    private :
        double m_vitesseRotation;
        double m_vitesseRotationMax;
    public :
        void dessiner();
        virtual void dessinerEnfants();
        void appliquerVitesse();
        void translater();
};

#endif // ELEMENTROTATIF_H
