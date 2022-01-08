#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <vector>
#include <QPoint>
#include <QVector2D>
#include <QChar>
#include <functional>

#include "NoeudDijkstra.h"

#include "Ville.h"
#include "ListeChainee.h"
#include "Inter.h"
#include "StructZone.h"
#include "RemplisseurDeScene.h"
#include "VertexArray.h"

/////////////
// ELEMENT //
/////////////
class Element
{
public :
    Element(Zone); //Lie l'Element à la zone !
    virtual ~Element();

    StructZone zone() { return m_zone; }
    Ville* ville() { return m_zone.getVille(); }
	SousVille* sousVille() { return m_zone.getSousVille(); }

	virtual void mettreAJourHauteurs();
	virtual double hauteurEffective(double x, double y);
    virtual double hauteurEffectiveVoiture(double x, double y);
    virtual void remplir(RemplisseurDeScene & remplisseur) = 0;
    virtual double angleMontee(); //Angle que fait l'élément avec le plan xy

    virtual QChar caractereMap();
protected :
	double hauteurMin();
protected :
    Zone m_zone;
    double m_hauteur, m_hauteurMin;
};

/////////////////////
// ELEMENT DE BLOC //
/////////////////////

class Bloc;
class ElementDeBloc : public Element
{
    public :
        ElementDeBloc(Zone z, Bloc * b)
            : Element(z)
            , m_bloc(b)
        {}

        bool enBordure();
        Bloc * getBloc() { return m_bloc; }
    private :
        Bloc * m_bloc;
};

//////////////
// BATIMENT //
//////////////
class Batiment : public ElementDeBloc
{
public:
    Batiment(Zone, Bloc*);
    ~Batiment() {}
    void setHauteur(double h);
    void setIndice(int indice);
	void setCouleur(QColor); //Can change !
    int indice();

    // Element
    void remplir(RemplisseurDeScene & remplisseur);
    QChar caractereMap();

    double hauteurAbsolue() { return m_hauteur + m_hauteurMin; }

private:
    int m_indice;
    QColor m_couleur;
	std::vector<VertexArray::Handle> m_handlesDessins;
};

////////////
// Jardin //
////////////

class Jardin : public ElementDeBloc
{
public:
    Jardin(Zone, Bloc*);
    ~Jardin() {}
    QChar caractereMap();
    void remplir(RemplisseurDeScene & remplisseur);
};

///////////
// Route //
///////////

class Carrefour;
class Route;

class RouteOrientation
{
    public :
        RouteOrientation(QPoint direction);

        QPoint orientation() const { return m_orientation; }
        bool verticale() const { return m_verticale; }
        bool horizontale() const { return !m_verticale; }
        int paral() const { return m_verticale; }
        int perp() const { return !m_verticale; }
    protected :
        const QPoint m_orientation;
        const bool m_verticale; //Utilisé en tant que int, m_verticale renvoie la direction paralele (0 pour x, 1 pour y)
};

class BordCarrefour;

class RouteSimple : public Element, public RouteOrientation
{
    public :
        RouteSimple(Zone, Route * principale);
        ~RouteSimple();
        //Informations
        int largeur() { return m_largeur; }
        int longueur() { return m_longueur; }
        double angleMontee();
        QPoint directionVers(StructZone);
        int numeroDirectionDe(StructZone);

        void setTriangles(int num);

        void mettreAJourHauteurs();
        double hauteurEffective(double x, double y);
        virtual double hauteurEffectiveVoiture(double x, double y);

        //Dessin
        void remplirTriangles(RemplisseurDeScene & remplisseur, QPoint direction);
        void remplirBandeDeSeparation(RemplisseurDeScene &);
        void remplirBandeDeSeparationPointille(RemplisseurDeScene & sommets);

        void remplirBase(RemplisseurDeScene & remplisseur);
        void remplirRoutePrioritaire(RemplisseurDeScene & remplisseur, QPoint direction);

        //Autres infos
        Carrefour * getCarrefour(int i); //0 = debut, 1 = fin
        Route * principale() { return m_principale; }
        Carrefour * debouchee(int i) { return m_debouchees[i]; }

        void attribuerIndiceGPS(int & indice);

        //Element
        void remplir(RemplisseurDeScene & remplisseur);
        QChar caractereMap();
    protected :
        Route * m_principale;
        const int m_largeur;
        const int m_longueur;
        Carrefour * m_debouchees[2];
		BordCarrefour * m_bords[2][2]; //Pour une verticale : [Bas-Haut] [Gauche-Droite]
        bool m_triangles[2];
};

class Route : public Element, public RouteOrientation
{
    public :
        Route(Zone zone, QPoint orientation);
        ~Route();
        //Infos
        int largeur() { return m_largeur; }
        QPoint directionVers(StructZone);
        int numeroDirectionDe(StructZone);
        RouteSimple * getRouteExtreme(int numero);
        //Fonction de création
        void couper(StructZone carrefour);
        void creerCarrefours3AuxBouts();

        void creerSousRoutes(); //Va creer des liaisons sur la carte vers les petites routes simples
        //Accà¨s
        RouteSimple * routeSimple(size_t i) { return m_sousRoutes[i]; }
        size_t nombreSousRoutes() { return m_sousRoutes.size(); }

        Carrefour * chercherCarrefour(int direction, StructZone zone); //0 = debut

        void attribuerIndiceGPS(int & indice);

        //Element
        void remplir(RemplisseurDeScene & remplisseur);
        QChar caractereMap();

        void mettreAJourHauteurs();
        // double hauteurEffective(double x, double y); //Pas encore implémenté mais ne devrait pas etre utilisé
    private :
        std::vector<Zone> m_sousZones;
        std::vector<RouteSimple*> m_sousRoutes;
        const int m_largeur;
        const int m_longueur;
};

////////////////
// Carrefours //
////////////////

class Carrefour;

class Avertissement
{
	public :
		virtual void dessiner() = 0;
        virtual ~Avertissement();
	public :
        static void REMPLIR_MODELES();
    protected :
        Avertissement();
    protected :
        static VertexArray DESSIN_FLECHE;
};

class BordCarrefour : public NoeudDijkstra
{
	public :
		class Priorite : public Avertissement
		{
			public :
				Priorite(BordCarrefour* source, double temps, bool droite);
				virtual void dessiner();

			private :
				BordCarrefour* m_source;
				double m_temps;
			private :
				int m_sens;
		};
    public :
		BordCarrefour(RouteSimple *, Carrefour *, QVector2D position2D);
        ~BordCarrefour();
        QVector3D position3D() const;
        void mettrePrioriteSur(BordCarrefour*);

        void resetObservations();
        void observerVoitures();
        void dessiner();
        Carrefour* getCarrefour() { return m_carrefour; }
        RouteSimple* getRoute() { return m_route; }
		std::vector<Avertissement*> const& getAvertissements() { return m_avertissements; }
		void ajouterAvertissement(Avertissement*);
    private :
        RouteSimple * m_route;
        Carrefour * m_carrefour;
		QVector2D m_position2D;

		std::vector<Avertissement*> m_avertissements;
        std::vector<BordCarrefour*> m_priorites;
};

class EntreeCarrefour : public BordCarrefour
{
    public :
		EntreeCarrefour(RouteSimple *, Carrefour *, QVector2D position);
};

class SortieCarrefour : public BordCarrefour
{
    public :
		SortieCarrefour(RouteSimple *, Carrefour *, QVector2D position);
};

class Carrefour : public Element, public NoeudDijkstra
{
	public :
		class AvertissementCarrefour : public Avertissement
		{
			public :
				AvertissementCarrefour() {}
				void dessiner();
		};

    public :
        QVector3D position3D() const;

        void eleverA(double hauteur);
        double altitude();
		void propagerHauteur(std::function<double(double)>, std::set<Carrefour*> &aEviter);
        void ajouterEntree(BordCarrefour *);
        void montrerLiaisons(VertexArray &, float r = 0, float g = 1, float b = 0, double dz = 0);

        NoeudDijkstra * noeudLePlusProche(QVector3D);
        virtual void ajouterLesAcces() = 0;
        virtual double hauteurEffectiveVoiture(double x, double y);

        void dessiner();
        void observerVoitures();
	private :
		void observerVoituresDansCarrefour();
    protected :
        Carrefour(Zone zone) : Element(zone) {}
		double m_hauteurCarrefour;
		std::vector<BordCarrefour*> m_bords;

};

class Carrefour4 : public Carrefour
{
    public:
        Carrefour4(Zone, Route*, Route*);
        ~Carrefour4() {}
        // Element
        void remplir(RemplisseurDeScene & remplisseur);
        QChar caractereMap();
        void ajouterLesAcces();
    private :
        void dessinerLignes();
        void remplirLignes(RemplisseurDeScene & remplisseur);
    private :
        Route * m_routesPrincipales[2]; //[Dim]
        RouteSimple * m_routesSimples[2][2]; //[Dim][Debut]
};

class Carrefour3Multiple : public Carrefour
{
    public :
        Carrefour3Multiple(Zone zone, Route *principale, Route *secondaire);
        ~Carrefour3Multiple();

        Route * routePrincipale() { return m_principale; }
        void ajouterAutre(Carrefour3Multiple *);

        void remplir(RemplisseurDeScene & remplisseur);
        QChar caractereMap();
        void ajouterLesAcces();
    private :
        std::vector<Route *> m_secondaires;
        Route * m_principale;
        RouteSimple * m_principalesSimples[2]; //[debut]
};

#endif // ELEMENTS_H
