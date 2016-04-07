#include "Voiture.h"
#include "Ville.h"

#include <cmath>

const double Voiture::POURCENTAGE_FROTTEMENT = 0.95; //Pourcentage par frame

Voiture::Voiture()
{
}

void Voiture::deplacer()
{
    Ville* ville = getVille();

    position += cos(angleX) * m_vitesse.toVector3D();
    angle += m_vitesseAngle;

    //Frottement
    /* Rappel :
      v *= f
      v  = v * f
      v  = v + v*f - v
      v  = v + (v*f - v)
      v += v*f - v
      v += (f-1) * v
    */
    m_vitesse *= POURCENTAGE_FROTTEMENT;
    m_vitesseAngle = 0;

    position.setZ( ville->hauteurEffective(position.x(),position.y()) );

    //Verification de changement de zone dans la ville
    QPoint nouvel_endroit = ville->calculerZone(this);
    if(m_endroitVille != nouvel_endroit)
    {
        ville->changementDeZone(this, m_endroitVille, nouvel_endroit);
        m_endroitVille = nouvel_endroit;
    }
}
