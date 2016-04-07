#ifndef FONCTIONSMATHEMATIQUES_H
#define FONCTIONSMATHEMATIQUES_H

#include <cmath>
#include <vector>
#include <QVector3D>
#include <QPoint>

namespace Math
{
const double PI = acos(-1.0);
const double PRECISION = 1e-5;

double randUniform();
double randNormal(double moyenne = 0.0, double ecart_type = 1.0);

double enRadians(double);
double enDegres(double);
bool egalite(double a, double b, double precision = PRECISION);

QPoint pointFromDirection(int direction, int paralele_a_direction, int perpendiculaire_a_direction);

namespace Matrice
{
void identity(double matrice[16]);
void changement(double matrice[16], double e1x, double e1y, double e1z, double e2x, double e2y, double e2z, double e3x, double e3y, double e3z, double tx=0, double ty=0, double tz=0);
void rotationX(double matrice[16], double angle);
void rotationY(double matrice[16], double angle);
void rotationZ(double matrice[16], double angle);
void scale(double matrice[16], double sx, double sy, double sz);
void scale(double matrice[16], double s);
void translation(double matrice[16], double x, double y, double z);
void multiplication(const double matrice[16], double const vec[], double res[], size_t taille = 4); // res = matrice * vec
void multiplicationMatrice(const double matrice1[16], const double matrice2[16], double res[16], size_t taille = 4); // res = m1 * m2
void multiplication(const double matrice[16], double point[], size_t taille = 4); // point = matrice * point
void multiplicationMatrice(double matrice1[16], const double matrice2[16], size_t taille = 4); // m1 = m2 * m1

class Multiplieur
{
	public :
		Multiplieur();
		Multiplieur(const double[16]);
		Multiplieur(Multiplieur const&);
		Multiplieur& operator=(Multiplieur const&);
		Multiplieur& operator*=(const double[16]);
		explicit operator double*() { return actuelle(); }
		explicit operator const double*() const { return actuelle(); }
		double *actuelle();
		const double *actuelle() const;
	private :
		double donnees[2][16];
		short num;
};

class Pile
{
	public:
		struct Nouveau
		{
			Nouveau(Pile& p_) : p(p_) { p.push(); }
			~Nouveau() { p.pop(); }
			Pile& p;
		};
	public:
		Pile();
		void identity();
		void multiplier(const double matrice[16]);
		void push();
		void pop();
		double* top();
		const double* top() const;
	private:
		std::vector< std::vector<double> > m_matrices;
};
}
}

#endif // FONCTIONSMATHEMATIQUES_H
