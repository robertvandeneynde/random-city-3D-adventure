#ifndef VILLE_H
#define VILLE_H

#include <cstdlib>
#include <cmath>
#include <ctime>

#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include <QGLWidget>
#include <QDebug>
#include <QVector3D>
#include <QThread>
#include <QPoint>

#include "ImportateurObj.h"

#include "Helico.h"
#include "Mustang.h"
#include "Perso.h"

#include "VoiturePNJ.h"

#include "ListeChainee.h"
#include "RemplisseurDeScene.h"
#include "VertexArray.h"
#include "StructZone.h"
#include "Parametres.h"
#include "Chargement.h"

#include "VilleInterface.h"
#include "SousVille.h"

class Element;
class Bloc;
class Route;
class Carrefour;
class EntreeZone;
class Carrefour3Multiple;
class Carrefour4;
class BoulePeinture;
class Batiment;

class NoeudDijkstra;
class GPS;

class ThreadGenerationVille;
class Zone;
class Ville : public VilleInterface, public SousVille
{
        Q_OBJECT
	public:
		class HandleBoulePeinture
		{
			private:
				std::list<ObjetScene*>::iterator itObjetScene;
				std::list<BoulePeinture*>::iterator itBoulePeinture;
				friend class Ville;
		};
    public :
        Ville(Parametres const&, Chargement * = NULL); //Construction Sous Thread, la ville sera generee quand generee() renverra true
        ~Ville();
        bool generee();
        void setOptionsDeJeu(const OptionsDeJeu *);
        //VilleInterface
        void afficherDansFichierTexte(QString fileName = "map.txt");
        void setTextureSet(TextureSet * cible) { m_textureSet = cible; }
        void drawGL();
        void drawMiniCarte();
        void enterFrame();
        void afficherArretes(bool);
        CameraFreeFly * getCamera();
        ObjetControlable * getObjetControle();

        //Manipulation / Lecture
        void lierElements(StructZone, Element *);

        Element * get(int x, int y); //Sans verification
        Element * get(QPoint); //Sans verification
        Element * get(int p[2]); //Sans verification

        Element * at(int x, int y); //Renvoie NULL si out_of_range
        Element * at(QPoint); //Renvoie NULL si out_of_range
        Element * at(int p[2]); //Renvoie NULL si out_of_range

		Element * at(double x, double y);
		Element * at(QPointF);

        Parametres const & params() { return m_params; }

        double hauteurBatimentHasard();

        Carrefour * cibleGPSAleatoire();

		double hauteurEffective(double x, double y); //Position non entieres !
        double hauteurEffectiveVoiture(double x, double y); //Position non entieres !

		double hauteurEffective(QPointF p) { return hauteurEffective(p.x(), p.y()); }
		double hauteurEffectiveVoiture(QPointF p) { return hauteurEffectiveVoiture(p.x(), p.y()); }

		double getHauteurBase(int x,int y);
        void setHauteurBase(int x,int y, double h);
        //Manipulation des listes
        void creerCarrefour3(Zone, Route * principale, Route * secondaire);
        void ajouterCarrefour4(Carrefour4*);
        void ajouterRoute(Route*);
        void ajouterBloc(Bloc*);
		HandleBoulePeinture ajouterBoulePeinture(BoulePeinture*);
		void enleverBouleDePeinture(BoulePeinture*, HandleBoulePeinture);

        void rentrerDansVehicule(Perso * perso);

        void changementDeZone(Voiture * voiture, QPoint ancienne, QPoint nouvelle);
        QPoint calculerZone(ObjetScene * objet) const;
        std::set<Voiture *> voituresA(QPoint);
    public slots:
        void changerObjetControle(ObjetControlable *);
    private :
        void afficher(QTextStream &);
        void remplirSommetsTerrain(VertexArray & sommets);
        NoeudDijkstra * chercherNoeudLePlusProche(QVector3D);
		void remplirSommets();

    private slots:
        friend class ThreadGenerationVille;
        void generer(); //Sera lancé dans un nouveau thread
        void finDeThread();
    private :
        Parametres const m_params;
        ThreadGenerationVille * m_threadGeneration;
        Chargement * m_ecranChargement;

        Element* ** m_elements;
        std::vector< std::vector < double> > m_hauteursBase; // citySizeX+1 * citySizeY+1
        typedef std::map< QPoint, std::set<Voiture*>, bool(*)(QPoint,QPoint) > MapVoitures;
        MapVoitures m_voituresPositionnees;

        ListeChaineeStd<Bloc*> m_blocs;
        ListeChaineeStd<Route*> m_routes;
        ListeChaineeStd<Carrefour4*> m_carrefours4;
        ListeChaineeStd<Carrefour3Multiple*> m_carrefours3Multiples;
        ListeChaineeStd<Carrefour*> m_carrefours;
        std::vector<Carrefour*> m_carrefoursTab;

        bool m_dessinerArretes;
        //GLuint m_displayList;
        VertexArray m_sommets, m_sommetsArretes;
        VertexArray m_sommetsTerrain;
        VertexArray m_miniCarte;

        Helico           * m_helico;
        Mustang          * m_mustang;
        Perso            * m_perso;
        std::vector<VoiturePNJ *> m_voituresPNJ;
		std::list<BoulePeinture*> m_boulesDePeintures;
        VertexArray m_indicateurPerso, m_indicateurMustang, m_indicateurVoiture, m_indicateurHelico;


        CameraFreeFly    * m_camera;
        ObjetControlable * m_objetControle;

        std::vector<ObjetControlable*> m_objetsControlables;
        std::vector<Vehicule*>         m_vehicules;
        std::vector<Voiture*>          m_voitures;
        std::list<ObjetScene*>         m_objetsScene;

        OptionsDeJeu  const * m_optionsDeJeu;
        TextureSet * m_textureSet;

        class AfficheurGPS
        {
            public :
                AfficheurGPS();
                ~AfficheurGPS();

                void addDZ(double);
                GPS * getDonnees();
                void setDonnees(GPS *);
                void dessiner();
                void setDepart(NoeudDijkstra *);
                void setCible(NoeudDijkstra *);
                NoeudDijkstra * getCible();
                void setCouleur(QColor);
                void setLargeur(double);
                double distanceRestante();
            private :
                void recalculer();
            private :
                NoeudDijkstra * m_cible;
                NoeudDijkstra * m_depart;
                GPS * m_donnees;
                VertexArray * m_cheminActuel;
                QColor m_couleur;
                double m_dz;
                double m_largeur;
        };

        AfficheurGPS m_afficheurGPS[2];
};

class ThreadGenerationVille : public QThread
{
		Q_OBJECT
	public :
		ThreadGenerationVille(Ville * cible)
			: m_ville(cible)
		{}
	protected :
		void run();
	private :
		Ville * m_ville;
};

#endif // VILLE_H
