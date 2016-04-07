#include "Element.h"

#include "Ville.h"

#include "fonctionsMathematiques.h"

#include "Inter.h"
#include "Dessinateur.h"
#include "Bloc.h"

#include <algorithm>
#include <QGLWidget>
#include <QDebug>
#include <cmath>
#include <cassert>

Element::Element(Zone zone)
    : m_zone(zone)
    , m_hauteur(0.01)
	, m_hauteurMin(0)
{
    m_zone.lierElement(this);
}

Element::~Element()
{

}

QChar Element::caractereMap()
{
    return '?';
}

double Element::angleMontee()
{
    return 0;
}

double Element::hauteurEffective(double /*x*/, double /*y*/)
{
    return m_hauteur + m_hauteurMin;
}

double Element::hauteurEffectiveVoiture(double /*x*/, double /*y*/)
{
    return 2e9;
}

void Element::mettreAJourHauteurs()
{
	m_hauteurMin = hauteurMin();
}

double Element::hauteurMin()
{
    double leMin = 1e9;
    StructZone z = m_zone;
    for(int x = z.x.debut() ; x <= z.x.fin() ; x++)
        for(int y = z.y.debut() ; y <= z.y.fin() ; y++)
            leMin = std::min(leMin, m_zone.getVille()->getHauteurBase(x,y));
    return leMin;
}


/////////////////////
// ELEMENT DE BLOC //
/////////////////////

bool ElementDeBloc::enBordure()
{
    StructZone z1 = m_bloc->zone(), z2 = zone();
    return z1.x.debut() == z2.x.debut() || z1.x.fin() == z2.x.fin() || z1.y.debut() == z2.y.debut() || z1.y.fin() == z2.y.fin();
}

//////////////
// BATIMENT //
//////////////

Batiment::Batiment(Zone zone, Bloc * bloc)
    : ElementDeBloc(zone, bloc)
	, m_couleur(Qt::white)
{

}

void Batiment::remplir(RemplisseurDeScene & remplisseur)
{
	VertexArray::HandleBuilder builder = remplisseur.dessinateur().handleBuilder();
    remplisseur.dessinateur().cubeDroit(m_zone, m_hauteurMin, m_hauteur, m_couleur);
	m_handlesDessins.push_back( builder() );
}

void Batiment::setCouleur(QColor couleur)
{
	if(m_couleur != couleur)
	{
		m_couleur = couleur;
		for(VertexArray::Handle & handle : m_handlesDessins)
			handle.setCouleur(couleur);
	}
}

void Batiment::setHauteur(double h)
{
    m_hauteur = h;
}

void Batiment::setIndice(int indice)
{
    m_indice = indice;
}
int Batiment::indice()
{
    return m_indice;
}

QChar Batiment::caractereMap()
{
    return 'A' + m_indice;
}

////////////
// Jardin //
////////////

Jardin::Jardin(Zone zone, Bloc * bloc)
    : ElementDeBloc(zone, bloc)
{

}

void Jardin::remplir(RemplisseurDeScene & remplisseur)
{
    remplisseur.dessinateur().cubeDroitTexture(m_zone, m_hauteurMin, 0.01, remplisseur.textureSet().JARDIN);
}

QChar Jardin::caractereMap()
{
    return ' ';
}

/////////////////
// RouteSimple //
/////////////////

RouteOrientation::RouteOrientation(QPoint orientation)
    : m_orientation(orientation)
    , m_verticale(orientation.x() == 0)
{
    assert(orientation == QPoint(1,0) || orientation == QPoint(0,1));
}

RouteSimple::RouteSimple(Zone zoneLiee, Route * principale)
    : Element(zoneLiee)
    , RouteOrientation(principale->orientation())
    , m_principale(principale)
    , m_largeur(StructZone(zoneLiee)[perp()].longueur())
    , m_longueur(StructZone(zoneLiee)[paral()].longueur())
{
    StructZone zone = m_zone;
    int point[2];
	double pos[2];

    point[perp()] = zone[perp()].debut();
    for(int i = 0 ; i < 2 ; i++)
    {
        point[paral()] = zone[paral()].agrandi(1).getC(i);
        Carrefour * carrefour = m_debouchees[i] = dynamic_cast<Carrefour*>(m_zone.getVille()->at(point));
		m_bords[i][0] = m_bords[i][1] = NULL;
        if(carrefour)
        {
			pos[paral()] = zone[paral()][i];
            for(int j = 0 ; j < 2 ; j++)
            {
                double facteur = m_largeur == 1 ? 0.50 : 0.75 - (verticale() ? j : 1-j) * 0.50;
				pos[perp()] = zone[perp()].getPondere(facteur);
				QVector2D position(pos[0], pos[1]);
                if(i == j) //Pour rouler a gauche : i != j
					m_bords[i][j] = new SortieCarrefour(this, carrefour, position);
                else
					m_bords[i][j] = new EntreeCarrefour(this, carrefour, position);
            }
        }
    }

	if(m_bords[0][0] && m_bords[1][0])
    {
		m_bords[0][0]->ajouterAcces(m_bords[1][0]);
		m_bords[1][1]->ajouterAcces(m_bords[0][1]);
        /* Rouler a gauche
		m_bords[1][0]->ajouterAcces(m_bords[0][0]);
		m_bords[0][1]->ajouterAcces(m_bords[1][1]);
        */
    }

    m_triangles[0] = m_triangles[1] = false;
}

RouteSimple::~RouteSimple()
{
    for(int i = 0 ; i < 2 ; i++)
        for(int j = 0 ; j < 2 ; j++)
			delete m_bords[i][j];
}

void RouteSimple::setTriangles(int num)
{
    m_triangles[num] = true;
}

QPoint RouteSimple::directionVers(StructZone zone)
{
    return Math::pointFromDirection(perp(), 0, zone[paral()].debut() < StructZone(m_zone)[paral()].debut() ? -1 : 1);
}

QPoint Route::directionVers(StructZone zone)
{
    int point[2];
    point[perp()] = 0;
    point[paral()] = zone[paral()].debut() < StructZone(m_zone)[paral()].debut() ? -1 : 1;
    return QPoint(point[0], point[1]);
}

int RouteSimple::numeroDirectionDe(StructZone zone)
{
    return zone[paral()].debut() < StructZone(m_zone)[paral()].debut() ? 0 : 1;
}

int Route::numeroDirectionDe(StructZone zone)
{
    return zone[paral()].debut() < StructZone(m_zone)[paral()].debut() ? 0 : 1;
}

Carrefour * RouteSimple::getCarrefour(int i)
{
    StructZone zone = m_zone;
    int point[2];
    point[paral()] = zone[paral()].agrandi(1).getC(i);
    point[perp()] = zone[perp()].debut();
    return dynamic_cast<Carrefour*>(m_zone.getVille()->at(point));
}

void RouteSimple::mettreAJourHauteurs()
{
	//La zone
	StructZone zone = m_zone;

	double hDebut = m_zone.hauteurBaseCoinDebut();
	double hFin = m_zone.hauteurBaseCoinFin();
	double pas = (hFin - hDebut) / zone[paral()].longueur();

	Inter reduit = zone[paral()].reduit(1);
	StructZone laZone = m_zone;
	laZone[perp()] = zone[perp()];
	for(int p = reduit.debut() ; p <= reduit.fin() ; p++)
	{
		laZone[paral()] = Inter(p,p);
		int l = p - zone[paral()].debut();
		m_zone.nouvelleZone(laZone).eleverA(hDebut + l * pas);
	}

	//Les Noeuds Dijkstra
	for(int	i = 0; i < 2; i++)
	for(int j = 0; j < 2; j++)
		if(m_bords[i][j])
			m_bords[i][j]->refreshPoidsDijkstra();
}

double RouteSimple::hauteurEffective(double x, double y)
{
    double posRel[2] = {x - m_zone.mX().debut(), y - m_zone.mY().debut()};

    double h1 = m_zone.hauteurBaseCoinDebut();
    double h2 = m_zone.hauteurBaseCoinFin();

    double pente = (h2 - h1) / m_longueur;

    return h1 + pente * posRel[paral()];
}

double RouteSimple::hauteurEffectiveVoiture(double x, double y)
{
    return hauteurEffective(x,y);
}

double RouteSimple::angleMontee()
{
    return atan2(m_zone.hauteurBaseCoinFin() - m_zone.hauteurBaseCoinDebut(), m_longueur);
}

void RouteSimple::remplirTriangles(RemplisseurDeScene & remplisseur, QPoint direction)
{
	using namespace Math::Matrice;
    VertexArray & sommets = remplisseur.vertexArray();
	Pile& pile = sommets.pileMatrice();
	Pile::Nouveau r(pile);

    double matrice[16];
	translation(matrice, m_zone.mX().moyenne(), m_zone.mY().moyenne(), 0);
	pile.multiplier(matrice);

	rotationZ(matrice, -Math::PI/2 + atan2(direction.y(), direction.x()));
	pile.multiplier(matrice);

	translation(matrice, m_largeur == 1 ? - m_largeur / 2.0 : 0, m_longueur/2.0, 0);
	pile.multiplier(matrice);

    remplisseur.dessinateur().trianglesPriorite(m_zone.getVille(), m_largeur == 1 ? m_largeur : m_largeur / 2.0, 5);
}

void RouteSimple::remplirBase(RemplisseurDeScene & remplisseur)
{
    remplisseur.dessinateur().cubePenche(m_zone, 0.01, 0x333333);
}

void RouteSimple::remplir(RemplisseurDeScene & remplisseur)
{
    if(m_largeur > 1)
        remplirBandeDeSeparation(remplisseur);

    for(int i = 0 ; i < 2 ; i++)
    {
		if(m_bords[i][1-i])
        {
            if(m_triangles[i])
                remplirTriangles(remplisseur, m_orientation * (i == 0 ? -1 : 1));
        }
    }

    remplirBase(remplisseur);
}

void RouteSimple::remplirBandeDeSeparation(RemplisseurDeScene & remplisseur)
{
    remplisseur.dessinateur().bandeDeSeparation(m_zone, paral());
}

void RouteSimple::remplirBandeDeSeparationPointille(RemplisseurDeScene & remplisseur)
{
    remplisseur.dessinateur().pointilles(m_zone, paral());
}

QChar RouteSimple::caractereMap()
{
    return verticale() ? 0x2502 : 0x2500; //Voir aussi : 0x25B3, 0x2190, 0x2500
}

void RouteSimple::attribuerIndiceGPS(int & indice)
{
    for(int i = 0 ; i < 2 ; i++)
		if(m_bords[i][0])
            for(int j = 0 ; j < 2 ; j++)
				m_bords[i][j]->setIndice(indice++);
}

///////////
// Route //
///////////
Route::Route(Zone zone, QPoint orientation)
    : Element(zone)
    , RouteOrientation(orientation)
    , m_largeur(zone.mStruct()[perp()].longueur())
    , m_longueur(zone.mStruct()[paral()].longueur())
{
    m_sousZones.push_back(m_zone);
}

Route::~Route()
{
    for(int i = 0 ; i < (int)m_sousRoutes.size() ; i++)
        delete m_sousRoutes[i];
}

RouteSimple * Route::getRouteExtreme(int numero)
{
    assert(! m_sousRoutes.empty());
    return numero == 0 ? m_sousRoutes.front() : m_sousRoutes.back();
}

void Route::couper(StructZone carrefour)
{
    Inter interCarr = carrefour[paral()];

    StructZone laZone = m_zone;
    std::vector<Zone> nouveau;
    for(Zone zone : m_sousZones)
    {
        Inter interv = StructZone(zone)[paral()];
        if(! interv.intersects( interCarr ))
            nouveau.push_back( zone );
        else
        {
            laZone[paral()] = interv.couperGetPrecedent( interCarr.debut() );
            if(laZone[paral()].longueur() > 0)
                nouveau.push_back( m_zone.nouvelleZone(laZone) );

            laZone[paral()] = interv.couperGetSuivant( interCarr.fin() );
            if(laZone[paral()].longueur() > 0)
                nouveau.push_back( m_zone.nouvelleZone(laZone) );
        }
    }
    m_sousZones = nouveau;
}

void Route::creerSousRoutes()
{
    for(Zone zone : m_sousZones)
        m_sousRoutes.push_back( new RouteSimple(zone, this) );

    m_sousRoutes.front()->setTriangles(0);
    m_sousRoutes.back()->setTriangles(1);
}

void Route::creerCarrefours3AuxBouts()
{
    StructZone zone = m_zone;
    int point[2];

    for(int cote = 0 ; cote < 2 ; cote++)
    {
        Route * routePrimaire = NULL; //Route deja existante
        Carrefour3Multiple * carrefourExistant = NULL; //Carrefour deja existant

        point[paral()] = zone[paral()].agrandi(1).getC(cote);
        int & vari = point[perp()];
        for(vari = zone[perp()].debut() ; vari < zone[perp()].fin() ; vari++)
        {
            Element * elem = m_zone.getVille()->at(point);
            routePrimaire = dynamic_cast<Route*>(elem);
            if(routePrimaire)
                break;
            carrefourExistant = dynamic_cast<Carrefour3Multiple*>(elem);
        }

        if(carrefourExistant)
            routePrimaire = carrefourExistant->routePrincipale();
        if(routePrimaire)
        {
            Route* rx = verticale() ? this : routePrimaire;
            Route* ry = verticale() ? routePrimaire : this;
            StructZone zoneCarr(rx->zone().x, ry->zone().y);

			m_zone.getVille()->creerCarrefour3(Zone(routePrimaire->sousVille(), zoneCarr), routePrimaire, this);
            routePrimaire->couper(zoneCarr);
        }
    }
}

void Route::remplir(RemplisseurDeScene & remplisseur)
{
    for(int i = 0 ; i < (int)m_sousRoutes.size() ; i++)
        m_sousRoutes[i]->remplir(remplisseur);
}

QChar Route::caractereMap()
{
    return m_sousRoutes.front()->caractereMap();
}

void Route::mettreAJourHauteurs()
{
	for(RouteSimple* sousRoute : m_sousRoutes)
		sousRoute->mettreAJourHauteurs();
}

void Route::attribuerIndiceGPS(int & indice)
{
	for(RouteSimple* sousRoute : m_sousRoutes)
		sousRoute->attribuerIndiceGPS(indice);
}

/////////////////////
// BordCarrefour //
/////////////////////

VertexArray Avertissement::DESSIN_FLECHE;

Avertissement::Avertissement()
{
    REMPLIR_MODELES();
}

Avertissement::~Avertissement()
{

}

void Avertissement::REMPLIR_MODELES()
{
    if(DESSIN_FLECHE.length())
		return;
    //DESSIN_FLECHE = new VertexArray();
    VertexArray& va = DESSIN_FLECHE;
	const double epaisseur(0.1), longueur(0.3), hauteur(0.3);

	va.setModeAffichage(GL_QUADS);
	const double points[4][3] = {
		{-longueur/2, -epaisseur/2, hauteur*1/3},
		{+longueur/2, -epaisseur/2, hauteur*1/3},
		{0, -epaisseur/2, -hauteur*2/3},
		{0, -epaisseur/2, -hauteur*2/3},
	};
	for(const double * point : points)
	{
		va.nouvellePosition(point);
		va.nouvelleCouleur(Qt::red);
		va.nouvelleNormale(0,-1,0);
	}
	va.extrude( {0, epaisseur, 0} );
	//Dessinateur(va).cubeDroit(-longueur/2, longueur, -epaisseur/2, epaisseur, 0, hauteur, Qt::red);
}

BordCarrefour::BordCarrefour(RouteSimple * route, Carrefour * carrefour, QVector2D position)
    : m_route(route)
	, m_carrefour(carrefour)
	, m_position2D(position)
{
    m_carrefour->ajouterEntree(this);
}

void BordCarrefour::observerVoitures()
{
	if(m_priorites.empty())
		return;

	Ville* ville = m_carrefour->ville();

	QVector2D direction( m_route->directionVers( m_carrefour->zone() ) );
	StructZone z = m_route->zone();
	for(int x = z.x.debut(); x < z.x.fin(); x++)
	{
		for(int y = z.y.debut(); y < z.y.fin(); y++)
		{
			for(Voiture* voiture : ville->voituresA( QPoint(x,y) ))
			{
				double produitVitesse = QVector2D::dotProduct(direction, voiture->getVitesse());
				double produitPosition = QVector2D::dotProduct(direction, m_position2D - QVector2D(voiture->position));
				//Projection = direction * produit => Projection.length() == produit
				double tempsSecondes = (produitPosition / produitVitesse) * Parametres::DT; // s = (u / (u/fr)) * s/fr
				if(tempsSecondes > 0 && tempsSecondes < 5)
				{
					QVector2D maPosition2D( this->position3D() );

					for(BordCarrefour* prio : m_priorites)
					{
						QVector2D saPosition2D( prio->position3D() );
						QVector2D laDroite( prio->getRoute()->directionVers( prio->getCarrefour()->zone() ) ); //Direction face
						laDroite = QVector2D(laDroite.y(), -laDroite.x()); //Tourne
						bool droite = QVector2D::dotProduct(maPosition2D - saPosition2D, laDroite) > 0;

						prio->ajouterAvertissement( new Priorite(this, tempsSecondes, droite) );
					}
					//ajouterAvertissement( new Priorite(this, tempsSecondes) );
				}
			}
		}
	}
}

BordCarrefour::~BordCarrefour()
{

}

void BordCarrefour::mettrePrioriteSur(BordCarrefour * autre)
{
    m_priorites.push_back(autre);
}

void BordCarrefour::resetObservations()
{
	for(Avertissement* a : m_avertissements)
		delete a;
	m_avertissements.clear();
}

void BordCarrefour::ajouterAvertissement(Avertissement * avertissement)
{
	m_avertissements.push_back(avertissement);
}

void BordCarrefour::dessiner()
{
	if(! m_avertissements.empty())
    {
        Repere r;
		r.translate(position3D() + QVector3D(0, 0, 0.3));
        r.rotateYZ(QVector3D( m_route->directionVers(m_carrefour->zone()) ), QVector3D(0,0,1));
		for(Avertissement* a : m_avertissements)
		{
			a->dessiner();
			r.translate( QVector3D(0, 0, 0.3 * 1.25) );
		}
	}
}

BordCarrefour::Priorite::Priorite(BordCarrefour *source, double temps, bool droite)
	: m_source(source)
	, m_temps(temps)
	, m_sens(droite ? -1 : 1)
{
}

void BordCarrefour::Priorite::dessiner()
{
	Repere r2; r2.rotate(m_sens * Math::PI/2, Repere::Y);
	DESSIN_FLECHE.dessiner();
}

QVector3D BordCarrefour::position3D() const
{
	return QVector3D(m_position2D.x(), m_position2D.y(), m_carrefour->altitude());
}

EntreeCarrefour::EntreeCarrefour(RouteSimple * r, Carrefour * c, QVector2D position)
    : BordCarrefour(r,c,position)
{}

SortieCarrefour::SortieCarrefour(RouteSimple * r, Carrefour * c, QVector2D position)
    : BordCarrefour(r,c,position)
{}

///////////////
// Carrefour //
///////////////

NoeudDijkstra * Carrefour::noeudLePlusProche(QVector3D pos)
{
	if(m_bords.empty())
        return this;

	NoeudDijkstra * cible = m_bords.front();
	double mini = (cible->position3D() - pos).lengthSquared();
	for(BordCarrefour* bord : m_bords)
    {
		double d = (bord->position3D() - pos).lengthSquared();
		if(d < mini) {
			cible = bord;
            mini = d;
        }
    }
    return cible;
}

void Carrefour::ajouterEntree(BordCarrefour * nouvelle)
{
    NoeudDijkstra::ajouterAccesSymetrique(this, nouvelle);
    if(std::max(m_zone.mX().longueur(), m_zone.mY().longueur()) > 1)
		for(BordCarrefour* autre : m_bords)
            NoeudDijkstra::ajouterAccesSymetrique(autre, nouvelle);

	m_bords.push_back(nouvelle);
}

void Carrefour::eleverA(double hauteur)
{
    m_zone.eleverA(hauteur);
    m_hauteurMin = m_hauteurCarrefour = hauteur;
	refreshPoidsDijkstra();
}

void Carrefour::propagerHauteur(std::function<double(double)> f, std::set<Carrefour*>& aEviter)
{
	std::set<Carrefour*> autres;

	for(BordCarrefour* bord : m_bords)
	{
		for(int i = 0; i < 2; i++)
		{
			Carrefour* autre = bord->getRoute()->debouchee(i);
			if(autre && autre != this && autre->sousVille() == this->sousVille() && aEviter.find(autre) == aEviter.end())
				autres.insert(autre);
		}
	}
	aEviter.insert(this);
	for(Carrefour* autre : autres)
	{
		autre->eleverA( f(m_hauteurCarrefour) );
		autre->propagerHauteur(f, aEviter);
	}
}

QVector3D Carrefour::position3D() const
{
    return QVector3D(m_zone.mX().moyenne(), m_zone.mY().moyenne(), m_hauteurCarrefour);
}

void Carrefour::dessiner()
{
	for(BordCarrefour* entree : m_bords)
        entree->dessiner();
}

void Carrefour::montrerLiaisons(VertexArray & sommets, float r, float g, float b, double dz)
{
    MapAcces const& map = noeudsAccessibles();
    QVector3D maPosition = position3D();
    for(MapAcces::const_iterator it = map.begin() ; it != map.end() ; ++it)
    {
        QVector3D autre = it->first->position3D();

        for(int i = 0 ; i < 2 ; i++)
        {
            sommets.nouvellePosition(maPosition.x(), maPosition.y(), maPosition.z() + 0.5);
            sommets.nouvelleCouleur(r,g,b);
            sommets.nouvelleNormale(0,0,1);
        }

        for(int i = 0 ; i < 2 ; i++)
        {
            sommets.nouvellePosition(autre.x(), autre.y(), autre.z() + 0.5 + dz);
            sommets.nouvelleCouleur(r*0.70, g*0.70, b*0.70);
            sommets.nouvelleNormale(0,0,1);
        }
    }
}

double Carrefour::hauteurEffectiveVoiture(double x, double y)
{
    return hauteurEffective(x,y);
}

double Carrefour::altitude()
{
    return m_hauteurCarrefour;
}

void Carrefour::observerVoitures()
{
	for(BordCarrefour* entree : m_bords)
        entree->resetObservations();
	observerVoituresDansCarrefour();
	for(BordCarrefour* entree : m_bords)
        entree->observerVoitures();
}

void Carrefour::observerVoituresDansCarrefour()
{
	if(m_bords.empty())
		return;

	StructZone z = zone();
	for(int x = z.x.debut(); x < z.x.fin(); x++)
	{
		for(int y = z.y.debut(); y < z.y.fin(); y++)
		{
			if(! ville()->voituresA( QPoint(x,y) ).empty())
			{
				for(BordCarrefour* entree : m_bords)
					if(dynamic_cast<EntreeCarrefour*>(entree))
						entree->ajouterAvertissement( new AvertissementCarrefour() );
			}
		}
	}
}

void Carrefour::AvertissementCarrefour::dessiner()
{
	DESSIN_FLECHE.dessiner();
}

////////////////
// Carrefour4 //
////////////////

Carrefour4::Carrefour4(Zone zone, Route * r1, Route * r2)
    : Carrefour(zone)
{
    m_routesPrincipales[0] = r1;
    m_routesPrincipales[1] = r2;
    for(int i = 0 ; i < 2; i++)
        m_routesPrincipales[i]->couper(m_zone);

    for(int i = 0 ; i < 2 ; i++)
        for(int j = 0 ; j < 2 ; j++)
            m_routesSimples[i][j] = NULL; //Sera rempli avec ajouterLesAcces
}

void Carrefour4::ajouterLesAcces()
{
    StructZone zone = m_zone;
    int point[2];

    for(int i = 0 ; i < 2 ; i++)
    {
        Route* rp = m_routesPrincipales[i];
        point[rp->perp()] = zone[rp->perp()].debut();
        for(int j = 0 ; j < 2 ; j++)
        {
            point[rp->paral()] = zone[rp->paral()].agrandi(1).getC(j);
            m_routesSimples[i][j] = dynamic_cast<RouteSimple*>( m_zone.getVille()->at(point) );
        }
    }

    RouteSimple* routesOrdre[4] = {m_routesSimples[0][0], m_routesSimples[1][0], m_routesSimples[0][1], m_routesSimples[1][1]};

    //Assert
    for(RouteSimple* routeExistante : routesOrdre)
        assert(routeExistante);

    std::map<RouteSimple*, EntreeCarrefour*> entreesCorresp;

    //Ajout
	for(BordCarrefour* bord : m_bords)
        if( EntreeCarrefour* entree = dynamic_cast<EntreeCarrefour*>(bord) )
            entreesCorresp[entree->getRoute()] = entree;

    //Liaisons
    for(int i = 0; i < 4; i++) {
        auto it = entreesCorresp.find( routesOrdre[i] );
        auto it2 = entreesCorresp.find( routesOrdre[(i+1) % 4] );
        if(it != entreesCorresp.end() && it2 != entreesCorresp.end())
            it->second->mettrePrioriteSur(it2->second);
    }
}

void Carrefour4::remplir(RemplisseurDeScene & remplisseur)
{
    remplirLignes(remplisseur);

    remplisseur.dessinateur().cubeDroit(m_zone, m_hauteurMin, 0.01, 0x333333);
}

void Carrefour4::remplirLignes(RemplisseurDeScene & remplisseur)
{
    VertexArray & sommets = remplisseur.vertexArray();
	Math::Matrice::Pile & pile = sommets.pileMatrice();
	Math::Matrice::Pile::Nouveau r(pile);

    double lDemi = m_zone.mX().longueur() / 2.0;

    double matrice[16];
    Math::Matrice::translation(matrice, m_zone.mX().debut() + lDemi, m_zone.mY().debut() + lDemi, m_zone.hauteurBaseCoinDebut());
	pile.multiplier(matrice);

    const double posX = m_zone.mX().longueur() == 1 ? -0.5 : 0.3;
    const double largeurBande = 0.10;
    for(int i = 0 ; i < 4 ; i++)
    {
        for(int j = 0 ; j < 4 ; j++)
        {
            int a = j/2, b = (j+1)%4 / 2;
            sommets.nouvellePosition((posX + a * (1-posX)) * lDemi, -lDemi + b * largeurBande, 0);
            sommets.nouvelleNormale(0,0,-1);
            sommets.nouvelleCouleur(0.8,0.8,0.8);
        }
        const double haut[] = {0, 0, 0.01 * 1.5};
        sommets.extrude(haut);

        sommets.transformerDernierPoint(24);

        Math::Matrice::rotationZ(matrice, Math::PI/2);
		pile.multiplier(matrice);
    }
}

QChar Carrefour4::caractereMap()
{
    return 0x253C; // 0x00D7 ou 0x2715 (multiplier) 0x253C (dessin)
}

////////////////////////
// Carrefour3Multiple //
////////////////////////

Carrefour3Multiple::Carrefour3Multiple(Zone zone, Route * principale, Route * secondaire)
    : Carrefour(zone)
{
    m_secondaires.push_back(secondaire);
    m_principale = principale;
    m_principale->couper(m_zone);
}

Carrefour3Multiple::~Carrefour3Multiple()
{

}

void Carrefour3Multiple::ajouterAutre(Carrefour3Multiple * carrefour)
{
    m_secondaires.insert(m_secondaires.end(), carrefour->m_secondaires.begin(), carrefour->m_secondaires.end());

    m_zone = m_zone.nouvelleZone( StructZone::fusion(m_zone, carrefour->m_zone) );
    m_zone.lierElement(this);
}

QChar Carrefour3Multiple::caractereMap()
{
    return m_principale->verticale() ? '|' : '-';
}

void Carrefour3Multiple::remplir(RemplisseurDeScene & remplisseur)
{
    if(m_principale->largeur() > 1)
        remplisseur.dessinateur().pointilles(m_zone, m_principale->verticale());

    remplisseur.dessinateur().cubeDroit(m_zone, m_hauteurMin, 0.01, 0x333333);
}

void Carrefour3Multiple::ajouterLesAcces()
{
    int point[2];

    point[ m_principale->perp() ] = zone()[ m_principale->perp() ].debut();
    for(int i = 0 ; i < 2 ; i++)
    {
        point[ m_principale->paral() ] = zone()[ m_principale->paral() ].agrandi(1).getC(i);
        m_principalesSimples[i] = dynamic_cast<RouteSimple*>( m_zone.getVille()->at(point) );
        //m_principalesSimples[i]->setPanneauDePriorites(i-1);
    }

    std::map<RouteSimple*, EntreeCarrefour*> entreesCorresp;

    //Ajout
	for(BordCarrefour* bord : m_bords)
        if( EntreeCarrefour* entree = dynamic_cast<EntreeCarrefour*>(bord) )
            entreesCorresp[entree->getRoute()] = entree;

    //Test
    for(RouteSimple* r : m_principalesSimples)
        if(entreesCorresp.find(r) == entreesCorresp.end())
            return; //Pas de priorites

    int posPrimaire = zone()[ m_principale->perp() ].debut();
    for(Route* routeComplete : m_secondaires)
    {
        int posSecondaire = routeComplete->zone()[ m_principale->perp() ].debut();
        int indice = posSecondaire < posPrimaire ? 0 : 1;
        RouteSimple* routeSimpleSecondaire = routeComplete->getRouteExtreme(1 - indice);
        auto entreeSecondaire = entreesCorresp.find(routeSimpleSecondaire);
        if(entreeSecondaire != entreesCorresp.end())
            for(RouteSimple* routeSimplePrincipale : m_principalesSimples)
                entreesCorresp[ routeSimplePrincipale ]->mettrePrioriteSur(entreeSecondaire->second);
            //Pour avoir uniquement la bande qui touche :
            //(Problemes rues petites, Mustang)
            //entreesCorresp[ m_principalesSimples[indice] ]->mettrePrioriteSur(entreeSecondaire->second);

    }
}
