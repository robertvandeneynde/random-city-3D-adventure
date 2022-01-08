#ifndef STAY_HIGH_H
#define STAY_HIGH_H

#include "../objectif.h"

#include <QObject>

namespace objectifs {

class stay_high : public Objectif //, public QObject
{
    //Q_OBJECT
public:
    void setVille(Ville*) override;
};

}

#endif // STAY_HIGH_H
