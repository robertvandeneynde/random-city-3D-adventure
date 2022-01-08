#include "Bloc.h"

#include "Element.h"
#include "Ville.h"

#include <algorithm>
#include <QDir>

Bloc::Bloc(Zone zone)
    : m_zone(zone)
    , m_ville(zone.getVille())
{
    m_hauteurSol = 0;
    m_zone.decouperBatiments(this);

    if(m_ville->params().activerJardins)
        creerJardins();

    int i = 0;
    for(ListeChaineeStd<Batiment*>::Iterateur it = m_batiments.premier() ; it.valide() ; ++it)
    {
        m_elements.push_back(*it);
        (*it)->setIndice(i++);
    }
    for(ListeChaineeStd<Jardin*>::Iterateur it = m_jardins.premier() ; it.valide() ; ++it)
        m_elements.push_back(*it);
}

void Bloc::creerJardins()
{
    ListeChaineeStd<Batiment*>::iterator it = m_batiments.begin();
    while(it != m_batiments.end())
    {
        Batiment * bati = *it;

        if( bati->enBordure() )
            ++it;
        else
        {
            m_jardins.push_back( new Jardin( m_zone.nouvelleZone(bati->zone()), this) );
            m_batiments.erase(it++);
            delete bati;
        }
    }
}

void Bloc::mettreAJourHauteurs()
{
    StructZone z = m_zone;
    m_hauteurSol = 1e9;
    //On cherche le minimum sur la bordure
    int lesY[2] = {z.y.debut(), z.y.fin()};
    int lesX[2] = {z.x.debut(), z.x.fin()};
    for(int i = 0 ; i < 2 ; i++)
    {
        for(int x = z.x.debut() ; x <= z.x.fin() ; x++)
            m_hauteurSol = std::min(m_hauteurSol, m_ville->getHauteurBase(x, lesY[i]));
        for(int y = z.y.debut() + 1 ; y <= z.y.fin() - 1 ; y++)
            m_hauteurSol = std::min(m_hauteurSol, m_ville->getHauteurBase(lesX[i], y));
    }

    //On l'applique aux points du centre
    Inter sousX(z.x.debut() + 1, z.x.fin() - 1);
    Inter sousY(z.y.debut() + 1, z.y.fin() - 1);
    m_zone.nouvelleZone(sousX, sousY).eleverA(m_hauteurSol);
    //On en informe ses elements
    for(ListeChaineeStd<ElementDeBloc*>::Iterateur elem = m_elements.premier() ; elem.valide() ; ++elem)
        (*elem)->mettreAJourHauteurs();
}

void Bloc::nouveauBatiment(Zone zone)
{
    Batiment * bati = new Batiment(zone, this);
	bati->setHauteur( m_ville->hauteurBatimentHasard() );
	bati->setIndice( m_batiments.size() );

    m_batiments.push_back(bati);
}

void Bloc::remplir(RemplisseurDeScene & remplisseur)
{
    for(ListeChaineeStd<ElementDeBloc*>::Iterateur elem = m_elements.premier() ; elem.valide() ; ++elem)
        (*elem)->remplir(remplisseur);
}

Bloc::~Bloc()
{
    m_elements.deleteAll();

    Q_FOREACH(QFile* fichier, m_listeDeFichiersLog) {
        fichier->remove();
        delete fichier;
    }
}

Batiment * Bloc::chercherPlateformeHelicoHasard()
{
    std::vector<Batiment*> ensemble;
    for(ListeChaineeStd<ElementDeBloc*>::Iterateur elem = m_elements.premier() ; elem.valide() ; ++elem)
        if(Batiment * bati = dynamic_cast<Batiment*>(elem.donnee()))
            if(bati->zone().x.longueur() >= 4 && bati->zone().y.longueur() >= 4)
                ensemble.push_back(bati);

    return ensemble.empty() ? NULL : ensemble[rand() % ensemble.size()];
}


/////////////////////////
// AttributionCouleurs //
/////////////////////////

#include <vector>
#include <set>
#include <algorithm>

#include <QColor>

using namespace std;

#define DEBUG_BLOC 0

const int INDEFINI = -1;

Bloc::AttributionCouleurs::AttributionCouleurs(Bloc* bloc)
    : bloc(bloc)
    , ordreEssai(bloc->m_batiments)
    , couleurs(bloc->m_batiments.size(), INDEFINI)
    , connexions(bloc->m_batiments.size())
{
#if DEBUG_BLOC
    fichierLog = new QFile();
    bloc->m_listeDeFichiersLog.append(fichierLog);
    log.setDevice(fichierLog);

    QString nomBloc;
    QTextStream stream(&nomBloc);
    stream << bloc->m_ville->params().nom;
    stream << "_Bloc_";
    stream << "_P_" << bloc->zone().x.debut() << "," << bloc->zone().y.debut() << "_";
    stream << "_T_" << bloc->zone().x.longueur() << "," << bloc->zone().y.longueur() << "_";
    stream << "_N_" << bloc->m_batiments.size() << "_";
    fichierLog->setFileName(nomBloc + ".txt");
    fichierLog->open(QFile::WriteOnly | QFile::Truncate);
    log << nomBloc << endl;
#endif
    nombreAppel = 0;

    creerConnexions();

    ordreEssai.trier( MonFacteurDeTri(this) ); //Pour essayer de limiter les appels
    finDeParcours = attribue(ordreEssai.premier());
#if DEBUG_BLOC
    log << "Nombre appels : " << nombreAppel;
#endif
    if(finDeParcours)
    {
        afficherCouleursZone();
        const vector< vector<QColor> > & couleursReeles = bloc->m_ville->params().couleursBatiments;

        vector< vector<QColor> > ordre(4);
        for(int i = 0 ; i < (int)couleursReeles.size(); i++)
            ordre[i].assign(1, couleursReeles[i][rand() % couleursReeles[i].size()]); //(couleursReeles[i].begin(), couleursReeles[i].end());
        random_shuffle(ordre.begin(), ordre.end());

        for(ListeChaineeStd<Batiment*>::Iterateur bati = bloc->m_batiments.premier() ; bati.valide(); ++bati)
        {
            vector<QColor> & toUse = ordre[ couleur(*bati) ];
            (*bati)->setCouleur( toUse[ rand() % toUse.size() ] );
        }
    }
}

bool Bloc::AttributionCouleurs::MonFacteurDeTri::operator()(Batiment* b1, Batiment* b2)
{
    return self->voisins(b1).size() > self->voisins(b2).size();
}

void Bloc::AttributionCouleurs::afficherCouleursZone()
{
#if DEBUG_BLOC > 1
    for(int y = bloc->zone().y.debut() ; y < bloc->zone().y.fin(); y++)
    {
        for(int x = bloc->zone().x.debut() ; x < bloc->zone().x.fin(); x++)
            log << bloc->m_ville->at(x,y)->caractereMap();
        log << "   ";

        for(int x = bloc->zone().x.debut() ; x < bloc->zone().x.fin(); x++)
        {
            Element * elem = bloc->m_ville->at(x,y);
            Batiment * bati = dynamic_cast<Batiment*>(elem);
            if(bati)
                log << 1 + couleur(bati);
            else
                log << elem->caractereMap();
        }

        log << endl;
    }
#endif
}

bool Bloc::AttributionCouleurs::couleursAttribuees()
{
    return finDeParcours;
}

int & Bloc::AttributionCouleurs::couleur(Batiment * batiment)
{
    return couleurs[ batiment->indice() ];
}

set<Batiment*> & Bloc::AttributionCouleurs::voisins(Batiment * batiment)
{
    return connexions[ batiment->indice() ];
}

void Bloc::AttributionCouleurs::creerConnexions()
{
    for(ListeChaineeStd<Batiment*>::Iterateur it = bloc->m_batiments.premier(); it.valide(); ++it)
    {
        Batiment* batiment = *it;
        StructZone zone = batiment->zone();

        int point[2];
        for(int d = 0 ; d < 2 ; d++)
        {
            for(point[d] = zone[d].debut() ; point[d] < zone[d].fin() ; point[d]++)
            {
                for(int i = 0 ; i < 2 ; i++)
                {
                    point[1-d] = zone[1-d].agrandi(1).getC(i);
                    if(Batiment* voisin = dynamic_cast<Batiment*>( bloc->m_ville->at(point) ))
                        voisins(batiment).insert(voisin);
                }
            }
        }
    }
}

bool Bloc::AttributionCouleurs::attribue(ListeChaineeStd<Batiment*>::Iterateur actuel)
{
    if(! actuel.valide())
        return true;
    Batiment* batiment = *actuel;
    nombreAppel++;

#if DEBUG_BLOC > 1
    QString affichageBatiment = "<" + QString(batiment->caractereMap()) + ">";
    log << affichageBatiment << " ";
    for(ListeChaineeStd<Batiment*>::Iterateur it = ordreEssai.premier() ; it.valide() ; ++it)
        log << (*it == batiment ? "(" : "") << (*it)->caractereMap() << (*it == batiment ? ")" : "");
    log << endl;
    afficherCouleursZone();
#endif

    vector<int> possib(4, true);

    for(set<Batiment*>::iterator voisin = voisins(batiment).begin() ; voisin != voisins(batiment).end() ; ++voisin)
        if(couleur(*voisin) != INDEFINI)
            possib[couleur(*voisin)] = false;

    vector<int> possibV;
    for(unsigned c = 0 ; c < possib.size() ; c++)
        if(possib[c])
            possibV.push_back(c);

    random_shuffle(possibV.begin(), possibV.end());

    ListeChaineeStd<Batiment*>::Iterateur suivant = actuel;
    ++suivant;
    for(unsigned int i = 0 ; i < possibV.size() ; i++)
    {
        couleur(batiment) = possibV[i];

#if DEBUG_BLOC > 1
        log << affichageBatiment << "on essaie : " << "[";
        for(unsigned int j = 0 ; j < possibV.size() ; j++)
            log << (i == j ? "(" : "") << 1 + possibV[j] << (i == j ? ")" : "");
        log << "]" << endl;
        log << endl;
#endif

        if( attribue(suivant) )
            return true;
    }

#if DEBUG_BLOC > 1
    log << affichageBatiment << "Aucune couleur ne marche dans [";
    for(unsigned int i = 0 ; i < possibV.size(); i++)
        log << 1 + possibV[i];
    log << "]. Baaacktrack !" << endl;
#endif
    couleur(batiment) = INDEFINI;
    return false;
}

Batiment* Bloc::findHighestBuilding() {
    bool found = false;
    double h;
    Batiment* b;

    for(Batiment* batiment : m_batiments) {
        if(!found || h < batiment->hauteurAbsolue()) {
            h = batiment->hauteurAbsolue();
            b = batiment;
        } else {

        }
        found = true;
    }

    if(found)
        return b;
    else
        return nullptr; // TODO Exception
}
