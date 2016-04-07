#include "fonctionsMathematiques.h"

#include <cstdlib>
#include <vector>
#include <cmath>

namespace Math
{

double randUniform()
{
    return (double)rand() / RAND_MAX;
}
double randNormal(double moyenne, double ecart_type)
{
    double u1 = randUniform();
    double u2 = randUniform();
    return moyenne + ecart_type * sqrt(-2.0 * log(u1)) * cos(2 * PI * u2);
}

double enRadians(double v) {
    return v * PI/180;
}
double enDegres(double v) {
    return v * 180/PI;
}

bool egalite(double a, double b, double precision)
{
    return abs(a - b) < precision;
}

QPoint pointFromDirection(int direction, int paral, int perp)
{
    return direction == 0 ? QPoint(paral, perp) : QPoint(perp, paral);
}

namespace Matrice
{
void identity(double matrice[])
{
	for(int i = 0 ; i < 16 ; i++)
		matrice[i] = 0;
	for(int i = 0 ; i < 4; i++)
        matrice[i*4 + i] = 1;
}

/*inline */void changement(double matrice[16], double e1x, double e1y, double e1z, double e2x, double e2y, double e2z, double e3x, double e3y, double e3z, double tx, double ty, double tz)
{
    matrice[0] = e1x;
    matrice[1] = e1y;
    matrice[2] = e1z;
    matrice[3] = 0;

    matrice[4] = e2x;
    matrice[5] = e2y;
    matrice[6] = e2z;
    matrice[7] = 0;

    matrice[8] = e3x;
    matrice[9] = e3y;
    matrice[10] = e3z;
    matrice[11] = 0;

    matrice[12] = tx;
    matrice[13] = ty;
    matrice[14] = tz;
    matrice[15] = 1;
}

void rotationX(double matrice[], double angle)
{
    const double c = cos(angle), s = sin(angle);
    changement(matrice, 1,0,0, 0,c,s, 0,-s,c);
}

void rotationY(double matrice[], double angle)
{
    const double c = cos(angle), s = sin(angle);
    changement(matrice, c,0,-s, 0,1,0, s,0,c);
}

void rotationZ(double matrice[16], double angle)
{
    const double c = cos(angle), s = sin(angle);
    changement(matrice, c,s,0, -s,c,0, 0,0,1);
}

void scale(double matrice[16], double sx, double sy, double sz)
{
    changement(matrice, sx,0,0, 0,sy,0, 0,0,sz);
}

void scale(double matrice[16], double s)
{
    changement(matrice, s,0,0, 0,s,0, 0,0,s);
}

void translation(double matrice[16], double x, double y, double z)
{
    changement(matrice, 1,0,0, 0,1,0, 0,0,1, x,y,z);
}

void multiplication(const double matrice[], const double vec[], double res[], size_t taille)
{
    for(size_t i = 0 ; i < taille ; i++)
    {
        res[i] = 0;
        for(size_t j = 0 ; j < taille ; j++)
            res[i] += matrice[i + 4 * j] * vec[j];
    }
}
void multiplicationMatrice(const double matrice1[], const double matrice2[], double res[], size_t taille)
{
    for(size_t i = 0 ; i < taille ; i++)
    {
        for(size_t j = 0 ; j < taille ; j++)
        {
            double & cible = res[i + j * 4];
            cible = 0;
            for(size_t k = 0 ; k < taille ; k++)
                cible += matrice1[i + k * 4] * matrice2[k + j * 4];
        }
    }
}
void multiplication(const double matrice[], double point[], size_t taille)
{
    std::vector<double> copie(point, point + taille);
    multiplication(matrice, &copie[0], point, taille);
}
void multiplicationMatrice(double matrice1[], const double matrice2[], size_t taille)
{
	double ancienMatrice1[16];
	memcpy(ancienMatrice1, matrice1, 16 * sizeof(double));
	multiplicationMatrice(ancienMatrice1, matrice2, matrice1, taille);
}

Multiplieur::Multiplieur()
{
	num = 0;
	identity(donnees[num]);
}

Multiplieur::Multiplieur(const double matrice[])
{
	num = 0;
	memcpy(this, matrice, 16 * sizeof(double));
}

Multiplieur::Multiplieur(const Multiplieur & autre)
{
	num = 0;
	memcpy(this, &autre, 16 * sizeof(double));
}

Multiplieur& Multiplieur::operator =(const Multiplieur & autre)
{
	memcpy(this, &autre, 16 * sizeof(double));
	return *this;
}

Multiplieur& Multiplieur::operator *=(const double mat[16])
{
	multiplicationMatrice(mat, donnees[num], donnees[!num]);
	num = !num;
	return *this;
}

double* Multiplieur::actuelle()
{
	return donnees[num];
}

const double* Multiplieur::actuelle() const
{
	return donnees[num];
}

Pile::Pile()
	: m_matrices(1, std::vector<double>(16))
{
	identity();
}

void Pile::identity()
{
	Math::Matrice::identity( top() );
}

void Pile::multiplier(const double matrice[])
{
	Math::Matrice::multiplicationMatrice(top(), matrice);
}

double* Pile::top()
{
	return & m_matrices.back()[0];
}

const double* Pile::top() const
{
	return & m_matrices.back()[0];
}

void Pile::push()
{
	m_matrices.push_back( m_matrices.back() );
}

void Pile::pop()
{
	m_matrices.pop_back();
}

}

}
