#include "Vehicule.h"

#include "Perso.h"
#include "Ville.h"

Vehicule::Vehicule(Ville * ville)
    : m_ville(ville)
{
    m_perso = NULL;
}

Vehicule::~Vehicule()
{

}

void Vehicule::setPerso(Perso * perso)
{
    m_perso = perso;
}

void Vehicule::action()
{
    if(m_perso)
        m_perso->sortirDuVehicule();
}

void Vehicule::enterFrame()
{
    if(m_perso)
    {
        m_perso->position = position;
        m_perso->angle = angle;
    }
}
