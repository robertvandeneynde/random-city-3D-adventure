#include "fonctionsGl.h"

void glBlocCreerSommets(VertexArray & sommets ,double x, double y, double dx, double dy, double hauteur)
{
    glBlocCreerSommets(sommets, x, y, 0, dx, dy, hauteur);
}

void glBlocCreerSommets(VertexArray & sommets ,double x, double y, double z, double dx, double dy, double hauteur)
{
    static const int base[4][2] = {
        {0,0},
        {1,0},
        {1,1},
        {0,1}
    };
    static const int faces[6][4] = {
        {3,2,1,0},
        {4,5,6,7},
        {0,1,5,4},
        {2,3,7,6},
        {0,4,7,3},
        {1,2,6,5}
    };
    static const double normales[6][3] = {
        { 0, 0,-1},
        { 0, 0,+1},
        { 0,-1, 0},
        { 0,+1, 0},
        {-1, 0, 0},
        {+1, 0, 0}
    };

    //Inverser une normale va changer noir/blanc
    //Inverser l'ordre de définition des faces va changer son facteur de visiblité : visible de l'intérieur/extérieur.
    double points[8][3];
    int n = 0;
    for(double h = z ; h < z + hauteur*1.42; h += hauteur)
    {
        for(int i = 0 ; i < 4 ; i++)
        {
            points[n][0] = x + dx * base[i][0];
            points[n][1] = y + dy * base[i][1];
            points[n][2] = h;
            n++;
        }
    }

    float couleurs[4];
    glGetFloatv(GL_CURRENT_COLOR, couleurs);

    double matrice[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, matrice);

    for(int i = 0 ; i < 6 ; i++)
    {
        for(int j = 0 ; j < 4 ; j++)
        {
            sommets.nouvellePosition(points[ faces[i][j] ]);
            sommets.nouvelleNormale(normales[i]);
            sommets.nouvelleCouleur(couleurs, 4);

            sommets.transformerDernierPoint(matrice);
        }
    }
}

void glDrawRepere(int echelle)
{
    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex3d(0,0,0);
        glVertex3d(echelle,0,0);

        glColor3f(0,1,0);
        glVertex3d(0,0,0);
        glVertex3d(0,echelle,0);

        glColor3f(0,0,1);
        glVertex3d(0,0,0);
        glVertex3d(0,0,echelle);
    glEnd();
}
