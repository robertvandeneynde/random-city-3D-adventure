#ifndef ZONE_H
#define ZONE_H

#include "StructZone.h"

class Ville;
class SousVille;
class Bloc;
class Element;

class Zone
{
	public :
		//Creation
		Zone() {}
		Zone(SousVille * ville);
		Zone(SousVille * ville, StructZone str) : m_sousVille(ville), s(str) {}
		Zone(SousVille * ville, Inter leX, Inter leY) : m_sousVille(ville), s(leX, leY) {}
		Zone nouvelleZone(Inter leX, Inter leY) const { return Zone(m_sousVille, leX, leY); }
		Zone nouvelleZone(StructZone str) const { return Zone(m_sousVille, str); }

		void decouperBlocs(int tailleRoute = 1);
		void decouperBatiments(Bloc *);
		void lierElement(Element *);

		void eleverA(double hauteur);

		double hauteurBaseCoinDebut() const;
		double hauteurBaseCoinFin() const;
		double hauteurBaseCoin(int x, int y) const; //0 = debut, 1 = fin

		SousVille * getSousVille() const;
		Ville * getVille() const;

		StructZone mStruct() const { return s; }
		operator StructZone() const { return s; }
		Inter mX() const { return s.x; }
		Inter mY() const { return s.y; }
	private :
		SousVille * m_sousVille;
		StructZone s; //Should be const
};

#endif // ZONE_H
