#include "stay_high.h"

#include "Ville.h"
#include "Element.h"

#include <QObject>

namespace objectifs {

void stay_high::setVille(Ville* ville)
{
    // find the highest building

    Batiment* highest = ville->findHighestBuilding();

    qDebug() << highest->zone().coin(0,0);

    // TODO : add an indicator on top of the highest building

    QObject::connect(ville, &Ville::batimentSteppedOn, [highest](Batiment* building){
        if (building == highest)
            qDebug() << "Cling!";
    });

    // connect signal "when stepped on building"
    //  start timer, when finished, give objectifs
}

}
