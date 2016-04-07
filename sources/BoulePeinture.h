#ifndef BOULEPEINTURE_H
#define BOULEPEINTURE_H

#include "ObjetScene.h"
#include "Ville.h"

#include <QVector3D>
#include <QColor>

class BoulePeinture : public ObjetScene
{
	public:
		//Ajoute la boule à la ville
		BoulePeinture(Ville*, QVector3D vitesse_initiale, QColor couleur_);
		virtual ~BoulePeinture() {}
		void enterFrame();
		void dessiner();

		QVector3D vitesse; //Vitesse en unit/secondes
		const QColor couleur;
		double angleY;

	private :
		VertexArray m_sommets;
		Ville* m_ville;
		Ville::HandleBoulePeinture m_handleVille;
};

#endif // BOULEPEINTURE_H
