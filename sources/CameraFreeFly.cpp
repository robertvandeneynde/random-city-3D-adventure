#include "CameraFreeFly.h"

#include "fonctionsMathematiques.h"

#include <QMatrix4x4>
#include <GL/glu.h>

CameraFreeFly::CameraFreeFly(QVector3D haut)
    : m_haut(haut.normalized())
{
}

void CameraFreeFly::glLook()
{
    QVector3D cible = m_position + m_regard;
    gluLookAt(m_position.x(), m_position.y(), m_position.z(), cible.x(), cible.y(), cible.z(), m_haut.x(), m_haut.y(), m_haut.z());
    //QMatrix4x4 mat;
    //mat.lookAt(m_position, m_position + m_regard, m_haut);
    //glMultMatrixf(mat.constData());
}

void CameraFreeFly::setPosition(QVector3D pos)
{
    m_position = pos;
}
void CameraFreeFly::setVitesse(double v)
{
    m_vitesse = v;
}

void CameraFreeFly::setRegard(QVector3D reg)
{
    m_regard = reg;
    m_regard.normalize();
}
void CameraFreeFly::regarderVers(QVector3D point)
{
    setRegard(point - m_position);
}
void CameraFreeFly::avancer(double d)
{
    m_position += d * m_regard;
}

void CameraFreeFly::deplacer(QVector3D dep)
{
    m_position += dep;
}
void CameraFreeFly::deplacementLateral(double v)
{
    m_position += v * QVector3D::crossProduct(m_regard, m_haut).normalized();
}
void CameraFreeFly::accelerer(double a)
{
    m_vitesse += a;
}
void CameraFreeFly::tourner(double angleAutourDeHaut, double angleAutourDeLateral)
{
    QMatrix4x4 rot;
    rot.rotate(angleAutourDeHaut, m_haut);
    QVector3D droite = QVector3D::crossProduct(m_regard, m_haut);
    rot.rotate(angleAutourDeLateral, droite);

    m_regard = rot * m_regard;
    m_regard.normalize();
}
