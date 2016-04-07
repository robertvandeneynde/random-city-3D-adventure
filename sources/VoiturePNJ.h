#ifndef VOITUREPNJ_H
#define VOITUREPNJ_H

#include "ImportateurObj.h"
#include "Voiture.h"

#include <QGLWidget>
#include <QVector3D>
#include "GPS.h"

#include <QVector2D>

class Ville;

class VoiturePNJ : public Voiture
{
    public :
        static const int MAX_QUALITY = 2;
    public:
        VoiturePNJ(Ville * v);
        virtual ~VoiturePNJ();

        void setGPS(GPS * gps); //Will be deleted

        void setQualiteDessin(int); //De 1 à MAX_QUALITY
        void dessiner();
        void enterFrame();
    private :
        void effectuerChoixGPS();
        void calculerCoordRoues();
        void actualiserAngleX();
        Ville* getVille();
    private :
        static ImportateurObj * OBJ[MAX_QUALITY];
        static int s_count;
        static QVector3D m_rouesCoord[4];

        QVector3D m_coordRouesAbsolues[4];

        Ville * m_ville;
        GPS * m_GPS;
        GPS::Iterateur m_cible;
        int m_qualiteDessin;

        static const double VITESSE_MAX;
        static const double DISTANCE_ARRIVEE;
        static const double ANGLE_OK;
        static const double SCALE;
        static const double FROTTEMENT_FREINAGE;
		static const double VITESSE_MIN_FREINAGE;
};

#endif // VOITUREPNJ_H
