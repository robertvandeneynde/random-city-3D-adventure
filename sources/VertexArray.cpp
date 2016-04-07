#include "VertexArray.h"

#include <algorithm>
#include <cassert>
#include <QDebug>

#include "fonctionsMathematiques.h"

namespace {
    inline char * BUFFER_OFFSET(size_t a) {
        return (char*)NULL + a;
    }
}

VertexArray::VertexArray()
{
    setModeAffichage(GL_TRIANGLES);
    utiliserBuffer(false);

    m_textures.push_back( InfoTexture(0,0) );
}

VertexArray::~VertexArray()
{

}

void VertexArray::nouvellePosition(QVector2D const& vec)
{
    nouvellePosition(vec.x(), vec.y());
}

void VertexArray::nouvellePosition(QVector3D const& vec)
{
    nouvellePosition(vec.x(), vec.y(), vec.z());
}

void VertexArray::nouvellePosition(QVector4D const& vec)
{
    nouvellePosition(vec.x(), vec.y(), vec.z(), vec.w());
}

void VertexArray::nouvellePosition(double x, double y, double z, double w)
{
    m_pos.push_back(x);
    m_pos.push_back(y);
    m_pos.push_back(z);
    m_pos.push_back(w);
}

void VertexArray::nouvellePosition(const double p[], size_t taille)
{
    assert(taille == 2 || taille == 3 || taille == 4);
    m_pos.insert(m_pos.end(), p, p + taille);
    if(taille <= 2)
        m_pos.push_back(0.0);
    if(taille <= 3)
        m_pos.push_back(1.0);
}
void VertexArray::nouvellePosition(std::vector<double> const& p)
{
    assert(p.size() == 2 || p.size() == 3 || p.size() == 4);
    m_pos.insert(m_pos.end(), p.begin(), p.end());
    if(p.size() <= 2)
        m_pos.push_back(0.0);
    if(p.size() <= 3)
        m_pos.push_back(1.0);
}

void VertexArray::nouvelleNormale(const QVector3D & vec)
{
    nouvelleNormale(vec.x(), vec.y(), vec.z());
}
void VertexArray::nouvelleNormale(double x, double y, double z)
{
    m_norm.push_back(x);
    m_norm.push_back(y);
    m_norm.push_back(z);
}
void VertexArray::nouvelleNormale(const double p[])
{
    m_norm.insert(m_norm.end(), p, p + N_SIZE);
}
void VertexArray::nouvelleNormale(std::vector<double> const& p)
{
    assert(p.size() == N_SIZE);
    m_norm.insert(m_norm.end(), p.begin(), p.end());
}

void VertexArray::nouvelleCouleur(const QColor & col)
{
    nouvelleCouleur(col.redF(), col.greenF(), col.blueF(), col.alphaF());
}

void VertexArray::nouvelleCouleur(float r, float g, float b, float a)
{
    m_coul.push_back(r);
    m_coul.push_back(g);
    m_coul.push_back(b);
    m_coul.push_back(a);
}
void VertexArray::nouvelleCouleur(const float c[], size_t taille)
{
    assert(taille == C_SIZE || taille == C_SIZE-1);
    m_coul.insert(m_coul.end(), c, c + taille);
    if(taille == C_SIZE-1)
        m_coul.push_back(1.0);
}
void VertexArray::nouvelleCouleur(std::vector<float> const& c)
{
    assert(c.size() == C_SIZE || c.size() == C_SIZE-1);
    m_coul.insert(m_coul.end(), c.begin(), c.end());
    if(c.size() == C_SIZE-1)
        m_coul.push_back(1.0);
}


void VertexArray::nouvelleTexCoord(QVector2D const& vec)
{
    nouvelleTexCoord(vec.x(), vec.y());
}

void VertexArray::nouvelleTexCoord(double x, double y)
{
    m_texc.push_back(x);
    m_texc.push_back(y);
}

void VertexArray::nouvelleTexCoord(const double p[])
{
    m_texc.insert(m_texc.end(), p, p + T_SIZE);
}

void VertexArray::nouvelleTexCoord(std::vector<double> const& p)
{
    assert(p.size() == T_SIZE);
    m_texc.insert(m_texc.end(), p.begin(), p.end());
}

void VertexArray::transformerDernierPoint(size_t n)
{
	transformerDernierPoint(m_matrices.top(), n);
}
void VertexArray::transformerDernierePosition(size_t n)
{
	transformerDernierePosition(m_matrices.top(), n);
}
void VertexArray::transformerDerniereNormale(size_t n)
{
	transformerDerniereNormale(m_matrices.top(), n);
}

VertexArray::Handle VertexArray::handleDernier(size_t n, size_t dec)
{
	size_t a = length() - (n + dec);
	return Handle(this, a, n);
}

void VertexArray::Handle::pos(double *& a, double *& b)
{
	a = & self->m_pos[from * VertexArray::V_SIZE];
	b = a + count * VertexArray::V_SIZE;
}

void VertexArray::Handle::coul(float *& a, float *& b)
{
	a = & self->m_coul[from * VertexArray::C_SIZE];
	b = a + count * VertexArray::C_SIZE;
}

void VertexArray::Handle::norm(double *& a, double *& b)
{
	a = & self->m_norm[from * VertexArray::N_SIZE];
	b = a + count * VertexArray::N_SIZE;
}

void VertexArray::Handle::texc(double *& a, double *& b)
{
	a = & self->m_texc[from * VertexArray::T_SIZE];
	b = a + count * VertexArray::T_SIZE;
}

size_t VertexArray::Handle::size()
{
	return count;
}

void VertexArray::Handle::setCouleur(QColor c)
{
	float* beg, *end;
	coul(beg,end);
	for(float* a = beg ; a < end; a += 4)
	{
		a[0] = c.redF();
		a[1] = c.greenF();
		a[2] = c.blueF();
		a[3] = c.alphaF();
	}
	self->m_buffer.destroy();
	/*
	//Experimental...
	if(self->m_buffer.isCreated() && self->m_utiliserBuffer)
	{
		self->m_buffer.create();
		self->m_buffer.bind();
		self->writeBufferPartial(from * VertexArray::C_SIZE, count * VertexArray::C_SIZE * sizeof(float), COUL);
		self->m_buffer.release();
	}
	*/
}

void VertexArray::Handle::setPosition(QVector3D vec)
{
	double *a, *b;
	for(pos(a,b) ; a < b; a += 4)
	{
		a[0] = vec.x();
		a[1] = vec.y();
		a[2] = vec.z();
	}
	self->m_buffer.destroy();
}

QVector3D VertexArray::Handle::position()
{
	double *a, *b;
	pos(a,b);
	return QVector3D(a[0], a[1], a[2]);
}

QColor VertexArray::Handle::couleur()
{
	float *a, *b;
	coul(a,b);
	return QColor::fromRgbF(a[0], a[1], a[2], a[3]);
}

std::vector<QVector3D> VertexArray::Handle::positions()
{
	std::vector<QVector3D> vec(size());
	double *a, *b;
	std::vector<QVector3D>::iterator it = vec.begin();
	for(pos(a,b) ; a < b; a += 4)
		*it++ = QVector3D(a[0], a[1], a[2]);
	return vec;
}

std::vector<QColor> VertexArray::Handle::couleurs()
{
	std::vector<QColor> vec(size());
	float *a, *b;
	std::vector<QColor>::iterator it = vec.begin();
	for(coul(a,b) ; a < b; a += 4)
		*it++ = QColor::fromRgbF(a[0], a[1], a[2], a[3]);
	return vec;
}

void VertexArray::Handle::move(QVector3D vec)
{
	double *a, *b;
	pos(a,b);
	for( ; a < b; a += 4)
	{
		a[0] += vec.x();
		a[1] += vec.y();
		a[2] += vec.z();
	}
	self->m_buffer.destroy();
}

void VertexArray::Handle::transformerPosition(const double matrice[16])
{
	double *a, *b;
	for(pos(a,b); a < b; a += 4)
		Math::Matrice::multiplication(matrice, a, 4);
}
void VertexArray::Handle::transformerNormale(const double matrice[16])
{
	double *a, *b;
	for(norm(a,b); a < b; a += 3)
		Math::Matrice::multiplication(matrice, a, 3);
}

VertexArray::Handle VertexArray::HandleBuilder::get()
{
	return self->handleDernier(self->length() - debut);
}

void VertexArray::HandleBuilder::reset()
{
	debut = self->length();
}

VertexArray::Handle VertexArray::HandleBuilder::getReset()
{
	Handle h = get();
	reset();
	return h;
}

Math::Matrice::Pile& VertexArray::pileMatrice()
{
	return m_matrices;
}

double * VertexArray::dernierePosition(size_t n)
{
    return &m_pos[m_pos.size() - n*V_SIZE];
}

double * VertexArray::derniereNormale(size_t n)
{
    return &m_norm[m_norm.size() - n*N_SIZE];
}

float * VertexArray::derniereCouleur(size_t n)
{
    return &m_coul[m_coul.size() - n*C_SIZE];
}



void VertexArray::equilibrerCouleurs()
{
    size_t val = std::min(m_pos.size() / V_SIZE, m_norm.size() / N_SIZE) * C_SIZE;
    if(m_coul.size() < val)
    {
        size_t ancien_size = m_coul.size();
        m_coul.resize(val);
        for(size_t i = ancien_size; i < val; i += C_SIZE)
            for(size_t j = 0 ; j < C_SIZE ; j++)
                m_coul[i+j] = m_coul[i+j - C_SIZE];
    }
}

void VertexArray::extrude(const double direction[], size_t n)
{
    extrude(std::vector<double>(direction, direction+3), n);
}

void VertexArray::extrude(std::vector<double> direction, size_t n)
{
    direction.resize(V_SIZE);
    direction.back() = 0;
    typedef std::vector<double>::iterator It;
    //qDebug() << "Debut extrude";
    size_t avantPos = m_pos.size();
    size_t avantNor = m_norm.size();
    m_pos.reserve(m_pos.size() + 5 * n * V_SIZE);
    m_norm.reserve(m_norm.size() + 5 * n * N_SIZE);

    //qDebug() << "1";

    //Ajout des n points de face opposee a celle de la derniere ajoutee
    It premier_bas = m_pos.end() - n * V_SIZE;
    It premier_haut = m_pos.end();
    for(size_t i = 0 ; i < n ; i++) {
        for(size_t j = 0 ; j < V_SIZE ; j++) {
            m_pos.push_back(*(premier_bas + i*V_SIZE + j) + direction[j]); //n points translated
        }   
    }
    /*for(int i = 0 ; i < m_pos.size() ; i++)
        qDebug() << "2 : m_pos[" << i << "] = " << m_pos[i];*/
    It dernier_haut = m_pos.end() - V_SIZE;

    //qDebug() << "2";

    //Cette face a une normale inversee
    It premier_haut_norm = m_norm.end() - N_SIZE;
    for(size_t i = 0 ; i < n ; i++)
        for(size_t j = 0 ; j < N_SIZE ; j++)
            m_norm.push_back(- *(premier_haut_norm + j)); //n normales renversees

    //qDebug() << "3 : taille = " << m_pos.size();

    //On cree les n faces laterales qui sont des quads
    It dec = m_pos.end() - (n*2) * V_SIZE;
    for(size_t i = 0 ; i < n ; i++) //n faces
    {
        size_t points[4] = {
            i, //Actuel
            i + n, //Translated
			(i+1)%n + n, //Translated Suivant
            (i+1)%n //Translated
        };
        /*for(int j = 0 ; j < 4 ; i++)
            qDebug() << "Points : m_pos[" << (dec + points[j] * V_SIZE) - m_pos.begin() << "] = " << *(dec + points[j] * V_SIZE);*/

        //qDebug() << "A";
        for(size_t j = 0 ; j < 4 ; j++)
            m_pos.insert(m_pos.end(), dec + points[j] * V_SIZE, dec + (points[j]+1) * V_SIZE);

        /*for(size_t j = m_pos.size() - 4*V_SIZE; j < m_pos.size(); j+=V_SIZE)
            qDebug() << "m_pos[" << j << "] = " << m_pos[j] << m_pos[j+1] << m_pos[j+2] << m_pos[j+3];*/

        //qDebug() << "B";
        std::vector<double> normale = calculerNormale();
        //qDebug() << "C";
        for(size_t k = 0 ; k < 4 ; k++)
            nouvelleNormale(normale);
        //qDebug() << "D";
    }

    //qDebug() << "4";

    assert(m_pos.size() == avantPos + 5 * n * V_SIZE);
    assert(m_norm.size() == avantNor + 5 * n * N_SIZE);

    //La face translated est definie a l'envers
    for(size_t i = 0 ; i < n/2 ; i++)
        for(size_t j = 0 ; j < V_SIZE ; j++)
            std::iter_swap(premier_haut + i * V_SIZE + j, dernier_haut - i * V_SIZE + j);
    //qDebug() << "5";
    equilibrerCouleurs();

    /*for(size_t i = 0 ; i < m_pos.size() ; i += V_SIZE)
        qDebug() << m_pos[i] << m_pos[i+1] << m_pos[i+2] << m_pos[i+3];
    qDebug() << "";
    for(size_t i = 0 ; i < m_norm.size() ; i += N_SIZE)
        qDebug() << m_norm[i] << m_norm[i+1] << m_norm[i+2];
    qDebug() << "";
    for(size_t i = 0 ; i < m_coul.size() ; i += C_SIZE)
        qDebug() << m_coul[i] << m_coul[i+1] << m_coul[i+2] << m_coul[i+3];
    qDebug() << "";*/
    //qDebug() << "6";
}
std::vector<double> VertexArray::calculerNormale() const
{
    //Exemple m_pos = [... 4,6,8,1, 3,7,8,1, 2,6,8,1]
    std::vector<double> v1(N_SIZE), v2(N_SIZE), res(N_SIZE);

    //qDebug() << "N1";
    size_t dernier[3];
    for(size_t i = 0 ; i < 3; i++)
        dernier[i] = m_pos.size() - (i+1)*V_SIZE;
    //dernier = {indice du 2, indice du 3, indice du 4}

    for(size_t j = 0 ; j < N_SIZE ; j++)
    {
        v1[j] = m_pos[dernier[1] + j] - m_pos[dernier[2] + j];
        v2[j] = m_pos[dernier[0] + j] - m_pos[dernier[1] + j];
    }

    /*qDebug() << "V1 = " << v1[0] << v1[1] << v1[2];
    qDebug() << "V2 = " << v2[0] << v2[1] << v2[2];*/
    //qDebug() << "N3";
    //v1 = [-1,1,0], v2 = [-1,-1,0]
    res[0] = v1[1] * v2[2] - v1[2] * v2[1];
    res[1] = v1[2] * v2[0] - v1[0] * v2[2];
    res[2] = v1[0] * v2[1] - v1[1] * v2[0];
    //res = v1 cross v2 = [0,0,2]
    return res;
}

void VertexArray::noCurrentTexture()
{
    texturesEquilibrees();
    if(m_textures.back().def)
		m_textures.push_back( InfoTexture(length(), 0) );
}

void VertexArray::setCurrentTexture(GLuint n)
{
    texturesEquilibrees();
    if(n != m_textures.back().id)
        m_textures.push_back( InfoTexture(n, length(), 0) );
}

void VertexArray::texturesEquilibrees()
{
    size_t val = std::min(m_pos.size() / V_SIZE, m_norm.size() / N_SIZE) * T_SIZE;
    if(m_texc.size() < val)
    {
        if(m_textures.back().def)
        {
            m_textures.back().count = m_texc.size() / T_SIZE - m_textures.back().from;
            m_textures.push_back( InfoTexture(m_texc.size() / T_SIZE, 0) );
        }
        m_texc.resize(val);
    }
    m_textures.back().count = length() - m_textures.back().from;
}

size_t VertexArray::length() const
{
    assert(m_pos.size() / V_SIZE == m_coul.size() / C_SIZE && m_coul.size() / C_SIZE == m_norm.size() / N_SIZE);
    assert(m_pos.size() % V_SIZE == 0 && m_coul.size() % C_SIZE == 0 && m_norm.size() % N_SIZE == 0 && m_texc.size() % T_SIZE == 0);
    return m_pos.size() / V_SIZE;
}

size_t VertexArray::weight() const
{
    return oct(m_pos) + oct(m_norm) + oct(m_coul) + oct(m_texc);
}

void VertexArray::utiliserBuffer(bool val)
{
	m_utiliserBuffer = val;
}

void VertexArray::initBuffer()
{
	m_buffer.destroy();
	m_buffer = QGLBuffer();

	m_buffer.create();
	m_buffer.bind();

    allouerBuffer();

	m_buffer.release();
}

void VertexArray::copierSansCouleurs(VertexArray &autre)
{
    autre.m_pos = m_pos;
    autre.m_norm = m_norm;
    autre.m_coul.assign(m_coul.size(), 0);
    for(size_t i = 3; i < m_coul.size(); i += C_SIZE)
        autre.m_coul[i] = 1;
}

void VertexArray::setModeAffichage(GLenum mode)
{
    m_modeAffichage = mode;
}
void VertexArray::dessiner()
{
    /* Actions a faire a la fin de la creation des tableaux */
    texturesEquilibrees();

    if(m_utiliserBuffer && !m_buffer.isCreated()) {
        initBuffer();
    }
	/* fin */

    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if(! m_utiliserBuffer)
        setPointers();
    else
	{
		m_buffer.bind();
        setVBOPointers();
    }

	for(InfoTexture& info : m_textures)
		info.draw(m_modeAffichage);

	if(m_utiliserBuffer)
		m_buffer.release();

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void VertexArray::InfoTexture::draw(GLenum modeAffichage)
{
	if(def) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id);
	} else {
        glDisable(GL_TEXTURE_2D);
	}
    glDrawArrays(modeAffichage, from, count);
}

void VertexArray::writeBufferPartial(size_t indice, size_t bytes, TypeDonnee type) const
{
	//Experimental...
	size_t w = 0;
	if(type == POSITION)
		m_buffer.write(w, &m_pos[indice], bytes);
	else {
		w += oct(m_pos);
		if(type == NORMAL)
			m_buffer.write(w, &m_norm[indice], bytes);
		else {
			w += oct(m_norm);
			if(type == COUL)
				m_buffer.write(w, &m_coul[indice], bytes);
			else {
				w += oct(m_coul);
				m_buffer.write(w, &m_texc[indice], bytes);
			}
		}
	}
}

void VertexArray::allouerBuffer() const
{
	m_buffer.allocate( weight() );
	size_t w = 0;
	m_buffer.write(w += 0, &m_pos[0], oct(m_pos));
	m_buffer.write(w += oct(m_pos), &m_norm[0], oct(m_norm));
	m_buffer.write(w += oct(m_norm), &m_coul[0], oct(m_coul));
	m_buffer.write(w += oct(m_coul), &m_texc[0], oct(m_texc));
}

void VertexArray::setVBOPointers() const
{
    size_t w = 0;
    glVertexPointer(V_SIZE, GL_DOUBLE, 0, BUFFER_OFFSET(w));
    glNormalPointer(GL_DOUBLE, 0, BUFFER_OFFSET(w += oct(m_pos)));
    glColorPointer(C_SIZE, GL_FLOAT, 0, BUFFER_OFFSET(w += oct(m_norm)));
    glTexCoordPointer(T_SIZE, GL_DOUBLE, 0, BUFFER_OFFSET(w += oct(m_coul)));
}
void VertexArray::setPointers() const
{
	glVertexPointer(V_SIZE, GL_DOUBLE, 0, &m_pos[0]);
    glNormalPointer(GL_DOUBLE, 0, &m_norm[0]);
    glColorPointer(C_SIZE, GL_FLOAT, 0, &m_coul[0]);
    glTexCoordPointer(T_SIZE, GL_DOUBLE, 0, &m_texc[0]);
}
