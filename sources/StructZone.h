#ifndef STRUCTZONE_H
#define STRUCTZONE_H

#include "Inter.h"
#include <QPoint>
#include <QString>

struct StructZone
{
    static StructZone fromDirection(int direction, Inter paralele_a_direction, Inter perpendiculaire_a_direction);
    static StructZone fusion(StructZone s1, StructZone s2);
    QString toQString() const;

    StructZone() : x(d[0]), y(d[1]) {}
    StructZone(Inter mX, Inter mY) : x(d[0]), y(d[1]) {x = mX; y = mY; }
    StructZone(StructZone const& autre) : x(d[0]), y(d[1]) { x = autre.x; y = autre.y; }
    StructZone& operator=(StructZone const& autre) { x = autre.x; y = autre.y; return *this; }

    Inter& x;
    Inter& y;
    Inter d[2];

    Inter & operator[](int n) { return d[n]; }
    Inter const & operator[](int n) const { return d[n]; }

    QPoint debut() const;
    QPoint fin() const;
    QPoint finComprise() const;
    QPoint taille() const;
    QPoint coin(int a, int b) const;
};

#endif // STRUCTZONE_H
