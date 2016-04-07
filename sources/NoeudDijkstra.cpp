#include "NoeudDijkstra.h"

#include <cassert>

NoeudDijkstra::NoeudDijkstra()
{
    m_indice = -1; //ie la plus grande valeur d'un uint
}

NoeudDijkstra::~NoeudDijkstra()
{

}

void NoeudDijkstra::setIndice(size_t i)
{
    m_indice = i;
}
size_t NoeudDijkstra::indice() const
{
    assert(m_indice != size_t(-1));
    return m_indice;
}

void NoeudDijkstra::ajouterAcces(NoeudDijkstra *cible)
{
    m_acces[cible] = (cible->position3D() - position3D()).length();
}

void NoeudDijkstra::refreshPoidsDijkstra()
{
	for(auto & paire : m_acces)
		paire.second = (paire.first->position3D() - this->position3D()).length();
}

void NoeudDijkstra::ajouterAccesSymetrique(NoeudDijkstra *a, NoeudDijkstra *b)
{
    a->ajouterAcces(b);
    b->ajouterAcces(a);
}

void NoeudDijkstra::enleverAcces(NoeudDijkstra *cible)
{
    MapAcces::iterator it = m_acces.find(cible);
    if(it != m_acces.end())
        m_acces.erase(it);
}

void NoeudDijkstra::enleverTousLesAcces(bool symetrique)
{
    if(symetrique)
        for(MapAcces::iterator it = m_acces.begin(); it != m_acces.end(); ++it)
            it->first->enleverAcces(this);

    m_acces.clear();
}

NoeudDijkstra::MapAcces const& NoeudDijkstra::noeudsAccessibles() const
{
    return m_acces;
}
