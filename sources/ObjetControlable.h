#ifndef OBJETCONTROLABLE_H
#define OBJETCONTROLABLE_H

#include <QVector3D>
#include "CameraFreeFly.h"
#include "NoeudDijkstra.h"
#include "ObjetScene.h"

class ObjetControlable : public virtual ObjetScene, public NoeudDijkstra
{
    public :
        ObjetControlable();
        virtual ~ObjetControlable();
        virtual void placerCamera(CameraFreeFly *) = 0;

        //Fonctions appelées enterFrame
        virtual void avancer() {}
        virtual void reculer() {}
        virtual void gauche() {}
        virtual void droite() {}
        virtual void haut() {}
        virtual void bas() {}
        //Fonction press
        virtual void saut() {}
        virtual void action() {}
		virtual void action2() {}
        QVector3D position3D() const { return this->position; }
    protected :
        void placerCameraParamPos(CameraFreeFly * camera, double recul, double hauteur, double avancement);
        void placerCameraParamPos(CameraFreeFly * camera, double recul, double hauteur, double avancement, double angleAUtiliser);
        void placerCameraParamAngle(CameraFreeFly * camera, double recul, double hauteur, double angleRegard);
};

#endif // OBJETCONTROLABLE_H
