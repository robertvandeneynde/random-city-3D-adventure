#ifndef OBJECTIF_H
#define OBJECTIF_H

class Ville;

class Objectif
{
public:
    Objectif();
    virtual void setVille(Ville* v) = 0;
};

#endif // OBJECTIF_H
