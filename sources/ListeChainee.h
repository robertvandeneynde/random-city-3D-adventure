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
            Node() {}
            Node(T d) : donnee(d) { }
            T donnee;
            Node* suivant;
            Node* precedent;
        };
    public :
        class iterator : public std::iterator<std::bidirectional_iterator_tag, T>
        {
            public :
                iterator() {
                    node = NULL;
                }
                T& donnee() const {
                    return node->donnee;
                }
                T& operator*() const {
                    return donnee();
                }
                T* operator->() {
                    return &node->donnee;
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

        class Iterateur : public std::iterator<std::forward_iterator_tag, T>
        {
            public :
                Iterateur() {
                    liste = NULL;
                }
                Iterateur(ListeChaineeStd &cible) {
                    liste = &cible;
                    it = liste->begin();
                }
                T& donnee() const {
                    return *it;
                }
                T& operator*() const {
                    return *it;
                }
                Iterateur operator++(int) {
                    Iterateur copie = *this;
                    ++it;
                    return copie;
                }
                Iterateur& operator++() {
                    ++it;
                    return *this;
                }

                bool operator==(Iterateur const& autre) const {
                    return it == autre.it;
                }
                bool operator!=(Iterateur const& autre) const {
                    return it != autre.it;
                }
                bool valide() const {
                    return it != liste->end();
                }
                void insert(T t) {
                    liste->insert(*this, t);
                }
                void erase() {
                    liste->erase(*this);
                }
            private :
                iterator it;
                ListeChaineeStd* liste;
        };

    public :
        ListeChaineeStd()
        {
            m_dummy.suivant = m_dummy.precedent = &m_dummy;
            m_size = 0;
        }

        ListeChaineeStd(ListeChaineeStd& autre)
        {
            m_dummy.suivant = m_dummy.precedent = &m_dummy;
            m_size = 0;
            for(iterator it = autre.begin(); it != autre.end(); ++it)
                push_back(*it);
        }

        ListeChaineeStd& operator=(ListeChaineeStd& autre)
        {
            clear();
            for(iterator it = autre.begin(); it != autre.end(); ++it)
                push_back(*it);
        }

        ~ListeChaineeStd()
        {
            clear();
        }

        void clear()
        {
            iterator it = begin();
            while(it != end())
            {
                Node * p = it.node;
                ++it;
                delete p;
            }
            m_size = 0;
        }

        iterator insert(iterator it, T donnee)
        {
            Node* cible = new Node(donnee);

            cible->suivant = it.node;
            cible->precedent = it.node->precedent;

            it.node->precedent = cible;
            cible->precedent->suivant = cible;

            m_size++;
            return cible;
        }

        void erase(iterator it)
        {
            it.node->precedent->suivant = it.node->suivant;
            it.node->suivant->precedent = it.node->precedent;
            m_size--;
            delete it.node;
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
            iterator it = begin();
            for( ; i < m_size / 2 ; i++)
                gauche.push_back(*it++);
            for( ; i < m_size ; i++)
                droite.push_back(*it++);

            gauche.trier(comp);
            droite.trier(comp);

            it = begin();
            iterator itG = gauche.begin();
            iterator itD = droite.begin();

            while(itG != gauche.end() && itD != droite.end())
                *it++ = comp(*itG, *itD) ? *itG++ : *itD++;

            while(itG != gauche.end())
                *it++ = *itG++;
            while(itD != droite.end())
                *it++ = *itD++;
        }

    private :
        template <typename Comp>
        void trier(Comp comp, iterator debut, iterator fin, int dist)
        {
            if(dist < 2)
                return;

            iterator milieu = debut;
            for(unsigned int i = 0; i < dist / 2; i++)
                ++milieu;

            trier(comp, debut, milieu, dist / 2);
            iterator lastGauche = milieu;
            --lastGauche;

            trier(comp, milieu, fin, dist - dist / 2);
            milieu = lastGauche;
            ++milieu;

            while(debut != fin && milieu != fin)
                if(comp(*debut, *milieu))
                    ++debut;
                else
                    deplace(debut.node, (milieu++).node);
        }

        void deplace(Node* pos, Node* cible)
        {
            cible->precedent->suivant = cible->suivant;
            cible->suivant->precedent = cible->precedent;
            pos->precedent->suivant = cible.node;
            cible->precedent = pos->precedent;
            pos->precedent = cible;
            cible->suivant = pos;
        }

    public :

        bool empty() const {
            return m_size == 0;
        }

        void push_front(T donnee) {
            insert(begin(), donnee);
        }

        void push_back(T donnee) {
            insert(end(), donnee);
        }

        void pop_back() {
            erase(last());
        }

        void pop_front() {
            erase(begin());
        }

        Iterateur premier() {
            return Iterateur(*this);
        }

        iterator begin() {
            return m_dummy.suivant;
        }

        iterator end() {
            return &m_dummy;
        }

        iterator last() {
            return m_dummy.precedent;
        }

        iterator beforeBegin() {
            return &m_dummy;
        }

        T& front() {
            return m_dummy.suivant->donnee;
        }

        T& back() {
            return m_dummy.precedent->donnee;
        }

        unsigned int size() {
            return m_size;
        }

        // (delete T) doit etre possible
        void deleteAll()
        {
            for(iterator it = begin() ; it != end(); ++it)
                delete *it;
        }

    private :
        Node m_dummy;
        unsigned int m_size;
};

#endif // LISTECHAINEE_H
