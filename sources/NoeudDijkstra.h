#ifndef NOEUDDIJKSTRA_H
#define NOEUDDIJKSTRA_H

#include <QVector3D>
#include <QVector2D>
#include <map>

class NoeudDijkstra
{
    public :
        typedef std::map<NoeudDijkstra *, double> MapAcces;
    public :
        NoeudDijkstra();
        virtual ~NoeudDijkstra();

        virtual QVector3D position3D() const = 0;

        void setIndice(size_t i);
        void ajouterAcces(NoeudDijkstra * cible);
        void enleverAcces(NoeudDijkstra * cible);
		void refreshPoidsDijkstra();
        static void ajouterAccesSymetrique(NoeudDijkstra*, NoeudDijkstra*);
        void enleverTousLesAcces(bool symetrique = false);

        MapAcces const& noeudsAccessibles() const;
        size_t indice() const;
    private :
        size_t m_indice;
        MapAcces m_acces;
};

#endif // NOEUDDIJKSTRA_H
