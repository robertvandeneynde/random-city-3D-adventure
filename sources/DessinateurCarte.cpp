#include "DessinateurCarte.h"

#include "Ville.h"

DessinateurCarte::DessinateurCarte(VertexArray & va)
    : Dessinateur(va)
{
}

void DessinateurCarte::bandeDeSeparation(Zone, int directionParal, QColor)
{

}

void DessinateurCarte::cubeDroit(double x, double dx, double y, double dy, double z, double dz, QColor couleur)
{
    sommets.nouvellePosition(x, y, 0);
    sommets.nouvellePosition(x+dx, y, 0);
    sommets.nouvellePosition(x+dx, y+dy, 0);
    sommets.nouvellePosition(x, y+dy, 0);
    for(int i = 0 ; i < 4 ; i++)
    {
        sommets.nouvelleCouleur(couleur);
        sommets.nouvelleNormale(0,0,1);
    }
}

void DessinateurCarte::cubeDroitTexture(double x, double dx, double y, double dy, double z, double dz, GLuint texture, QColor couleur)
{
    sommets.setCurrentTexture(texture);
    sommets.nouvelleTexCoord(0, 0);
    sommets.nouvelleTexCoord(dx, 0);
    sommets.nouvelleTexCoord(dx, dy);
    sommets.nouvelleTexCoord(0, dy);
    cubeDroit(x, dx, y, dy, z, dz, couleur);
}

void DessinateurCarte::cubePenche(Zone z, double dz, QColor couleur)
{
    cubeDroit(z.mX().debut(), z.mX().longueur(), z.mY().debut(), z.mY().longueur(), 0, 0, couleur);
}

void DessinateurCarte::pointilles(Zone, int directionParal, QColor)
{

}

void DessinateurCarte::trianglesPriorite(Ville *ville, double largeur, int n)
{

}
