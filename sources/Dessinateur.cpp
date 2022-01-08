#include "Dessinateur.h"
#include "Ville.h"

Dessinateur::Dessinateur(VertexArray & cible)
	: sommets(cible)
{

}

void Dessinateur::bandeDeSeparation(Zone zoneLiee, int dir, QColor couleur)
{
    StructZone zone = zoneLiee;

    const double largeurLigne = 0.20;
    const double hauteur = 0.01 * 1.5;

    double interv[2][2];
    interv[dir][0] = zone[dir].debut();
    interv[dir][1] = zone[dir].fin();
    interv[1-dir][0] = zone[1-dir].moyenne() - largeurLigne/2;
    interv[1-dir][1] = interv[1-dir][0] + largeurLigne;

    for(int i = 0 ; i < 4 ; i++)
    {
        int a = i/2, b = (i+1)%4 / 2; //(0,0), (0,1), (1,1), (1,0)

        sommets.nouvellePosition(interv[0][a], interv[1][b], zoneLiee.hauteurBaseCoin(a,b));
        sommets.nouvelleNormale(0,0,-1);
        sommets.nouvelleCouleur(couleur);
    }

    double haut[] = {0, 0, hauteur};
    sommets.extrude(haut);
}

void Dessinateur::pointilles(Zone zoneLiee, int dir, QColor couleur)
{
    StructZone zone = zoneLiee;
    const double largeurLigne = 0.20;
    const double hauteur = 0.01 * 1.5;
    double haut[] = {0, 0, hauteur};

    double interv[2][2];
    interv[dir][0] = zone[dir].debut() + 0.25;
    interv[dir][1] = zone[dir].debut() + 0.75;
    interv[1-dir][0] = zone[1-dir].moyenne() - largeurLigne/2;
    interv[1-dir][1] = interv[1-dir][0] + largeurLigne;

    double limite = zone[dir].fin();
    while(interv[dir][1] <= limite)
    {
        for(int i = 0 ; i < 4 ; i++)
        {
            int a = i/2, b = (i+1)%4 / 2;
            sommets.nouvellePosition(interv[0][a], interv[1][b], zoneLiee.hauteurBaseCoin(a,b));
            sommets.nouvelleNormale(0,0,-1);
            sommets.nouvelleCouleur(couleur);
        }
        sommets.extrude(haut);

        interv[dir][0] += 0.75;
        interv[dir][1] += 0.75;
    }
}

void Dessinateur::trianglesPriorite(Ville * ville, double largeur, int n)
{
	using namespace Math::Matrice;
	Pile& pile = sommets.pileMatrice();
	Pile::Nouveau r(pile);

    double dec = 0.05/2 * largeur;
    largeur *= 0.95;
    double l = largeur / n;
    double points[4][3] = {
        {dec+l, 0, 0.015},
        {dec+0, 0, 0.015},
        {dec+l/2, -l*1.5, 0.015},
        {dec+l/2, -l*1.5, 0.015},
    };

    static const double normale[3] = {0,0,1};
    static const float couleur[3] = {0.7,0.7,0.7};


    double matrice[16];
    for(int i = 0 ; i < n ; i++)
    {
        for(int j = 0 ; j < 4 ; j++)
        {
            sommets.nouvellePosition(points[j]);
            sommets.nouvelleNormale(normale);
            sommets.nouvelleCouleur(couleur);

            sommets.transformerDernierPoint();
            double *pos = sommets.dernierePosition();
            pos[2] += 0.02 + ville->hauteurEffective(pos[0], pos[1]);
        }

        Math::Matrice::translation(matrice, l, 0, 0);
		pile.multiplier(matrice);
	}
}

void Dessinateur::cubeDroit(double x, double dx, double y, double dy, double z, double dz, QColor couleur)
{
    double coins[4][2] = {
        {x, y},
        {x, y+dy},
        {x+dx, y+dy},
        {x+dx, y}
    };

    for(int i = 0 ; i < 4 ; i++)
    {
        sommets.nouvellePosition(coins[i][0], coins[i][1], z);
        sommets.nouvelleNormale(0,0,-1);
        sommets.nouvelleCouleur(couleur);
    }

    double toExtrude[] = {0,0, dz};
	sommets.extrude(toExtrude,4);
}

void Dessinateur::cubeDroitTexture(double x, double dx, double y, double dy, double z, double dz, GLuint texture, QColor couleur)
{
    sommets.setCurrentTexture(texture);
    for(int i = 0; i < 6; ++i) {
        for(int j = 0; j < 4; ++j)
            sommets.nouvelleTexCoord((j == 0 || j == 1) * dx, (j == 0 || j == 3) * dy);
    }
	return cubeDroit(x, dx, y, dy, z, dz, couleur);
}

void Dessinateur::cubeDroit(StructZone z, double altitude, double dz, QColor couleur)
{
	return cubeDroit(z.x.debut(), z.x.longueur(), z.y.debut(), z.y.longueur(), altitude, dz, couleur);
}

void Dessinateur::cubeDroitTexture(StructZone z, double altitude, double dz, GLuint texture, QColor couleur)
{
	return cubeDroitTexture(z.x.debut(), z.x.longueur(), z.y.debut(), z.y.longueur(), altitude, dz, texture, couleur);
}

void Dessinateur::cubeDroitCentre(double x, double dx, double y, double dy, double z, double dz, QColor coul)
{
	cubeDroit(x-dx/2, dx, y-dy/2, dy, z-dz/2, dz, coul);
}

void Dessinateur::cubePenche(Zone zoneLiee, double dz, QColor couleur)
{
    StructZone z = zoneLiee;

    int coins[4][2] = {
        { z.x.debut(), z.y.debut() },
        { z.x.debut(), z.y.fin() },
        { z.x.fin(), z.y.fin() },
        { z.x.fin(), z.y.debut() }
    };

    for(int i = 0; i < 4; i++)
    {
        sommets.nouvellePosition(coins[i][0], coins[i][1], zoneLiee.getVille()->getHauteurBase(coins[i][0], coins[i][1]));
        sommets.nouvelleNormale(0,0,-1);
        sommets.nouvelleCouleur(couleur);
    }

    double hauteurs[] = {0,0,dz};
	sommets.extrude(hauteurs, 4);
}


Repere::Repere()
{
    glPushMatrix();
}

Repere::~Repere()
{
    glPopMatrix();
}

Repere& Repere::translate(QVector3D t)
{
    glTranslated(t.x(), t.y(), t.z());
    return *this;
}

Repere& Repere::rotate(double angle, QVector3D axe)
{
    glRotated(Math::enDegres(angle), axe.x(), axe.y(), axe.z());
    return *this;
}

Repere::FonctionRotation Repere::FONCTIONS_ROTATION[3] = {&Math::Matrice::rotationX, &Math::Matrice::rotationY, &Math::Matrice::rotationZ};

Repere& Repere::rotate(double angle, int dimension)
{
    double matrice[16];
    FONCTIONS_ROTATION[dimension](matrice, angle);
    glMultMatrixd(matrice);
    return *this;
}

Repere& Repere::scale(double s)
{
    glScaled(s, s, s);
    return *this;
}

Repere& Repere::rotate(QVector3D e1, QVector3D e2, QVector3D e3)
{
    double matrice[16];
    Math::Matrice::changement(matrice, e1.x(), e1.y(), e1.z(), e2.x(), e2.y(), e2.z(), e3.x(), e3.y(), e3.z());
    glMultMatrixd(matrice);
    return *this;
}

Repere& Repere::rotateXY(QVector3D nouveauX, QVector3D nouveauY)
{
    rotate(nouveauX, nouveauY, QVector3D::crossProduct(nouveauX, nouveauY));
    return *this;
}

Repere& Repere::rotateXZ(QVector3D nouveauX, QVector3D nouveauZ)
{
    rotate(nouveauX, QVector3D::crossProduct(nouveauZ, nouveauX), nouveauZ);
    return *this;
}

Repere& Repere::rotateYZ(QVector3D nouveauY, QVector3D nouveauZ)
{
    rotate(QVector3D::crossProduct(nouveauY, nouveauZ), nouveauY, nouveauZ);
    return *this;
}
