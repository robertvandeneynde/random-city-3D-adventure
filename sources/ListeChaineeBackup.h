#ifndef LISTECHAINEE_H
#define LISTECHAINEE_H

#include <QDebug>
#include <iterator>

template <typename T>
class ListeChaineeStd
{
    private :
        struct Node
        {
            Node(T d)
                : donnee(d)
            {
                suivant = precedent = NULL;
            }
            T donnee;
            Node * suivant;
            Node * precedent;
        };
    public :
        //Encapsule un Node
        class Iterateur : public std::iterator<std::forward_iterator_tag, T>
        {
            public :
                Iterateur() {
                    node = NULL;
                }
                Iterateur(Iterateur const& autre) {
                    node = autre.node;
                }
                T& donnee() const {
                    return node->donnee;
                }
                T& operator*() const {
                    return donnee();
                }
                Iterateur operator++(int) {
                    Iterateur copie = *this;
                    node = node->suivant;
                    return copie;
                }
                Iterateur& operator++() {
                    node = node->suivant;
                    return *this;
                }
                bool valide() const {
                    return node != NULL;
                }
                bool operator==(Iterateur const& autre) const {
                    return node == autre.node;
                }
                bool operator!=(Iterateur const& autre) const {
                    return node != autre.node;
                }
            private :
                Iterateur(Node * leNode, ListeChaineeStd* liste)
                    : liste(liste)
                    , node(leNode)
                {

                }
                friend class ListeChaineeStd;
                Node * node;
                ListeChaineeStd* liste;
        };

        class iterator : public std::iterator<std::bidirectional_iterator_tag, T>
        {
            public :
                iterator() {
                    node = NULL;
                }
                iterator(iterator const& autre) {
                    node = autre.node;
                }
                T& donnee() const {
                    return node->donnee;
                }
                T& operator*() const {
                    return donnee();
                }
                iterator operator--(int)
                {
                    iterator copie = *this;
                    node = node->precedent;
                    return copie;
                }
                iterator& operator--()
                {
                    node = node->precedent;
                    return *this;
                }
                iterator operator++(int)
                {
                    iterator copie = *this;
                    node = node->suivant;
                    return copie;
                }
                iterator& operator++() {
                    node = node->suivant;
                    return *this;
                }
                bool operator==(iterator const& autre) const {
                    return node == autre.node;
                }
                bool operator!=(iterator const& autre) const {
                    return node != autre.node;
                }
            private :
                iterator(Node * leNode) {
                    node = leNode;
                }
                friend class ListeChaineeStd;
                Node * node;
        };

    public :
        ListeChaineeStd()
        {
            m_premier = m_dernier = NULL;
            m_size = 0;
        }

        ListeChaineeStd(ListeChaineeStd const& autre)
        {
            m_premier = m_dernier = NULL;
            m_size = 0;
            for(Node* n = autre.m_premier; n; n = n->suivant)
                push_back(n->donnee);
        }

        ListeChaineeStd& operator=(ListeChaineeStd const& autre)
        {
            clear();
            for(Node* n = autre.m_premier; n; n = n->suivant)
                push_back(n->donnee);
        }

        ~ListeChaineeStd()
        {
            clear();
        }

        void push_front(T donnee)
        {
            Node * cible = new Node(donnee);
            if(! m_premier)
                m_premier = m_dernier = cible;
            else {
                m_premier->suivant = cible;
                cible->suivant = m_premier;
                m_premier = cible;
            }
            m_size++;
        }

        void push_back(T donnee)
        {
            pushBackNode(new Node(donnee));
        }

        bool empty() const
        {
            return m_size == 0;
        }

        template <typename Comp>
        void trier(Comp comp)
        {
            //Tri fusion
            if(m_size < 2)
                return;

            ListeChaineeStd gauche;
            ListeChaineeStd droite;

            unsigned int i = 0;
            Iterateur it = premier();
            for( ; i < m_size / 2 ; i++)
                gauche.push_back(*it++);
            for( ; i < m_size ; i++)
                droite.push_back(*it++);

            gauche.trier(comp);
            droite.trier(comp);

            it = premier();
            Iterateur itG = gauche.premier();
            Iterateur itD = droite.premier();

            while(itG.valide() && itD.valide())
                *it++ = comp(*itG, *itD) ? *itG++ : *itD++;

            while(itG.valide())
                *it++ = *itG++;
            while(itD.valide())
                *it++ = *itD++;
        }

        void recevoir_back(ListeChaineeStd& autre)
        {
            pushBackNode(autre.m_premier);
            autre.m_premier = autre.m_dernier = NULL;
            autre.m_size = 0;
        }

        void enlever(Iterateur it)
        {
            if(it.node->precedent)
                it.node->precedent->suivant = it.node->suivant;
            if(it.node->suivant)
                it.node->suivant->precedent = it.node->precedent;
            if(m_premier == it.node)
                m_premier = it.node->suivant;
            if(m_dernier == it.node)
                m_dernier = it.node->precedent;

            delete it.node;
            m_size--;
        }

        Iterateur find(T donnee)
        {
            for(Node * n = m_premier ; n ; n = n->suivant)
                if(n->donnee == donnee)
                    return n;
            return NULL;
        }

        void clear()
        {
            Node * n = m_premier;
            while(n)
            {
                Node * p = n;
                n = n->suivant;
                delete p;
            }
            m_premier = m_dernier = NULL;
            m_size = 0;
        }

        void pop_back()
        {
            enlever(dernier());
        }

        void pop_front()
        {
            enlever(premier());
        }

        Iterateur premier()
        {
            return Iterateur(m_premier, this);
        }

        Iterateur dernier()
        {
            return Iterateur(m_dernier, this);
        }

        iterator begin()
        {
            return iterator(m_premier);
        }

        iterator end()
        {
            return iterator(NULL);
        }

        T& front()
        {
            return m_premier->donnee;
        }

        T& back()
        {
            return m_dernier->donnee;
        }

        unsigned int size()
        {
            return m_size;
        }

        // (delete T) doit etre possible
        void deleteAll()
        {
            for(Node * n = m_premier ; n ; n = n->suivant)
                delete n->donnee;
        }

    private :
        void pushBackNode(Node* cible)
        {
            if(!m_dernier)
                m_premier = m_dernier = cible;
            else {
                m_dernier->suivant = cible;
                cible->precedent = m_dernier;
                m_dernier = cible;
            }
            m_size++;
        }
    private :
        Node * m_premier;
        Node * m_dernier;
        unsigned int m_size;
};

#endif // LISTECHAINEE_H
