#ifndef DESSINATEURCARTE_H
#define DESSINATEURCARTE_H

#include "Dessinateur.h"

class DessinateurCarte : public Dessinateur
{
    public:
        DessinateurCarte(VertexArray &);
        virtual void pointilles(Zone, int directionParal, QColor = QColor::fromRgbF(0.8,0.8,0.8));
        virtual void bandeDeSeparation(Zone, int directionParal, QColor = QColor::fromRgbF(0.8,0.8,0.8));
        virtual void cubeDroit(double x, double dx, double y, double dy, double z, double dz, QColor);
        virtual void cubeDroitTexture(double x, double dx, double y, double dy, double z, double dz, GLuint, QColor = Qt::white);
        virtual void cubePenche(Zone, double dz, QColor);
        virtual void trianglesPriorite(Ville * ville, double largeur, int n = 5); //Va desssiner dans la zone (x > 0, y < 0)
};

#endif // DESSINATEURCARTE_H
