#ifndef INTER_H
#define INTER_H

#include <cstdlib>
#include <vector>

class Inter //Note : <fin> est NON comprise
{
    public :
        //Constructeurs
        static Inter fromCompris(int debut, int finComprise) { return Inter(debut, finComprise+1); }
        Inter() {}
        Inter(int debut, int fin) { m_inter[0] = debut; m_inter[1] = fin; }
        Inter(Inter const& autre) { m_inter[0] = autre.m_inter[0]; m_inter[1] = autre.m_inter[1]; }
        Inter& operator=(Inter const& autre) { m_inter[0] = autre.m_inter[0]; m_inter[1] = autre.m_inter[1]; return *this; }
        //Accesseurs / Infos
        int operator[](int n) const {
            return m_inter[n];
        }
        int getC(int n) const {
            return m_inter[n] - n;
        }
        int debut() const {
            return m_inter[0];
        }
        int fin() const {
            return m_inter[1];
        }
        int finComprise() const {
            return fin() - 1;
        }
        int longueur()  const {
            return fin() - debut();
        }
        int hasard() const {
            return debut() + rand() % longueur();
        }
        double moyenne() {
            return (debut() + fin()) / 2.0;
        }
        double getPondere(double f) {
            return debut() + f * longueur();
        }
        bool comprendBordure(int v) const {
            return debut() <= v && v <= fin();
        }
        bool comprend(int v) const {
            return debut() <= v && v < fin();
        }
        bool comprend(Inter autre) const {
            return comprend(autre.debut()) && comprend(autre.finComprise());
        }
        bool intersects(Inter autre) const {
            return comprend(autre.debut()) || comprend(autre.finComprise());
        }
        bool intersectsBordure(Inter autre) const {
            return comprendBordure(autre.debut()) || comprendBordure(autre.fin());
        }
        Inter translate(int t0, int t1) const {
            return Inter(debut() + t0, fin() + t1);
        }
        Inter translate(int t) const {
            return translate(t,t);
        }
        Inter agrandi(int t) const {
            return translate(-t, t);
        }
        Inter agrandi(int t0, int t1) const {
            return translate(-t0, t1);
        }
        Inter reduit(int t) const {
            return translate(t, -t);
        }
        Inter reduit(int t0, int t1) const {
            return translate(t0, -t1);
        }
        std::vector<Inter> coupeEnDeux(int valeur) const
        {
            std::vector<Inter> res(2);
            res[0] = *this;
            res[1] = res[0].couperGetSuivant(valeur);
            return res;
        }
        //Operations
        //Coupe this en deux de telle sorte que ancien_this = nouveau_this | return
        //Exemple [5,10[ coupe en 7 devient [5,7[ et renvoie [7,10[
        Inter couperGetSuivant(int valeur)
        {
            Inter suivant(valeur, this->m_inter[1]);
            this->m_inter[1] = valeur;
            return suivant;
        }
        //Coupe this en deux de telle sorte que ancien_this = return | espace | nouveau_this
        //Exemple [5,10[ coupe en 7 devient [7,10[ et renvoie [5,7[
        Inter couperGetPrecedent(int valeur)
        {
            Inter precedent(this->m_inter[0], valeur);
            this->m_inter[0] = valeur;
            return precedent;
        }
    private :
        int m_inter[2];
};

#endif // INTER_H
