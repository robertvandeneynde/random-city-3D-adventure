#ifndef DESSINATEUR_H
#define DESSINATEUR_H

#include "VertexArray.h"

#include "StructZone.h"
#include "fonctionsMathematiques.h"

#include <QColor>
#include <QVector3D>

class Ville;
class Zone;

class Dessinateur
{
    public:
        Dessinateur(VertexArray &);
		virtual void pointilles(Zone, int directionParal, QColor = QColor::fromRgbF(0.8,0.8,0.8));
		virtual void bandeDeSeparation(Zone, int directionParal, QColor = QColor::fromRgbF(0.8,0.8,0.8));
		virtual void cubeDroit(double x, double dx, double y, double dy, double z, double dz, QColor); //Affichage QUADS
		virtual void cubeDroitTexture(double x, double dx, double y, double dy, double z, double dz, GLuint texture, QColor = Qt::white);
		virtual void cubePenche(Zone, double dz, QColor);
		virtual void trianglesPriorite(Ville * ville, double largeur, int n = 5); //Va desssiner dans la zone (x > 0, y < 0)

		void cubeDroit(StructZone, double z, double dz, QColor);
		void cubeDroitTexture(StructZone, double z, double dz, GLuint, QColor = Qt::white);
		void cubeDroitCentre(double x, double dx, double y, double dy, double z, double dz, QColor);

		VertexArray::HandleBuilder handleBuilder() { return sommets.handleBuilder(); }
    protected :
		VertexArray & sommets;
};

class Repere
{
    public :
        static const int X=0, Y=1, Z=2;
    private :
        typedef void (*FonctionRotation)(double[16], double);
        static FonctionRotation FONCTIONS_ROTATION[3];
    public :
        Repere();
        ~Repere();
        Repere& translate(QVector3D);
        Repere& rotate(double, QVector3D);
        Repere& rotate(double, int dimension);
        Repere& scale(double);
        Repere& rotate(QVector3D nouveauX, QVector3D nouveauY, QVector3D nouveauZ);
        //Troisième vecteur déterminé par produit vectoriel (Dextrogyre)
        Repere& rotateXY(QVector3D nouveauX, QVector3D nouveauY);
        Repere& rotateXZ(QVector3D nouveauX, QVector3D nouveauZ);
        Repere& rotateYZ(QVector3D nouveauY, QVector3D nouveauZ);
};

#endif // DESSINATEUR_H
