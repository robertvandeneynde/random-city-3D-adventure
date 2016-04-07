#include "ElementRotatif.h"

#include "fonctionsMathematiques.h"
#include "Dessinateur.h"

ElementRotatif::ElementRotatif()
    : donnees(NULL)
    , angle(0)
    , axeRotation(0,0,1)
    , m_vitesseRotation(0)
    , m_vitesseRotationMax(0)
{

}
ElementRotatif::~ElementRotatif()
{

}

void ElementRotatif::translater()
{
    if(donnees)
    {
        double matrice[16];
        Math::Matrice::translation(matrice, -coord.x(), -coord.y(), -coord.z());
        donnees->transformerDernierePosition(matrice, donnees->length());
    }
}
void ElementRotatif::appliquerVitesse()
{
    angle += m_vitesseRotation;
}

void ElementRotatif::setVitesseMax(double v)
{
    m_vitesseRotation = m_vitesseRotationMax = v;
}

void ElementRotatif::setVitesse(double v)
{
    m_vitesseRotation = v;
    if(m_vitesseRotation < 0)
        m_vitesseRotation = 0;
    if(m_vitesseRotation > m_vitesseRotationMax)
        m_vitesseRotation = m_vitesseRotationMax;
}

void ElementRotatif::augmenterVitesse(double v)
{
    setVitesse(m_vitesseRotation + v);
}

void ElementRotatif::dessiner()
{
    Repere R;
    R.translate(coord).rotate(Math::enRadians(angle), axeRotation);
    if(donnees)
        donnees->dessiner();
    dessinerEnfants();
}
void ElementRotatif::dessinerEnfants()
{

}
