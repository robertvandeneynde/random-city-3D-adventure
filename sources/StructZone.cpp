#include "StructZone.h"

#include <algorithm>

StructZone StructZone::fromDirection(int direction, Inter paralele_a_direction, Inter perpendiculaire_a_direction)
{
    StructZone z;
    z[direction] = paralele_a_direction;
    z[1-direction] = perpendiculaire_a_direction;
    return z;
}

StructZone StructZone::fusion(StructZone s1, StructZone s2)
{
    Inter leX(std::min(s1.x.debut(), s2.x.debut()), std::max(s1.x.fin(), s2.x.fin()));
    Inter leY(std::min(s1.y.debut(), s2.y.debut()), std::max(s1.y.fin(), s2.y.fin()));
    return StructZone(leX,leY);
}

QPoint StructZone::debut() const
{
    return QPoint(x.debut(), y.debut());
}

QPoint StructZone::fin() const
{
    return QPoint(x.fin(), y.fin());
}

QPoint StructZone::finComprise() const
{
    return QPoint(x.finComprise(), y.finComprise());
}

QPoint StructZone::taille() const
{
    return QPoint(x.longueur(), y.longueur());
}

QPoint StructZone::coin(int a, int b) const
{
    return QPoint(x.getC(a), y.getC(b));
}

QString StructZone::toQString() const
{
    return QString("Zone @(") + QString::number(x.debut()) + "," + QString::number(y.debut()) +
            ")(" + QString::number(x.longueur()) + "x" + QString::number(y.longueur()) + ")";
}
