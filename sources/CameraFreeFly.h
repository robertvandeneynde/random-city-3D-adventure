#ifndef CAMERAFREEFLY_H
#define CAMERAFREEFLY_H

#include <QVector3D>
#include <QGLWidget>

class CameraFreeFly
{
    public :
        CameraFreeFly(QVector3D haut = QVector3D(0,0,1));

        void glLook();
        void avancer(double d);

        void setPosition(QVector3D);
        void setRegard(QVector3D);
        void regarderVers(QVector3D point);
        void tourner(double angleAutourDeHaut, double angleAutourDeLateral);

        void accelerer(double);
        void setVitesse(double);

		void deplacer(QVector3D);
        void deplacementLateral(double);

        QVector3D position() { return m_position; }
        QVector3D regard() { return m_regard; }
        double vitesse() { return m_vitesse; }
    private :
        QVector3D m_position;
        QVector3D m_regard;
        double m_vitesse;

        const QVector3D m_haut;
};

#endif // CAMERAFREEFLY_H
