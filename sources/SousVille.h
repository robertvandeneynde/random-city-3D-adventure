#ifndef SOUSVILLE_H
#define SOUSVILLE_H

#include <vector>
#include <set>

#include "StructZone.h"
#include "Zone.h"

class Ville;

class SousVille
{
	public:
		SousVille(SousVille* parentSousVille, StructZone zone, Ville* ville = NULL);
		virtual ~SousVille();

		void setHauteurCaracteristique(double);

		StructZone zone() { return m_zone; }
		Ville* getVille() { return m_ville; }
		SousVille* getParentVille() { return m_parent; }
		std::set<SousVille*> const& enfantsSousVille() { return m_enfants; }
		double hauteurCaracteristique() { return m_hauteurCaracteristique; }
	private :
		std::set<SousVille*> m_enfants;
		SousVille* m_parent;
		Ville* m_ville;
		Zone m_zone;
		double m_hauteurCaracteristique;
};

#endif // SOUSVILLE_H
