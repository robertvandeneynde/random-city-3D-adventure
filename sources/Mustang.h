#ifndef MUSTANG_H
#define MUSTANG_H

#include <QVector3D>
#include <QVector2D>
#include "ImportateurObj.h"
#include "ElementRotatif.h"
#include "Vehicule.h"
#include "Chargement.h"
#include "fonctionsMathematiques.h"
#include "OscillateurHarmonique.h"
#include "Voiture.h"

class Ville;

class Mustang : public Vehicule, public Voiture
{
    public:
        Mustang(Ville* v, Chargement * = NULL);
        void enterFrame();
        void dessiner();
        void placerCamera(CameraFreeFly *);
        void gauche();
        void droite();
        void avancer();
        void reculer();
		void action2();
    private:
        void calculerCoordRoues();
        void calculerCoordCoins();
        void calculerCoordRoues(QVector3D laPosition ,QVector3D coordRoueAbsolues[4], double angle);
        void calculerCoordCoins(QVector3D laPosition ,QVector3D coordRoueAbsolues[4], double angle);
        void actualiserAngleX();
        void deplacer();
        bool deplacementCorrect(QVector3D newPosition, double newAngle, QVector2D & normaleMur);
        QVector2D determinerNormaleMur(QVector2D pos, QVector2D newPos);
        void gererRebond(QVector2D normale);
    private :
        Ville* getVille();
    private :
        struct Roue : public ElementRotatif
        {
                Roue();
        };
    private :
        ImportateurObj m_monObj;
        Roue m_roues[4];
        QVector3D m_coinsCoord[4];

        QVector3D m_coordRouesAbsolues[4];
        QVector3D m_coordCoinsAbsolus[4];
        OscillateurHarmonique m_oscCamera;
    public :
        static const double ACCELERATION; //Unités / seconde
        static const double VITESSE_ROTATION; //Radians / seconde
        static const double DELTA_Z_MAX;
        static const double SCALE;
};

#endif // MUSTANG_H
