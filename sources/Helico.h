#ifndef HELICO_H
#define HELICO_H

#include <QVector3D>
#include "ImportateurObj.h"
#include "ElementRotatif.h"
#include "Vehicule.h"
#include "Chargement.h"
#include "OscillateurHarmonique.h"

class Helico : public Vehicule
{
    public :
        Helico(Ville *, Chargement * = NULL);
        virtual ~Helico();
        void enterFrame();
        void dessiner();
        void placerCamera(CameraFreeFly *);
		
		void gauche();
        void droite();
        void avancer();
        void reculer();
        void haut();
        void bas();
        void action();

        void deplacer();

    private:
        bool deplacementCorrect(QVector3D nouvellePosition, double nouveauAngle);
        void calculerCoord(QVector3D laPosition ,double lAngle);
        void calculerCoord(QVector3D laPosition, std::vector<QVector3D> & coordAbsolues , double lAngle, std::vector<QVector3D> const &coord);

    private :
        struct Helice : public ElementRotatif
        {
            Helice();
        };
        ImportateurObj m_monObj;
        Helice m_helices[2];

        QVector3D m_vitesse;
        double m_vitesseAngle;
        bool m_baseComplete;

        std::vector<QVector3D> m_heliceCoord;
        std::vector<QVector3D> m_heliceCoordAbsolues;
        std::vector<QVector3D> m_baseCoord;
        std::vector<QVector3D> m_baseCoordAbsolues;

        OscillateurHarmonique m_oscCamera;
    public:
        static const double ACCELERATION; //Unités par seconde par seconde
        static const double ACCELERATION_HAUT; //Unités par seconde par seconde
        static const double GRAVITE; //Unités par seconde par seconde
        static const double POURCENTAGE_FROTTEMENT; //Pourcentage par frame
        static const double VITESSE_ROTATION; //Radians par seconde
        static const double DELTA_Z_MAX;
        static const double SCALE;
};

#endif // HELICO_H
