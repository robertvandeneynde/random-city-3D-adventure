#include "Zone.h"
#include "Ville.h"
#include "SousVille.h"
#include "Element.h"
#include "Bloc.h"

#include <algorithm>

///////////
// ZONE  //
///////////
Zone::Zone(SousVille *ville)
	: m_sousVille(ville)
	, s(ville->zone())
{}

void Zone::decouperBatiments(Bloc * bloc)
{
	//Description dans le rapport
	Inter hasard[2];
	bool decouper[2];
	for(int d = 0 ; d < 2 ; d++)
	{
		hasard[d] = s[d].reduit(1, 0);
		decouper[d] = (hasard[d].longueur() <= 0) ? false : rand() % 2;
	}

	if(! (decouper[0] || decouper[1]))
		bloc->nouveauBatiment(*this);
	else
	{
		std::vector<Inter> inter[2];
		for(int d = 0 ; d < 2 ; d++)
		{
			inter[d].assign(1, s[d]);
			if(decouper[d])
				inter[d].push_back( inter[d].front().couperGetSuivant( hasard[d].hasard() ) );
		}

		for(int i = 0 ; i < (int)inter[0].size() ; i++)
			for(int j = 0 ; j < (int)inter[1].size() ; j++)
				nouvelleZone(inter[0][i], inter[1][j]).decouperBatiments(bloc);
	}
}

void Zone::decouperBlocs(int tailleRoute)
{
	//Méthode de découpe, description dans le rapport
	const int minBlock = getVille()->params().minBlockSize;
	const int maxBlock = getVille()->params().maxBlockSize;

	bool decouper[2];
	for(int d = 0 ; d < 2 ; d++)
		decouper[d] = s[d].longueur() > maxBlock;

	if(! (decouper[0] || decouper[1]) )
		getVille()->ajouterBloc( new Bloc(*this) );
	else
	{
		SousVille* sousVilleEnfant = new SousVille(m_sousVille, s);

		sousVilleEnfant->setHauteurCaracteristique(
			m_sousVille->hauteurCaracteristique()
			+ (0.20 + Math::randUniform() * 0.20) * getVille()->params().moyenneH
		);

		std::vector<Inter> inter[2];
		for(int d = 0 ; d < 2; d++)
			inter[d].assign(1, s[d]);

		for(int d = 0 ; d < 2 ; d++)
			if(decouper[d])
				tailleRoute = std::min(tailleRoute, s[d].longueur() - 2*minBlock);

		Inter interCarre[2];
		Route* route[2] = {NULL, NULL};

		for(int d = 0 ; d < 2 ; d++)
		{
			if(decouper[d])
			{
				int hasard = s[d].reduit(minBlock, minBlock + tailleRoute - 1).hasard();
				inter[d].push_back( inter[d].front().couperGetSuivant(hasard) ); // [0001111]
				interCarre[d] = inter[d].back().couperGetPrecedent(inter[d].back().debut() + tailleRoute); //[000R111]

				StructZone zoneRoute = StructZone::fromDirection(d, interCarre[d], s[1-d]);
				route[d] = new Route(Zone(sousVilleEnfant, zoneRoute), QPoint(d == 1, d == 0));
				getVille()->ajouterRoute(route[d]);
				route[d]->creerCarrefours3AuxBouts();
			}
		}

		if(route[0] && route[1])
			getVille()->ajouterCarrefour4( new Carrefour4(Zone(sousVilleEnfant, interCarre[0], interCarre[1]), route[0], route[1]) );

		int nouvelleTailleRoute = std::max(tailleRoute-1, 1);
		for(int i = 0 ; i < (int)inter[0].size() ; i++)
			for(int j = 0 ; j < (int)inter[1].size() ; j++)
				Zone(sousVilleEnfant, inter[0][i], inter[1][j]).decouperBlocs(nouvelleTailleRoute);
	}
}
/*
void Zone::decouperBlocs(int tailleRoute)
{
	const int minBlock = getVille()->params().minBlockSize;
	const int maxBlock = getVille()->params().maxBlockSize;

	bool decouper[2];
	for(int i = 0 ; i < 2 ; i++)
		decouper[i] = s[i].longueur() > maxBlock;

	if(! (decouper[0] || decouper[1]) )
		getVille()->ajouterBloc( new Bloc(*this) );
	else
	{
		std::vector<Inter> inter[2];
		Inter interCarre[2];
		Route* route[2] = {NULL, NULL};

		for(int i = 0 ; i < 2 ; i++)
		{
			inter[i].assign(1, s[i]);
			if(decouper[i])
				tailleRoute = min(tailleRoute, s[i].longueur() - 2*minBlock);
		}

		for(int i = 0 ; i < 2 ; i++)
		{
			if(decouper[i])
			{
				Inter hasard = s[i].reduit(minBlock, minBlock + tailleRoute - 1);
				inter[i].push_back( inter[i].front().couperGetSuivant( hasard.hasard() ) ); // [0001111]
				interCarre[i] = inter[i].back().couperGetPrecedent(inter[i].back().debut() + tailleRoute); //[000R111]

				Inter zoneRoute[2];
				zoneRoute[i] = interCarre[i];
				zoneRoute[1-i] = s[1-i];

				route[i] = new Route(nouvelleZone(zoneRoute[0], zoneRoute[1]), QPoint(i == 1, i == 0));
				getVille()->ajouterRoute(route[i]);
				route[i]->creerCarrefours3AuxBouts();
			}
		}

		if(route[0] && route[1])
			getVille()->ajouterCarrefour4( new Carrefour4(nouvelleZone(interCarre[0], interCarre[1]), route[0], route[1]) );

		int nouvelleTailleRoute = max(tailleRoute-1, 1);
		for(int i = 0 ; i < (int)inter[0].size() ; i++)
			for(int j = 0 ; j < (int)inter[1].size() ; j++)
				nouvelleZone(inter[0][i], inter[1][j]).decouperBlocs(nouvelleTailleRoute);
	}
}
*/
void Zone::lierElement(Element * cible)
{
	getVille()->lierElements(*this, cible);
}
void Zone::eleverA(double hauteur)
{
	for(int x = s.x.debut() ; x <= s.x.fin() ; x++)
		for(int y = s.y.debut() ; y <= s.y.fin() ; y++)
			getVille()->setHauteurBase(x,y,hauteur);
}
double Zone::hauteurBaseCoinDebut() const
{
	return getVille()->getHauteurBase(s.x.debut(), s.y.debut());
}
double Zone::hauteurBaseCoinFin() const
{
	return getVille()->getHauteurBase(s.x.fin(), s.y.fin());
}
double Zone::hauteurBaseCoin(int x, int y) const
{
	return getVille()->getHauteurBase(s.x.debut() + x * s.x.longueur(), s.y.debut() + y * s.y.longueur());
}

SousVille* Zone::getSousVille() const
{
	 return m_sousVille;
}

Ville* Zone::getVille() const
{
	return m_sousVille->getVille();
}
