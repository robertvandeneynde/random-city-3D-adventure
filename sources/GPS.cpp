#include "GPS.h"
#include <QDebug>
#include <algorithm>

GPS::GPS(size_t nombreDeNode)
    : m_nombreDeNode(nombreDeNode)
{
    m_listeOuverte = NULL;
    m_arrivee = NULL;
    m_indiceChemin = -1;
}
GPS::~GPS()
{
    delete m_listeOuverte;
}

double GPS::calculHeuristique(NoeudDijkstra * noeud)
{
    if(m_arrivee == NULL)
        return 0;
    return (m_arrivee->position3D() - noeud->position3D()).length();
}

GPS::VariablesDynamiques::VariablesDynamiques()
{
    distance = 2e9;
    distancePresumeeVersDestination = 0;
    liste = AUCUNE;
    antecedent = NULL;
}

inline GPS::VariablesDynamiques const & GPS::infos(NoeudDijkstra * noeud)
{
    return vari(noeud);
}

inline GPS::VariablesDynamiques & GPS::vari(NoeudDijkstra * noeud)
{
    return m_variables[ noeud->indice() ];
}

void GPS::calculer(NoeudDijkstra * depart, NoeudDijkstra * arrivee)
// tous les noeuds sont accessibles de partout
{
    m_variables.assign(m_nombreDeNode, VariablesDynamiques());

    m_chemin.clear();
    delete m_listeOuverte;
    m_listeOuverte = new ListeOuverteTas(this);

    m_arrivee = arrivee;
    vari(depart).distance = 0;

    m_listeOuverte->push(depart);
    NoeudDijkstra * enCours = depart;

    size_t n = 0;
    while(! m_listeOuverte->vide() && enCours != arrivee && ++n <= m_nombreDeNode)
        traiter( enCours = m_listeOuverte->pop() );

    for(NoeudDijkstra * n = arrivee; n ; n = vari(n).antecedent)
        m_chemin.push_back(n);
}

void GPS::traiter(NoeudDijkstra * enCours)
{
    vari(enCours).liste = VariablesDynamiques::FERMEE;

    NoeudDijkstra::MapAcces const& map = enCours->noeudsAccessibles();
    for(NoeudDijkstra::MapAcces::const_iterator it = map.begin(); it != map.end(); ++it)
    {
        NoeudDijkstra* destination = it->first;
        double distance = it->second;

        if(vari(destination).liste != VariablesDynamiques::FERMEE)
        {
            double distanceDepuisLeDebut = distance + vari(enCours).distance;

            if(vari(destination).distance > distanceDepuisLeDebut)
            {
                vari(destination).distance = distanceDepuisLeDebut;
                vari(destination).antecedent = enCours;

                if(vari(destination).liste == VariablesDynamiques::OUVERTE)
                    m_listeOuverte->distanceDiminuee(destination);
                else
                    m_listeOuverte->push(destination);
            }
        }
    }
}

NoeudDijkstra * GPS::arrivee()
{
    return m_arrivee;
}

//Liste ouverte
void GPS::ListeOuverte::push(NoeudDijkstra * noeud)
{
    gps->vari(noeud).distancePresumeeVersDestination = gps->calculHeuristique(noeud);
    gps->vari(noeud).distancePresumee = gps->vari(noeud).distance + gps->vari(noeud).distancePresumeeVersDestination;
}
void GPS::ListeOuverte::distanceDiminuee(NoeudDijkstra * noeud)
{
    gps->vari(noeud).distancePresumee = gps->vari(noeud).distance + gps->vari(noeud).distancePresumeeVersDestination;
}

GPS::ListeOuverte::DonneeListe GPS::ListeOuverte::getDonnee(NoeudDijkstra * noeud)
{
    return DonneeListe(noeud, gps->vari(noeud).distancePresumee);
}

//ListeOuverteTableau
GPS::ListeOuverteTableau::ListeOuverteTableau(GPS * cible)
    : ListeOuverte(cible)
{

}

bool GPS::ListeOuverteTableau::vide()
{
    return m_liste.empty();
}

void GPS::ListeOuverteTableau::push(NoeudDijkstra * noeud)
{
    ListeOuverte::push(noeud);
    m_liste.push_back( getDonnee(noeud) );
}

NoeudDijkstra* GPS::ListeOuverteTableau::pop()
{
    std::list<DonneeListe>::iterator itMin = std::min_element(m_liste.begin(), m_liste.end());
    NoeudDijkstra* cible = itMin->cible;
    m_liste.erase(itMin);
    return cible;
}

//ListeOuverteTas
GPS::ListeOuverteTas::ListeOuverteTas(GPS * cible)
    : ListeOuverte(cible)
    , m_handles(cible->m_nombreDeNode)
{

}

GPS::ListeOuverteTas::~ListeOuverteTas()
{

}

bool GPS::ListeOuverteTas::vide()
{
    return m_tas.empty();
}

void GPS::ListeOuverteTas::push(NoeudDijkstra * noeud)
{
    ListeOuverte::push(noeud);
    m_handles[ noeud->indice() ] = m_tas.push( getDonnee(noeud) );
}

NoeudDijkstra * GPS::ListeOuverteTas::pop()
{
    return m_tas.pop().cible;
}

void GPS::ListeOuverteTas::distanceDiminuee(NoeudDijkstra * noeud)
{
    ListeOuverte::distanceDiminuee(noeud);
    m_tas.decreaseKey(m_handles[ noeud->indice() ], getDonnee(noeud) );
}

double GPS::distanceDuDepart(NoeudDijkstra * noeud)
{
    return vari(noeud).distance;
}

double GPS::distanceVersArrivee(NoeudDijkstra * noeud)
{
    return distanceDuDepart(m_arrivee) - distanceDuDepart(noeud);
}
