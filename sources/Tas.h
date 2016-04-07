#ifndef TAS_H
#define TAS_H

#include <QDebug>
#include <QString>
#include <cassert>
#include <utility>
#include <algorithm>

template <typename T, typename Comp = std::less<T> >
class Tas
{
    private :
        struct Node
        {
                Node(T d)
                    : donnee(d)
                {
                    enfants[0] = enfants[1] = NULL;
                    parent = NULL;
                }
                ~Node()
                {
                    delete enfants[0];
                    delete enfants[1];
                }
                Node * enfants[2];
                Node * parent;
                T donnee;
        };
    public :
        //Encapsule un Node
        class Handle
        {
            public :
                Handle() {
                    node = NULL;
                }
                Handle(Handle const& autre) {
                    node = autre.node;
                }
                T donnee() const {
                    return node->donnee;
                }
            private :
                Handle(Node * leNode) {
                    node = leNode;
                }
                friend class Tas;
                Node * node;
        };
    public :
        Tas(Comp c = Comp())
            : m_comparaison(c)
        {
            m_racine = NULL;
            m_size = 0;
        }

        ~Tas()
        {
            delete m_racine;
        }

        Handle push(T donnee)
        {
            Node * nouveau = new Node(donnee);
            m_size++;

            if(m_racine == NULL)
                m_racine = nouveau;
            else
            {
                //Chemin pour aller a taille = (Chemin pour aller a taille/2) concat (taille modulo 2)
                Node * parent = noeudDeTaille(m_size / 2);
                parent->enfants[m_size % 2] = nouveau;
                nouveau->parent = parent;

                remonterNoeud(nouveau);
            }

            return Handle(nouveau);
        }

        T pop()
        {
            T res = m_racine->donnee;
            Node* toPop = m_racine;

            if(m_size == 1)
            {
                m_racine = NULL;
            }
            else
            {
                Node * cible = noeudDeTaille(m_size);
                //On l'enlève du bas. cible->parent est bien different de toPop
                cible->parent->enfants[ trouverNumero(cible) ] = NULL;
                //On le met en racine
                m_racine = cible;
                cible->parent = NULL;
                for(int i = 0 ; i < 2 ; i++)
                {
                    cible->enfants[i] = toPop->enfants[i];
                    if(cible->enfants[i])
						cible->enfants[i]->parent = cible;
                }
                //On le descend pour garder la notion de tas
                for(int n = minEnfant(cible) ; n != -1 && m_comparaison(cible->enfants[n]->donnee, cible->donnee) ; n = minEnfant(cible))
                    descendreNoeud(cible, n);
            }

            m_size--;
			toPop->enfants[0] = toPop->enfants[1] = NULL; //Pour éviter la destruction récursive
            delete toPop;
            return res;
        }

        void decreaseKey(Handle h, T donnee)
        {
            Node * noeud = h.node;
            noeud->donnee = donnee;
            remonterNoeud(noeud);
        }

        bool empty() const
        {
            return m_racine == NULL;
        }

        int size() const
        {
            return m_size;
        }

    private :
        Node * noeudDeTaille(int taille)
        {
            if(taille == 1)
                return m_racine;
            else
                return noeudDeTaille(taille / 2)->enfants[taille % 2];
        }

        int minEnfant(Node* parent)
        {
            Node* gauche = parent->enfants[0];
            Node* droite = parent->enfants[1];
            if(droite)
                return m_comparaison(gauche->donnee, droite->donnee) ? 0 : 1;
            else if(gauche)
                return 0;
            else
                return -1;
        }

        void descendreNoeud(Node* aDescendre, int indiceEnfant)
        {
            Node * aMonter = aDescendre->enfants[indiceEnfant];
            Node * autreEnfant = aDescendre->enfants[1-indiceEnfant];

            if(autreEnfant)
                autreEnfant->parent = aMonter;

            if(aDescendre->parent)
                aDescendre->parent->enfants[ trouverNumero(aDescendre) ] = aMonter;

            for(int i = 0 ; i < 2 ; i++)
            {
                aDescendre->enfants[i] = aMonter->enfants[i];
                if(aDescendre->enfants[i])
                    aDescendre->enfants[i]->parent = aDescendre;
            }

            aMonter->parent = aDescendre->parent;
            aDescendre->parent = aMonter;
            aMonter->enfants[indiceEnfant] = aDescendre;
            aMonter->enfants[1-indiceEnfant] = autreEnfant;
			
			if(aDescendre == m_racine)
				m_racine = aMonter;
        }

        int trouverNumero(Node* enfant)
        {
            assert(enfant->parent);
            return enfant->parent->enfants[0] == enfant ? 0 : 1;
        }

        void remonterNoeud(Node * noeud)
        {
            while(noeud->parent && m_comparaison(noeud->donnee, noeud->parent->donnee))
                descendreNoeud(noeud->parent, trouverNumero(noeud));
        }

        Node * m_racine;
        int m_size;
        Comp m_comparaison;
};

template <typename T, typename Comp = std::less<T> >
class TasTab
{
    private :
        typedef unsigned int Indice;
        struct Node
        {
            Node(T t, Indice i) : donnee(t), indice(i) {}

            T donnee;
            Indice indice;

            bool racine() { return indice == 0; }
            Indice parent() { return (indice - 1) / 2; }
            Indice enfant() { return indice * 2 + 1; }
        };
    public :
        //Encapsule un Node
        class Handle
        {
            public :
                Handle() {
                    node = NULL;
                }
                Handle(Handle const& autre) {
                    node = autre.node;
                }
                T donnee() const {
                    return node->donnee;
                }
            private :
                Handle(Node * leNode) {
                    node = leNode;
                }
                friend class TasTab;
                Node * node;
        };
    public :
        TasTab(Comp c = Comp())
            : m_comparaison(c)
        {
            //m_size = 0;
        }

        ~TasTab()
        {
            for(Indice i = 0 ; i < m_tab.size(); i++)
                delete m_tab[i];
        }

        Handle push(T donnee)
        {
            Node * nouveau = new Node(donnee, m_tab.size());
            m_tab.push_back(nouveau);
            remonterNoeud(nouveau);
            return Handle(nouveau);
        }

        T pop()
        {
            T res = m_tab.front()->donnee;

            echanger(m_tab.front(), m_tab.back());
            delete m_tab.back();
            m_tab.pop_back();
            if(! m_tab.empty()) {
                Node* cible = m_tab.front();
                Node* enfant;
                while( (enfant = minEnfant(cible)) && m_comparaison(enfant->donnee, cible->donnee))
                    echanger(cible, enfant);
            }
            return res;
        }

        void decreaseKey(Handle h, T donnee)
        {
            Node * noeud = h.node;
            noeud->donnee = donnee;
            remonterNoeud(noeud);
        }

        bool empty() const
        {
            return m_tab.empty();
        }

        int size() const
        {
            return m_tab.size();
        }

    private :
        Node* minEnfant(Node* parent)
        {
            Indice gauche = parent->enfant();
            Indice droite = gauche + 1;
            if(droite < m_tab.size())
                if(m_comparaison(m_tab[gauche]->donnee, m_tab[droite]->donnee))
                    return m_tab[gauche];
                else
                    return m_tab[droite];
            else if(gauche < m_tab.size())
                return m_tab[gauche];
            else
                return NULL;
        }

        void echanger(Node* n1, Node* n2)
        {
            m_tab[n1->indice] = n2;
            m_tab[n2->indice] = n1;
            std::swap(n1->indice, n2->indice);
        }

        void remonterNoeud(Node * noeud)
        {
            while(m_tab.front() != noeud && m_comparaison(noeud->donnee, m_tab[noeud->parent()]->donnee))
                echanger(noeud, m_tab[noeud->parent()]);
        }

        std::vector<Node*> m_tab;
        Comp m_comparaison;
};

#endif // TAS_H
