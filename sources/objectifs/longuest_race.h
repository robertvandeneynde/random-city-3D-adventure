#ifndef LONGUEST_RACE_H
#define LONGUEST_RACE_H

#include "../objectif.h"

namespace objectifs {

class longuest_race : public Objectif
{
public:
    void setVille(Ville* v) override;
};

}

#endif // LONGUEST_RACE_H
