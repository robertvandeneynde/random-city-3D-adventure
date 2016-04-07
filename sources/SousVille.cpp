#include "SousVille.h"
#include "Ville.h"

#include "fonctionsMathematiques.h"

SousVille::SousVille(SousVille *parentSousVille, StructZone zone, Ville *ville)
	: m_zone(this, zone)
	, m_hauteurCaracteristique(0)
{
	//assert(parentSousVille || ville)
	m_parent = parentSousVille;
	m_ville = ville;
	if(m_parent)
	{
		m_parent->m_enfants.insert(this);
		if(! m_ville)
			m_ville = m_parent->getVille();
	}
}

void SousVille::setHauteurCaracteristique(double h)
{
	m_hauteurCaracteristique = h;
}

SousVille::~SousVille()
{
	for(SousVille* v : m_enfants)
		delete v;
}
