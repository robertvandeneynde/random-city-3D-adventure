#include "ObjetControlable.h"

#include "CameraFreeFly.h"

#include <QVector2D>
#include <cmath>

ObjetControlable::ObjetControlable()
{
}

ObjetControlable::~ObjetControlable()
{

}

void ObjetControlable::placerCameraParamPos(CameraFreeFly * camera, double recul, double hauteur, double avancement)
{
    placerCameraParamPos(camera, recul, hauteur, avancement, this->angle);
}

void ObjetControlable::placerCameraParamPos(CameraFreeFly *camera, double recul, double hauteur, double avancement, double angleAUtiliser)
{
    QVector3D direction(cos(angleAUtiliser), sin(angleAUtiliser), 0);
    camera->setPosition(position - recul * direction + QVector3D(0, 0, hauteur));
    camera->regarderVers(position + avancement * direction);
}

void ObjetControlable::placerCameraParamAngle(CameraFreeFly * camera, double recul, double hauteur, double angleRegard)
{
    placerCameraParamPos(camera, recul, hauteur, hauteur * tan(angleRegard) - recul);
}
