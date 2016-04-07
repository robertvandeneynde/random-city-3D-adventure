#ifndef OBJETSCENE_H
#define OBJETSCENE_H

#include <QVector3D>

class ObjetScene
{
    public:
        ObjetScene();

        virtual void dessiner() = 0;
        virtual void enterFrame() = 0;
    public :
        QVector3D position;
        double angle;
        double angleX;
};

#endif // OBJETSCENE_H
