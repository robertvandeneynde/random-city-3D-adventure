#ifndef GPS_H
#define GPS_H

#include <vector>
#include <list>
#include "NoeudDijkstra.h"
#include "VertexArray.h"
#include "Tas.h"

class GPS
{
    public :
        friend class Iterateur;
        class Iterateur
        {
            public :
                Iterateur() {
                    m_cible = NULL;
                    m_indice = -1;
                }
                Iterateur(GPS * cible) : m_cible(cible) {
                    m_indice = m_cible->m_chemin.size() - 1;
                }
                Iterateur operator++(int)
                {
                    Iterateur copie = *this;
                    ++(*this);
                    return copie;
                }
                Iterateur& operator++()
                {
                    m_indice--;
                    return *this;
                }
                NoeudDijkstra& operator*()
                {
                    return *m_cible->m_chemin[m_indice];
                }
                NoeudDijkstra* operator->()
                {
                    return m_cible->m_chemin[m_indice];
                }
                operator NoeudDijkstra*()
                {
                    return m_cible->m_chemin[m_indice];
                }
                bool valide()
                {
                    return m_indice >= 0;
                }
            private :
                GPS * m_cible;
                int m_indice;
        };
        struct VariablesDynamiques
        {
                VariablesDynamiques();
                double distance, distancePresumee, distancePresumeeVersDestination;
                enum {OUVERTE, FERMEE, AUCUNE} liste;
                NoeudDijkstra * antecedent;
        };
    public :
        GPS(size_t nombreDeNode);
        ~GPS();
        void calculer(NoeudDijkstra * depart, NoeudDijkstra * arrivee = NULL); //Itérateurs deviennent mauvais

        double distanceDuDepart(NoeudDijkstra *);
        double distanceVersArrivee(NoeudDijkstra *);

        NoeudDijkstra * arrivee();
        const VariablesDynamiques & infos(NoeudDijkstra *);
    private :
        void traiter(NoeudDijkstra* enCours);
        double calculHeuristique(NoeudDijkstra * noeud);
    private :
        class ListeOuverte
        {
            public :
                ListeOuverte(GPS * cible = NULL) { gps = cible; }
                virtual ~ListeOuverte() {}
                virtual bool vide() = 0;
                virtual NoeudDijkstra * pop() = 0;

                virtual void push(NoeudDijkstra *);
                virtual void distanceDiminuee(NoeudDijkstra *);
            protected :
                struct DonneeListe
                {
                    DonneeListe(NoeudDijkstra * laCible, double laDistance) : cible(laCible), distance(laDistance) {}
                    bool operator<(DonneeListe const& autre) const { return distance < autre.distance; }

                    NoeudDijkstra * cible;
                    double distance;
                };
                DonneeListe getDonnee(NoeudDijkstra *);
            protected :
                GPS * gps;
        };
        friend class ListeOuverteTableau;
        class ListeOuverteTableau : public ListeOuverte
        {
            public :
                ListeOuverteTableau(GPS *);
                bool vide();
                void push(NoeudDijkstra *);
                NoeudDijkstra * pop();
            private :
                std::list<DonneeListe> m_liste;
        };
        friend class ListeOuverteTas;
        class ListeOuverteTas : public ListeOuverte
        {
            public :
                ListeOuverteTas(GPS *);
                ~ListeOuverteTas();
                bool vide();
                void push(NoeudDijkstra *);
                NoeudDijkstra * pop();
                void distanceDiminuee(NoeudDijkstra *);
            private :
                typedef Tas<DonneeListe> MonTas;

                MonTas m_tas;
                std::vector<MonTas::Handle> m_handles;
        };

    private :
        VariablesDynamiques & vari(NoeudDijkstra *);
    private :
        size_t m_nombreDeNode;
        std::vector<VariablesDynamiques> m_variables;
        ListeOuverte * m_listeOuverte;
        NoeudDijkstra * m_arrivee;

        std::vector<NoeudDijkstra *> m_chemin;
        int m_indiceChemin;
};

#endif // GPS_H
