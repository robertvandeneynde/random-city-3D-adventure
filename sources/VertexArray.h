#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <vector>
#include <list>

#include <QGLWidget>
#include <QGLBuffer>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QColor>

#include "fonctionsMathematiques.h"

class VertexIndices;

class VertexArray
{
public :
	//Handle sur un vertex ou ensemble (consécutifs) de vertex
	//Attention, peut provoquer des segfault si le vertex n'est pas là
	class Handle
	{
		public :
			Handle() { from = count = 0; self = 0; }
			bool valid() { return self && count; }
			void transformerPosition(const double matrice[16]);
			void transformerNormale(const double matrice[16]);

			void setCouleur(QColor);
			void setPosition(QVector3D);
			void move(QVector3D);

			QVector3D position();
			QColor couleur();

			std::vector<QVector3D> positions();
			std::vector<QColor> couleurs();

			size_t size();
		private :
			void pos(double*&, double*&);
			void norm(double*&, double*&);
			void coul(float *&, float *&);
			void texc(double*&, double*&);
		private :
			Handle(VertexArray* self_, size_t from_, size_t count_) : self(self_), from(from_), count(count_) {}
			friend class VertexArray;
			VertexArray* self;
			size_t from;
			size_t count;
	};

	class HandleBuilder
	{
		public :
			HandleBuilder(VertexArray* self_) : self(self_) { reset(); }
			Handle get();
			Handle operator()() { return get(); }
			void reset();
			Handle getReset();
		private :
			VertexArray* self;
			size_t debut;

	};

public:
    VertexArray();
    ~VertexArray();
	
	//Push_back
    void nouvellePosition(QVector2D const&);
    void nouvellePosition(QVector3D const&);
    void nouvellePosition(QVector4D const&);
    void nouvellePosition(double x, double y, double z = 0, double w  = 1.0);
    void nouvellePosition(const double p[], size_t taille = 3); //Taille 2, 3 ou 4
    void nouvellePosition(std::vector<double> const&); //Taille 2, 3 ou 4

    void nouvelleNormale(QVector3D const&);
    void nouvelleNormale(double x, double y, double );
    void nouvelleNormale(const double p[3]);
    void nouvelleNormale(std::vector<double> const&); //Taille 3

    void nouvelleCouleur(QColor const&);
    void nouvelleCouleur(float r, float g, float b, float a = 1.0);
    void nouvelleCouleur(const float c[], size_t taille = 3); //Taille 3 ou 4
    void nouvelleCouleur(std::vector<float> const&); //Taille 3 ou 4

    void nouvelleTexCoord(QVector2D const&);
    void nouvelleTexCoord(double x, double y);
    void nouvelleTexCoord(const double p[2]); //Taille 2
    void nouvelleTexCoord(std::vector<double> const&); //Taille 2

    void noCurrentTexture();
    void setCurrentTexture(GLuint n);

	Math::Matrice::Pile& pileMatrice();

	//Obsolete, use Handle !

	void transformerDernierePosition(const double matrice[16], size_t n = 1) { handleDernier(n).transformerPosition(matrice); }
	void transformerDerniereNormale(const double matrice[16], size_t n = 1) { handleDernier(n).transformerPosition(matrice); }
	void transformerDernierPoint(const double matrice[16], size_t n = 1) { Handle h = handleDernier(n); h.transformerPosition(matrice); h.transformerNormale(matrice);}

    //Transform with current matrix
	void transformerDernierePosition(size_t n = 1);
    void transformerDerniereNormale(size_t n = 1);
    void transformerDernierPoint(size_t n = 1);

    double * dernierePosition(size_t n = 1);
    double * derniereNormale(size_t n = 1);
    float * derniereCouleur(size_t n = 1);

	// end obsolete

	Handle handleDernier(size_t nombre = 1, size_t dec = 0); //nombre de vertex / décalage depuis la fin
	HandleBuilder handleBuilder() { return HandleBuilder(this); }

    void extrude(std::vector<double> direction, size_t n = 4); //n >= 3 //Cree 5*n nouveaux points et 5*n nouvelles normales
    void extrude(const double direction[], size_t n = 4); //n >= 3 //Cree 5*n nouveaux points et 5*n nouvelles normales
    void equilibrerCouleurs();
    std::vector<double> calculerNormale() const; //Utilisant les 3 derniers points. Le sens est déterminé par la règle de la main droite dans repère OpenGL

    void copierSansCouleurs(VertexArray & autre);

    void setModeAffichage(GLenum);
    void utiliserBuffer(bool = true);
    void dessiner();

    size_t length() const;
    size_t weight() const;
private :

	enum TypeDonnee { POSITION, COUL, NORMAL, TEX };

    void initBuffer();
    void allouerBuffer() const;
	void writeBufferPartial(size_t indice, size_t count, TypeDonnee type) const;
    void setVBOPointers() const;
    void setPointers() const;
    void texturesEquilibrees();

    template <typename T>
    static size_t oct(std::vector<T> const& v) {
        return v.size() * sizeof(T);
    }

    static const size_t V_SIZE = 4;
    static const size_t C_SIZE = 4;
    static const size_t N_SIZE = 3;
    static const size_t T_SIZE = 2;
private:
	std::vector<double> m_pos;
    std::vector<float> m_coul;
    std::vector<double> m_norm;
    std::vector<double> m_texc;

    struct InfoTexture
    {
		InfoTexture(GLuint id_, size_t from_, size_t count_) : id(id_), def(true), from(from_), count(count_) {}
        InfoTexture(size_t from_, size_t count_) : def(false), from(from_), count(count_) {}

        void draw(GLenum modeAffichage);

        GLuint id;
        bool def;
		size_t from, count;
    };
    std::list<InfoTexture> m_textures;

	mutable QGLBuffer m_buffer;
    bool m_utiliserBuffer;
    GLenum m_modeAffichage;

	Math::Matrice::Pile m_matrices;
};

#endif // VERTEXARRAY_H
