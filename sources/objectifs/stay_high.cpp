#include "stay_high.h"

#include "Ville.h"
#include "Element.h"

#include <QObject>

namespace objectifs {

void stay_high::setVille(Ville* ville)
{
    // find the highest building

    Batiment* highest = ville->findHighestBuilding();

    qDebug() << QVector3D(QVector2D(highest->zone().coin(0,0)), (float)highest->hauteurAbsolue());

    // TODO : add an indicator on top of the highest building
    //ville->addObjetScene(new Cube());

    QObject::connect(ville, &Ville::batimentSteppedOn, [highest](Batiment* building){
        if (building == highest)
            qDebug() << "Cling!";
    });

    // connect signal "when stepped on building"
    //  start timer, when finished, give objectifs
}

}
