#include "BoulePeinture.h"
#include "Dessinateur.h"

#include "Parametres.h"

#include "Element.h"

#include <QDebug>
#include <QGLWidget>
#include <QMatrix4x4>


BoulePeinture::BoulePeinture(Ville* ville, QVector3D vitesse_initiale, QColor couleur_)
	: m_ville(ville), vitesse(vitesse_initiale), couleur(couleur_)
{
	angleY = 0;
	m_handleVille = m_ville->ajouterBoulePeinture(this);
	m_sommets.setModeAffichage(GL_QUADS);
	const double l = 0.1;
	Dessinateur(m_sommets).cubeDroitCentre(0, l*3, 0, l, 0, l, couleur);
}

void BoulePeinture::enterFrame()
{
	const double gravite = 10; // u/s²
	vitesse -= gravite * Parametres::DT * QVector3D(0,0,1); // u/s = u/s² * s/fr * 1fr
	position += vitesse * Parametres::DT; //u = u/s * s/fr * 1fr
	double leZ = vitesse.normalized().z();
	angleY = -asin(leZ);
	//angleX += Math::enRadians(90) * Parametres::DT; // rad/s * s/fr

	if( position.z() < m_ville->hauteurEffective( position.toPointF() ) )
	{
		Element* elem = m_ville->at( position.toPointF() );
		if(Batiment* bat = dynamic_cast<Batiment*>(elem))
			bat->setCouleur(couleur);

		m_ville->enleverBouleDePeinture(this, m_handleVille); //May delete
	}
}

void BoulePeinture::dessiner()
{
	Repere r; r.translate(position).rotate(angle, Repere::Z).rotate(angleX, Repere::X).rotate(angleY, Repere::Y);
	m_sommets.dessiner();
}

