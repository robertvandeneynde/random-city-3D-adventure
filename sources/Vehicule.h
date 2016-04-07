#ifndef VEHICULE_H
#define VEHICULE_H

#include "ObjetControlable.h"

class Perso;
class Ville;

class Vehicule : public ObjetControlable
{
    public :
        Vehicule(Ville *);
        virtual ~Vehicule();
        void setPerso(Perso *);
        void action();
        void enterFrame();
    protected :
        Perso * m_perso;
        Ville * m_ville;
};

#endif // VEHICULE_H
