#ifndef BLOC_H
#define BLOC_H

#include <vector>
#include <set>

#include "Ville.h"
#include "RemplisseurDeScene.h"
#include "ListeChainee.h"

class ElementDeBloc;
class Batiment;
class Jardin;

class Bloc
{
    public:
        Bloc(Zone zone);
        ~Bloc();
        void nouveauBatiment(Zone);

        int nombreBatiments() { return m_batiments.size(); }
        int nombreJardins() { return m_jardins.size(); }
        int nombreElements() { return m_elements.size(); }

        double hauteurSol() { return m_hauteurSol; }
        void mettreAJourHauteurs();
        StructZone zone() { return m_zone; }

        void remplir(RemplisseurDeScene &);
        Batiment * chercherPlateformeHelicoHasard();

    public :
        class AttributionCouleurs
        {
            public :
                AttributionCouleurs(Bloc*);
                bool couleursAttribuees();
                int nombreAppels() { return nombreAppel; }
            private :
                void afficherCouleursZone();
                bool attribue(ListeChaineeStd<Batiment*>::Iterateur);
                int & couleur(Batiment*);
                std::set<Batiment*> & voisins(Batiment*);
                void creerConnexions();
            private :
                struct MonFacteurDeTri {
                    Bloc::AttributionCouleurs* self;
                    MonFacteurDeTri(Bloc::AttributionCouleurs* s) : self(s) {}
                    bool operator()(Batiment*, Batiment*);
                };
            private :
                Bloc * bloc;

                ListeChaineeStd<Batiment*> ordreEssai;
                std::vector<int> couleurs;
                std::vector< std::set<Batiment*> > connexions;
                QPoint fin;

                bool finDeParcours;
                QFile* fichierLog;
                QTextStream log;
                int nombreAppel;
        };

    private :
        void creerJardins();
    private :
        ListeChaineeStd<Batiment*> m_batiments;
        ListeChaineeStd<Jardin*> m_jardins;
        ListeChaineeStd<ElementDeBloc*> m_elements;
        Zone m_zone;
        Ville * m_ville;
        double m_hauteurSol;

        QList<QFile*> m_listeDeFichiersLog;
};

#endif // BLOC_H
