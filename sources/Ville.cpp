#include "Ville.h"

#include <set>
#include <vector>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <cstdlib>

#include <QVector3D>
#include <QVector2D>
#include <QGLWidget>
#include <QDebug>
#include <stdexcept>
#include <cmath>


#include "fonctionsGl.h"
#include "fonctionsMathematiques.h"

#include "Bloc.h"
#include "Element.h"
#include "ImportateurObj.h"
#include "BoulePeinture.h"

#include "GPS.h"

#include "DessinateurCarte.h"

using namespace std;

namespace {
bool comparaisonPoint(QPoint a, QPoint b) {
    return a.x() < b.x() ? true : b.x() < a.x() ? false : a.y() < b.y();
}
}

Ville::Ville(Parametres const& params, Chargement * chargement)
	: SousVille(NULL, StructZone(Inter(0, params.citySizeX), Inter(0, params.citySizeY)), this)
	, m_params(params)
    , m_ecranChargement(chargement)
    , m_hauteursBase(1 + params.citySizeX, vector<double>(1 + params.citySizeY, 0))
    , m_voituresPositionnees(comparaisonPoint)
    , m_dessinerArretes(true)
    //, m_displayList(0)
{
    m_textureSet = 0;
    m_threadGeneration = new ThreadGenerationVille(this);
    connect(m_threadGeneration, SIGNAL(finished()), this, SLOT(finDeThread()));
    m_threadGeneration->start();
}

#include "objectifs/longuest_race.h"
#include "objectifs/stay_high.h"

void Ville::finDeThread()
{
    delete m_threadGeneration;
    m_threadGeneration = nullptr;
    m_ecranChargement = nullptr;
    emit generated();

    m_objectifs = {
        std::shared_ptr<Objectif>(new objectifs::longuest_race()),
        std::shared_ptr<Objectif>(new objectifs::stay_high()),
    };
    for(auto objectif : m_objectifs)
        objectif->setVille(this);
}
void ThreadGenerationVille::run()
{
    srand(time(nullptr));
    m_ville->generer();
}

void Ville::setOptionsDeJeu(const OptionsDeJeu * cible)
{
    m_optionsDeJeu = cible;
}

bool Ville::generee()
{
    return m_threadGeneration == NULL;
}
void Ville::generer()
{
    //Chargmement de quelques .obj
    if(m_ecranChargement)
        m_ecranChargement->afficherTexte("Chargement du Perso...");

    m_perso = new Perso(this, m_ecranChargement);
    m_objetsControlables.push_back(m_perso);

    if(m_ecranChargement)
        m_ecranChargement->afficherTexte(QString::fromUtf8("Chargement de l'hélico..."));

    m_helico = new Helico(this, m_ecranChargement);
    m_objetsControlables.push_back(m_helico);
    m_vehicules.push_back(m_helico);
    m_afficheurGPS[0].setCouleur( QColor(0xFF, 0x99, 0x00) );

    if(m_ecranChargement)
        m_ecranChargement->afficherTexte(QString::fromUtf8("Chargement de la Mustang..."));

    m_mustang = new Mustang(this, m_ecranChargement);
    m_objetsControlables.push_back(m_mustang);
    m_vehicules.push_back(m_mustang);
    m_afficheurGPS[1].setCouleur( QColor(0xFF, 0xFF, 0x00) );
    if(m_ecranChargement)
        m_ecranChargement->afficherTexte(QString::fromUtf8("Génération de la ville..."));

    //Camera
    m_camera = new CameraFreeFly();

    //Création de la matrice C d'Element
    m_elements = new Element**[m_params.citySizeX];
    for(int i = 0 ; i < m_params.citySizeX ; i++) {
        m_elements[i] = new Element*[m_params.citySizeY];
        for(int j = 0 ; j < m_params.citySizeY ; j++)
			m_elements[i][j] = NULL;
	}

	//Découpe de la ville
	Zone zoneInitiale(this);
	zoneInitiale.decouperBlocs(m_params.tailleRouteMaximale);

    if(m_ecranChargement)
    {
        m_ecranChargement->afficherTexte(QString::fromUtf8("Fin de la découpe :"));
        m_ecranChargement->afficherTexte(QString::fromUtf8("<strong>%1</strong> carrefour(s) créé(s)").arg(m_carrefours.size()));
        m_ecranChargement->afficherTexte(QString::fromUtf8("<strong>%1</strong> de type '4'").arg(m_carrefours4.size()));
        m_ecranChargement->afficherTexte(QString::fromUtf8("<strong>%1</strong> de type '3 Multiple'").arg(m_carrefours3Multiples.size()));

        m_ecranChargement->afficherTexte(QString::fromUtf8("Setting up..."));
    }

    //Attribution des couleurs des batiments
    {
        if(m_ecranChargement)
            m_ecranChargement->afficherTexte(QString::fromUtf8("Début de la coloration des batiments par backtracking"));

        int nombreBatiments = 0, nombreAttributions = 0;
        for(ListeChaineeStd<Bloc*>::Iterateur it = m_blocs.premier(); it.valide(); ++it)
        {
            nombreBatiments += (*it)->nombreBatiments();
            nombreAttributions += Bloc::AttributionCouleurs(*it).nombreAppels();
        }

        if(m_ecranChargement)
            m_ecranChargement->afficherTexte(QString::fromUtf8("<strong>%1</strong> batiments colorés pour <strong>%2</strong> assignements de couleur (<strong>%3%</strong> d'efficacité')")
                .arg(nombreBatiments).arg(nombreAttributions).arg(100*(double)nombreBatiments/(double)nombreAttributions));
    }

	//Remplacement sur la carte des routes complètes par des routes simples
	for(Route* route : m_routes)
		route->creerSousRoutes();

    //Élévation des carrefours
	bool ancienAlgoElevationCarrefour = false;
	if(ancienAlgoElevationCarrefour)
	{
		for(Carrefour4* carr : m_carrefours4)
			carr->eleverA( Math::randUniform() * (0.75 * m_params.moyenneH) );
		for(Carrefour3Multiple* carr : m_carrefours3Multiples)
			carr->eleverA( Math::randUniform() * (0.50 * m_params.moyenneH) );
	} else
	{
		auto f = [this](double x) {
			return x + (0.5 - Math::randUniform()) * (0.4 * m_params.moyenneH);
		};
		//ATTENTION ! Essayer avec f(x) = x+1..

		for(Carrefour4* carr : m_carrefours4)
		{
			carr->eleverA( carr->sousVille()->hauteurCaracteristique() + (0.5-Math::randUniform()) * (0.30 * m_params.moyenneH) );
			std::set<Carrefour*> aEviter;
			carr->propagerHauteur(f, aEviter);
		}
	}

    //Mise à jour des hauteurs
	//Premièrement les routes
	//Doit être fait quand les sous routes sont créées
	//Mettra aussi à jour les noeuds Dijkstra
	for(Route* route : m_routes)
		route->mettreAJourHauteurs();
    //Deuxièmement les blocs. Ceux-ci lisent les résultats des routes donc ne peuvent être effectués avant.
	for(Bloc* bloc : m_blocs)
		bloc->mettreAJourHauteurs();

    if(m_ecranChargement)
        m_ecranChargement->afficherTexte(QString::fromUtf8("Fin des élévations de carrefour"));

    //Attribution des indices GPS
    int nombreDeNoeudsDijkstra = 0;
	for(Carrefour* carr : m_carrefours)
    {
		carr->ajouterLesAcces();
		carr->setIndice(nombreDeNoeudsDijkstra++);
		carr->montrerLiaisons(m_sommetsTerrain);
    }
	for(Route* route : m_routes)
		route->attribuerIndiceGPS(nombreDeNoeudsDijkstra);
    for(size_t i = 0 ; i < m_vehicules.size(); i++)
        m_vehicules[i]->setIndice(nombreDeNoeudsDijkstra++);
    m_perso->setIndice(nombreDeNoeudsDijkstra++);

    //Initialisation des GPS indicateurs de Vehicule
    for(int i = 0 ; i < 2 ; i++)
    {
        m_afficheurGPS[i].setDonnees( new GPS(nombreDeNoeudsDijkstra) );
        m_afficheurGPS[i].addDZ(i * 0.02);//(i * 0.1);
    }

    //Creation du tableau de carrefour pour un accès aléatoire
    m_carrefoursTab.resize(m_carrefours.size());
    std::copy(m_carrefours.begin(), m_carrefours.end(), m_carrefoursTab.begin());

    //Positionnement de la voiture
    {
        ListeChaineeStd<Route*>::Iterateur it = m_routes.premier();
        for(size_t n = rand() % m_routes.size() ; n > 0 ; n--)
            ++it;
        Route * laRoute = *it;
        RouteSimple * premiere = laRoute->getRouteExtreme(0);

        QPoint orientation = premiere->orientation();
        m_mustang->angle = atan2(orientation.y(), orientation.x());

        int directionDebouchee = rand() % 2;
        Carrefour * carrefourVoiture = premiere->debouchee(directionDebouchee) ? premiere->debouchee(directionDebouchee) : premiere->debouchee(1 - directionDebouchee);

        double x = carrefourVoiture->zone().x.moyenne();
        double y = carrefourVoiture->zone().y.moyenne();
        m_mustang->position = QVector3D(x, y, hauteurEffective(x,y));

        m_afficheurGPS[1].setCible(carrefourVoiture);
    }

    //Positionnement du perso
    {
        ListeChaineeStd<Bloc*>::Iterateur itB = m_blocs.premier();
        for(size_t n = rand() % m_blocs.size() ; n > 0 ; n--)
            ++itB;
        Bloc * leBloc = *itB;
        m_perso->position.setX(leBloc->zone().x.debut() + Math::randUniform() * leBloc->zone().x.longueur());
        m_perso->position.setY(leBloc->zone().y.debut() + Math::randUniform() * leBloc->zone().y.longueur());
        m_perso->position.setZ( hauteurEffective(m_perso->position.x(), m_perso->position.y()) );
        m_perso->angle = 0;
    }

    //Positionnement de l'helico
    {
        m_helico->position = {1,1,20};
        m_helico->angle = Math::enRadians(rand() % 360);

        for(int n = 0; n < 10 ; n++)
        {
            ListeChaineeStd<Bloc*>::Iterateur itB = m_blocs.premier();
            for(size_t n = rand() % m_blocs.size() ; n > 0 ; n--)
                ++itB;

            if(Batiment * plateforme = (*itB)->chercherPlateformeHelicoHasard())
            {
                m_helico->position = QVector3D(plateforme->zone().x.moyenne(), plateforme->zone().y.moyenne(), 0);
                break;
            }
        }
        m_helico->position.setZ( hauteurEffective(m_helico->position.x(), m_helico->position.y()) + m_params.moyenneH * 2);
        m_afficheurGPS[0].setCible( chercherNoeudLePlusProche(m_helico->position) );
    }

    //Voitures PNJ
    {
        for(int i = 0 ; i < params().nombreDeVoituresPNJ; i++)
        {
            VoiturePNJ * voiturePNJ = new VoiturePNJ(this);
            GPS * gps = new GPS(nombreDeNoeudsDijkstra);

            gps->calculer(cibleGPSAleatoire(), cibleGPSAleatoire());

            voiturePNJ->setGPS(gps);
            voiturePNJ->position = GPS::Iterateur(gps)->position3D();
            m_voituresPNJ.push_back(voiturePNJ);
        }
    }

    m_objetsScene.insert(m_objetsScene.end(), m_objetsControlables.begin(), m_objetsControlables.end());
    m_objetsScene.insert(m_objetsScene.end(), m_voituresPNJ.begin(), m_voituresPNJ.end());

    m_voitures.insert(m_voitures.end(), m_voituresPNJ.begin(), m_voituresPNJ.end());
	m_voitures.insert(m_voitures.end(), & m_mustang, & m_mustang + 1);

    //Objet controlé
    changerObjetControle(m_perso);

    connect(this, SIGNAL(changementControle(ObjetControlable*)), this, SLOT(changerObjetControle(ObjetControlable*)));
}

Ville::~Ville()
{
    for(int i = 0 ; i < m_params.citySizeX ; i++)
        delete[] m_elements[i];
    delete[] m_elements;

    m_blocs.deleteAll();
    m_routes.deleteAll();
    m_carrefours.deleteAll();

    delete m_camera;
	for(ObjetControlable* o : m_objetsControlables)
		delete o;

	for(VoiturePNJ* v : m_voituresPNJ)
		delete v;

	for(BoulePeinture* boule : m_boulesDePeintures)
		delete boule;
}

Ville::AfficheurGPS::AfficheurGPS()
{
    m_donnees = NULL;
    m_cheminActuel = NULL;
    m_depart = NULL;
    m_cible = NULL;
    m_dz = 0.1;//0.7;
    m_largeur = 3;
}

Ville::AfficheurGPS::~AfficheurGPS()
{
    delete m_donnees;
}

void Ville::changementDeZone(Voiture * voiture, QPoint ancienne, QPoint nouvelle)
{
    std::set<Voiture*> & ancien = m_voituresPositionnees[ancienne];
    std::set<Voiture*> & nouveau = m_voituresPositionnees[nouvelle];
    ancien.erase(voiture);
    nouveau.insert(voiture);
    if(ancien.empty())
        m_voituresPositionnees.erase(ancienne);
}

QPoint Ville::calculerZone(ObjetScene * objet) const
{
    return QPoint(floor(objet->position.x()), floor(objet->position.y()));
}

std::set<Voiture *> Ville::voituresA(QPoint point)
{
    MapVoitures::iterator it = m_voituresPositionnees.find(point);
    if(it == m_voituresPositionnees.end())
        return std::set<Voiture*>();
    else
        return it->second;
}

void Ville::AfficheurGPS::recalculer()
{
    if(m_donnees && m_depart)
    {
        m_donnees->calculer(m_depart, m_cible);

        delete m_cheminActuel;
        m_cheminActuel = new VertexArray();
        m_cheminActuel->setModeAffichage(GL_LINE_STRIP);
        for(GPS::Iterateur n(m_donnees); n.valide() ; ++n)
        {
            QVector3D position = n->position3D();
            m_cheminActuel->nouvellePosition(position.x(), position.y(), position.z()+m_dz);
            m_cheminActuel->nouvelleCouleur(m_couleur);
            m_cheminActuel->nouvelleNormale(0,0,1);
        }
    }
}

void Ville::AfficheurGPS::addDZ(double d)
{
    m_dz += d;
}

void Ville::AfficheurGPS::setDonnees(GPS * cib)
{
    delete m_donnees;
    m_donnees = cib;
    recalculer();
}
GPS * Ville::AfficheurGPS::getDonnees()
{
    return m_donnees;
}

double Ville::AfficheurGPS::distanceRestante()
{
    if(m_depart && m_cible)
        return m_donnees->distanceDuDepart(m_cible);
    else
        return 0;
}

void Ville::AfficheurGPS::setDepart(NoeudDijkstra * noeud)
{
    if(m_depart != noeud)
    {
        m_depart = noeud;
        recalculer();
    }
}

void Ville::AfficheurGPS::setCible(NoeudDijkstra * cib)
{
    if(m_cible != cib)
    {
        m_cible = cib;
        recalculer();
    }
}

NoeudDijkstra * Ville::AfficheurGPS::getCible()
{
    return m_cible;
}

void Ville::AfficheurGPS::setCouleur(QColor c)
{
    m_couleur = c;
    recalculer();
}

void Ville::AfficheurGPS::setLargeur(double d)
{
    m_largeur = d;
}

void Ville::AfficheurGPS::dessiner()
{
    if(m_cheminActuel)
    {
        glLineWidth(m_largeur);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_cheminActuel->dessiner();
    }
}

Carrefour * Ville::cibleGPSAleatoire()
{
    return m_carrefoursTab[ rand() % m_carrefoursTab.size() ];
}

NoeudDijkstra * Ville::chercherNoeudLePlusProche(QVector3D pos)
{
    Element * el = at(floor(pos.x()), floor(pos.y()));
    Carrefour * carr = NULL;
    if(Carrefour* c = dynamic_cast<Carrefour*>(el))
        carr = c;
    else if(RouteSimple * r = dynamic_cast<RouteSimple*>(el))
    {
        Carrefour * c[2] = {r->debouchee(0), r->debouchee(1)};
        if(c[0] == NULL)
            carr = c[1];
        else if(c[1] == NULL)
            carr = c[0];
        else
            carr = (c[0]->position3D() - pos).lengthSquared() < (c[1]->position3D() - pos).lengthSquared() ? c[0] : c[1];
    }
    else if(ElementDeBloc * elemBloc = dynamic_cast<ElementDeBloc*>(el))
    {
        StructZone zoneBloc = elemBloc->getBloc()->zone();
        StructZone elargie(zoneBloc.x.agrandi(1), zoneBloc.y.agrandi(1));

        double mini = 2e9;
        for(int a = 0 ; a < 2 ; a++)
        for(int b = 0 ; b < 2 ; b++)
        {
            if(Carrefour * c = dynamic_cast<Carrefour*>( at(elargie.coin(a,b)) ) )
            {
                double d = (pos - c->position3D()).length();
                if(d < mini)
                    mini = d, carr = c;
            }
        }
    }
    if(carr)
        return carr->noeudLePlusProche(pos);
    else
        return NULL;
}

void Ville::changerObjetControle(ObjetControlable * cible)
{
    if(m_objetControle != cible)
    {
        m_objetControle = cible;
        m_objetControle->placerCamera(m_camera);
        emit changementControle(m_objetControle);
    }
}

void Ville::rentrerDansVehicule(Perso * perso)
{
    double distances[2];
    for(int i = 0 ; i < 2 ; i++)
        distances[i] = (m_perso->position - m_vehicules[i]->position).length();

    int m = distances[0] < distances[1] ? 0 : 1;
    if(distances[m] < 1)
    {
        perso->rentrerDansVehicule(m_vehicules[m]);
        changerObjetControle(m_vehicules[m]);
    }
}

void Ville::afficherDansFichierTexte(QString fileName)
{
    QFile fichier(fileName);
    fichier.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
    QTextStream stream(&fichier);
    stream.setCodec("UTF-8");
    afficher(stream);
}

void Ville::afficherArretes(bool afficher)
{
    m_dessinerArretes = afficher;
    //creationDisplayList();
}

CameraFreeFly * Ville::getCamera()
{
    return m_camera;
}
ObjetControlable * Ville::getObjetControle()
{
    return m_objetControle;
}

double Ville::getHauteurBase(int x,int y)
{
	return m_hauteursBase.at(x).at(y);
    throw std::out_of_range("Ville::getHauteurPoint arguments hors de la matrice des hauteurs");
}

void Ville::setHauteurBase(int x, int y, double h)
{
    m_hauteursBase[x][y] = h;
}

void Ville::enterFrame()
{
    //qDebug() << "--- Nouveau frame ---";
    qDebug() << QString("xyz = (%1, %2, %3)").arg(m_perso->position3D().x()).arg(m_perso->position3D().y()).arg(m_perso->position3D().z());

    auto m_objetsSceneCopy = m_objetsScene; // because it may delete
    for(ObjetScene* objet : m_objetsSceneCopy)
        objet->enterFrame();

    for(Carrefour* c : m_carrefours)
        c->observerVoitures();

    for(VoiturePNJ* voiture : m_voituresPNJ)
    {
        const double dist = m_optionsDeJeu->performance.distanceVoiture;
        bool proche = (voiture->position - m_objetControle->position).lengthSquared() < dist*dist;
        voiture->setQualiteDessin(proche ? VoiturePNJ::MAX_QUALITY : 1);
    }

    m_objetControle->placerCamera(m_camera);

    //Astuce : Ajouter au noeud dijkstra Perso les noeud accessibles en fonction de sa position
    //Ensuite les afficheurs auront le depart "perso"
    if(NoeudDijkstra * noeud = chercherNoeudLePlusProche(m_objetControle->position))
    {
        for(int i = 0 ; i < 2 ; i++)
        {
            m_afficheurGPS[i].setDepart(noeud);
            if(NoeudDijkstra * cible = chercherNoeudLePlusProche(m_vehicules[i]->position))
                m_afficheurGPS[i].setCible(cible);
        }
    }
}

void Ville::remplirSommetsTerrain(VertexArray & sommets)
{
    const double dec = 0.05;
    for(int i = 0 ; i < m_params.citySizeX ; i++)
    {
        for(int j = 0 ; j < m_params.citySizeY ; j++)
        {
            sommets.nouvellePosition(i, j, m_hauteursBase[i][j] - dec);
            sommets.nouvellePosition(i+1, j, m_hauteursBase[i+1][j] - dec);
            sommets.nouvellePosition(i+1, j+1, m_hauteursBase[i+1][j+1] - dec);
            sommets.nouvellePosition(i, j+1, m_hauteursBase[i][j+1] - dec);

            for(int k = 0 ; k < 4; k++) {
                sommets.nouvelleCouleur(1,0,0);
                sommets.nouvelleNormale(0,0,1);
            }
        }
    }
}

void Ville::drawGL()
{
    static clock_t leTemps = clock();

    clock_t tempsMtn = clock();
    double realFPS = (double) CLOCKS_PER_SEC / (double)(tempsMtn - leTemps);
    leTemps = tempsMtn;

    if(realFPS < Parametres::FPS) {
        qDebug() << "FPS drop ! expected:" << Parametres::FPS << "real:" << (int)realFPS << "percent:" << (int)(100 * (realFPS / Parametres::FPS));
    } else {
        qDebug() << "FPS nice : expected:" << Parametres::FPS << "real:" << (int)realFPS << "percent:" << (int)(100 * (Parametres::FPS / realFPS));
    }

    m_camera->glLook();

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    GLfloat lightPos[4] = {
        GLfloat(m_params.citySizeX / 2.0),
        GLfloat(m_params.citySizeY),
        GLfloat(m_params.moyenneH * 5),
        GLfloat(1)
    };
    //GLfloat lightPos[4] = {2.5,10,10};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    for(ObjetScene* obj : m_objetsScene)
        obj->dessiner();

    for(Carrefour* c : m_carrefours)
        c->dessiner();

    if(m_sommets.length() == 0)
        remplirSommets();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    m_sommets.dessiner();

    if(m_dessinerArretes)
    {
        glLineWidth(4);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        m_sommetsArretes.dessiner();
    }

    bool afficherTerrainDebug = false;
    if(afficherTerrainDebug)
    {
        glDisable(GL_CULL_FACE);
        glLineWidth(1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_sommetsTerrain.dessiner();
    }

    double dist0 = m_afficheurGPS[0].distanceRestante();
    double dist1 = m_afficheurGPS[1].distanceRestante();
    int nMin = dist0 < dist1 ? 0 : 1;

    m_afficheurGPS[nMin].setLargeur(10);
    m_afficheurGPS[1-nMin].setLargeur(3);

    for(int i = 0 ; i < 2 ; i++)
        m_afficheurGPS[i].dessiner();
}

void Ville::drawMiniCarte()
{
    if(m_sommets.length() == 0)
        remplirSommets();

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    { Repere r; r.rotate(m_perso->angle, Repere::Z);
        m_indicateurPerso.dessiner();
    }
    { Repere r; r.translate((m_helico->position - m_perso->position).toVector2D()).rotate(m_helico->angle, Repere::Z);
        m_indicateurHelico.dessiner();
    }
    { Repere r; r.translate((m_mustang->position - m_perso->position).toVector2D()).rotate(m_mustang->angle, Repere::Z);
        m_indicateurMustang.dessiner();
    }
    for(VoiturePNJ* voiture : m_voituresPNJ)
    {
        Repere r; r.translate((voiture->position - m_perso->position).toVector2D()).rotate(voiture->angle, Repere::Z);
        m_indicateurVoiture.dessiner();
    }

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    { Repere r; r.translate(-m_perso->position.toVector2D());
        m_miniCarte.dessiner();
    }

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Ville::remplirSommets()
{
    m_sommets.setModeAffichage(GL_QUADS);
    m_sommetsArretes.setModeAffichage(GL_QUADS);
    m_sommetsTerrain.setModeAffichage(GL_QUADS);
    m_miniCarte.setModeAffichage(GL_QUADS);

    m_sommets.utiliserBuffer();
    m_sommetsTerrain.utiliserBuffer();
    m_miniCarte.utiliserBuffer();

    Dessinateur dessinateur(m_sommets);
    DessinateurCarte dessinateurCarte(m_miniCarte);
    RemplisseurDeScene remplisseur(m_sommets, dessinateur, *m_textureSet);
    RemplisseurDeScene remplisseurCarte(m_miniCarte, dessinateurCarte, *m_textureSet);

    for(ListeChaineeStd<Bloc*>::Iterateur bloc = m_blocs.premier() ; bloc.valide() ; ++bloc)
    {
        (*bloc)->remplir(remplisseur);
        (*bloc)->remplir(remplisseurCarte);
    }

    for(ListeChaineeStd<Route*>::Iterateur route = m_routes.premier() ; route.valide() ; ++route)
    {
        (*route)->remplir(remplisseur);
        (*route)->remplir(remplisseurCarte);
    }
    for(ListeChaineeStd<Carrefour4*>::Iterateur carr = m_carrefours4.premier() ; carr.valide() ; ++carr)
    {
        (*carr)->remplir(remplisseur);
        (*carr)->remplir(remplisseurCarte);
    }
    for(ListeChaineeStd<Carrefour3Multiple*>::Iterateur carr = m_carrefours3Multiples.premier() ; carr.valide() ; ++carr)
    {
        (*carr)->remplir(remplisseur);
        (*carr)->remplir(remplisseurCarte);
     }

    m_sommets.copierSansCouleurs(m_sommetsArretes);
	remplirSommetsTerrain(m_sommetsTerrain);

    for(size_t i = 1 ; i <= m_miniCarte.length(); i++)
        m_miniCarte.derniereCouleur(i)[3] = 0.90;

    VertexArray * indicateurs[4] = {&m_indicateurPerso, &m_indicateurMustang, &m_indicateurHelico, &m_indicateurVoiture};
    QColor couleurs[4] = {0x00FFFF, 0xFFFF00, 0xFF9900, 0xCCCCCC};
    for(int i = 0 ; i < 4; i++)
    {
        VertexArray & indicateur = *indicateurs[i];
        indicateur.setModeAffichage(GL_TRIANGLES);
        indicateur.nouvellePosition(-1/3.0, 0.5);
        indicateur.nouvellePosition(2/3.0, 0);
        indicateur.nouvellePosition(0.0, 0.0);

        indicateur.nouvellePosition(-1/3.0, -0.5);
        indicateur.nouvellePosition(2/3.0, 0);
        indicateur.nouvellePosition(0.0, 0.0);
        for(int n = 0 ; n < 6; n++)
        {
            indicateur.nouvelleCouleur(couleurs[i]);
            indicateur.nouvelleNormale(0,0,1);
        }
    }
}

Element * Ville::get(int x, int y)
{
    return m_elements[x][y];
}

Element * Ville::get(QPoint p)
{
    return get(p.x(), p.y());
}

Element * Ville::get(int p[])
{
    return get(p[0], p[1]);
}

void Ville::lierElements(StructZone zone, Element * cible)
{
    for(int i = zone.x.debut() ; i < zone.x.fin() ; i++)
        for(int j = zone.y.debut() ; j < zone.y.fin() ; j++)
            m_elements[i][j] = cible;
}

Element * Ville::at(int x, int y)
{
    if(x >= 0 && x < m_params.citySizeX && y >= 0 && y < m_params.citySizeY)
        return m_elements[x][y];
    return NULL;
}

Element * Ville::at(QPoint p)
{
    return at(p.x(), p.y());
}

Element * Ville::at(int p[])
{
    return at(p[0], p[1]);
}

Element* Ville::at(QPointF p)
{
	return at((double)p.x(), (double)p.y());
}

Element * Ville::at(double x, double y)
{
	int xD = std::floor(x);
	int yD = std::floor(y);

	if(xD < 0 || xD >= m_params.citySizeX || yD < 0 || yD >= m_params.citySizeY)
		return 0;

	return m_elements[xD][yD];
}

double Ville::hauteurEffectiveVoiture(double x, double y)
{
	Element* elem = at(x,y);
	return elem == NULL ? 0 : elem->hauteurEffectiveVoiture(x, y);

    //return (m_hauteursBase[xD][yD] + m_hauteursBase[xD][yF] + m_hauteursBase[xF][yD] + m_hauteursBase[xF][yF]) / 4;
}

double Ville::hauteurEffective(double x, double y)
{
    int xD = std::floor(x);
    //int xF = std::ceil(x);
    int yD = std::floor(y);
    //int yF = std::ceil(y);

    if(xD < 0 || xD >= m_params.citySizeX || yD < 0 || yD >= m_params.citySizeY)
        return 0;

    return m_elements[xD][yD]->hauteurEffective(x, y);

    //return (m_hauteursBase[xD][yD] + m_hauteursBase[xD][yF] + m_hauteursBase[xF][yD] + m_hauteursBase[xF][yF]) / 4;
}

void Ville::ajouterRoute(Route * route)
{
    m_routes.push_back(route);
}
void Ville::ajouterCarrefour4(Carrefour4 * cible)
{
    m_carrefours4.push_back(cible);
    m_carrefours.push_back(cible);
}

void Ville::creerCarrefour3(Zone zone, Route *principale, Route *secondaire)
{
    StructZone laZone = zone;

    typedef std::set<Carrefour3Multiple *> LeSet;
    LeSet existants;
    for(int x = laZone.x.debut() - 1 ; x < laZone.x.fin() + 1; x++)
        for(int y = laZone.y.debut() - 1 ; y < laZone.y.fin() + 1; y++)
            if(Carrefour3Multiple * existant = dynamic_cast<Carrefour3Multiple*>( at(x,y) ))
                existants.insert(existant);

    Carrefour3Multiple * nouveau = new Carrefour3Multiple(zone, principale, secondaire);
    m_carrefours3Multiples.push_back(nouveau);
    m_carrefours.push_back(nouveau);

	for(Carrefour3Multiple* existant : existants)
    {
		m_carrefours3Multiples.erase(std::find(m_carrefours3Multiples.begin(), m_carrefours3Multiples.end(), existant));
		m_carrefours.erase(std::find(m_carrefours.begin(), m_carrefours.end(), existant));
		nouveau->ajouterAutre(existant);
		delete existant;
	}
}

Ville::HandleBoulePeinture Ville::ajouterBoulePeinture(BoulePeinture * boule)
{
	HandleBoulePeinture h;
	h.itBoulePeinture = m_boulesDePeintures.insert(m_boulesDePeintures.end(), boule);
	h.itObjetScene = m_objetsScene.insert(m_objetsScene.end(), boule);
	return h;
}

void Ville::enleverBouleDePeinture(BoulePeinture* boule, HandleBoulePeinture h)
{
	m_boulesDePeintures.erase(h.itBoulePeinture);
	m_objetsScene.erase(h.itObjetScene);
	delete boule;
}

void Ville::ajouterBloc(Bloc * cible)
{
    m_blocs.push_back(cible);
}

double Ville::hauteurBatimentHasard()
{
    return std::max(0.5, Math::randNormal(m_params.moyenneH, m_params.ecartTypeH));
}

void Ville::afficher(QTextStream & out)
{
    for(int y = 0 ; y < m_params.citySizeY ; y++)
    {
        for(int x = 0 ; x < m_params.citySizeX ; x++)
            out << m_elements[x][y]->caractereMap();
        out << endl;
    }
}

// Objectifs
Batiment* Ville::findHighestBuilding(){
    Batiment* max_b = nullptr;
    for(ListeChaineeStd<Bloc*>::Iterateur it = m_blocs.premier(); it.valide(); ++it) {
        Batiment* b = (*it)->findHighestBuilding();
        if(!max_b || (b && b->hauteurAbsolue() > max_b->hauteurAbsolue()))
            max_b = b;
    }
    return max_b;
}
