#include "Helico.h"
#include "Ville.h"

#include <QDebug>

#include "fonctionsMathematiques.h"

#include "Dessinateur.h"

const double Helico::ACCELERATION = 4; //Unités par seconde par seconde
const double Helico::ACCELERATION_HAUT = 6; //Unités par seconde par seconde
const double Helico::GRAVITE = 3; //Unités par seconde par seconde
const double Helico::POURCENTAGE_FROTTEMENT = 0.96; //Pourcentage par frame
const double Helico::VITESSE_ROTATION = Math::enRadians(45); //Radians par seconde
const double Helico::DELTA_Z_MAX = 0;
const double Helico::SCALE = 0.22;

Helico::Helico(Ville * ville, Chargement * chargement)
    : Vehicule(ville)
    , m_monObj("Ressources/helico.obj", "Ressources/")
    , m_oscCamera(0.025, 0.300)
{
    if(m_monObj.erreurOuvertureFichier())
        if(chargement)
            chargement->afficherTexte("Erreur ouverture helico.obj");

    m_monObj.utiliserBuffer();

    m_helices[0].donnees = m_monObj.enleverAffichage("RotorPrincipal");
    m_helices[1].donnees = m_monObj.enleverAffichage("AntiRotor");
    m_helices[0].coord = QVector3D(1.800, 0.000, 2.800);
    m_helices[1].coord = QVector3D(8.636, -0.800, 1.160);
    m_helices[0].axeRotation = QVector3D(0,0,1);
    m_helices[1].axeRotation = QVector3D(0,-1,0);

    for(int i = 0 ; i < 2; i++)
        m_helices[i].translater();
	
	// hélice
    for(double i = -7; i < 6.9; i+= 7/2.0)
           for(double j = -7; j < 6.9; j+= 7/2.0)
               m_heliceCoord.push_back(QVector3D(i, j, 2.8));

    // base
    for(double i = -1; i < 0.9; i+= 0.5)
           for(double j = -3.3; j < 1.2; j+= 4.6/4.0)
               m_baseCoord.push_back(QVector3D(-j, i, -1.5));
}
Helico::~Helico()
{

}

Helico::Helice::Helice()
{
    setVitesseMax(27);
    angle = 0;
}

void Helico::dessiner()
{
    Repere R;
    R.translate(position).rotate(angle + Math::PI, Repere::Z).scale(SCALE);

    m_monObj.dessiner();
    for(int i = 0 ; i < 2 ; i++)
        m_helices[i].dessiner();
}
void Helico::enterFrame()
{
    Vehicule::enterFrame();
	deplacer();

    m_oscCamera.valeurCible = angle;
    m_oscCamera.enterFrame();
    for(int i = 0 ; i < 2 ; i++)
    {
        m_helices[i].appliquerVitesse();
        m_helices[i].augmenterVitesse(m_baseComplete ? -0.5 : 0.5);
    }
}


void Helico::deplacer()
{
	// gravite
	m_vitesse += GRAVITE * Parametres::DT_2 * QVector3D(0,0,-1); //u/fr = u/s² * s²/fr² * 1fr

    QVector3D nouvellePosition = position + m_vitesse;
    double nouvelAngle = angle + m_vitesseAngle;

    // collisions
    calculerCoord(nouvellePosition, nouvelAngle);

    if(! deplacementCorrect(nouvellePosition, nouvelAngle))
    {
        m_vitesse *= 0;
        m_vitesseAngle = 0;
    }

    // deplacement
    position = position + m_vitesse;
    angle = angle + m_vitesseAngle;

    //Frottement
    m_vitesse *= POURCENTAGE_FROTTEMENT;
    m_vitesseAngle = 0;
}

bool Helico::deplacementCorrect(QVector3D nouvellePosition, double nouveauAngle)
{
    bool correct = true;
    m_baseComplete = true;
    for(size_t i = 0; i < m_baseCoordAbsolues.size(); i++)
        if(m_baseCoordAbsolues[i].z() < m_ville->hauteurEffective(m_baseCoordAbsolues[i].x(), m_baseCoordAbsolues[i].y()))
            correct = false;
        else
            m_baseComplete = false;

    bool deplacementVersLeHaut = m_vitesseAngle == 0 && Math::egalite(m_vitesse.x(),0) && Math::egalite(m_vitesse.y(),0) && m_vitesse.z() > 0;
    if(deplacementVersLeHaut)
        correct = true;
    else
        for(size_t i = 0; correct && i < m_heliceCoordAbsolues.size(); i++)
            if(m_heliceCoordAbsolues[i].z() < m_ville->hauteurEffective(m_heliceCoordAbsolues[i].x(), m_heliceCoordAbsolues[i].y()))
                correct = false;

    return correct;
}

void Helico::calculerCoord(QVector3D laPosition, double lAngle)
{
    calculerCoord(laPosition, m_baseCoordAbsolues, lAngle, m_baseCoord);
    calculerCoord(laPosition, m_heliceCoordAbsolues, lAngle, m_heliceCoord);
}

void Helico::calculerCoord(QVector3D laPosition, std::vector<QVector3D> & coordAbsolues , double lAngle, std::vector<QVector3D> const & coord)
{
    coordAbsolues.clear();
    double matriceZ[16], matriceS[16];
    Math::Matrice::rotationZ(matriceZ, lAngle + Math::PI);
    Math::Matrice::scale(matriceS, SCALE);
    for(size_t i = 0; i < coord.size(); i++)
    {
        double rel[3] = { coord[i].x(), coord[i].y(), coord[i].z()};

        Math::Matrice::multiplication(matriceZ, rel, 3);
        Math::Matrice::multiplication(matriceS, rel, 3);

        coordAbsolues.push_back(laPosition + QVector3D(rel[0],rel[1],rel[2]));
    }
}

void Helico::placerCamera(CameraFreeFly * camera)
{
    double angleAUtiliser = m_oscCamera.valeur;
    double recul = 4.0, hauteur = 3.0, avancement = hauteur * std::tan(Math::enRadians(60)) - recul;

    QVector3D direction(cos(angleAUtiliser), sin(angleAUtiliser), 0);
    QVector3D posCamera = position - recul * direction + QVector3D(0, 0, hauteur);
    camera->setPosition(posCamera);
    camera->regarderVers(position + avancement * direction);

    camera->avancer(0.2);
}

void Helico::gauche()
{
	m_vitesseAngle = VITESSE_ROTATION * Parametres::DT; // rad/fr = rad/s * s/fr
}

void Helico::droite()
{
	m_vitesseAngle = -VITESSE_ROTATION * Parametres::DT; // rad/fr = rad/s * s/fr
}

void Helico::avancer()
{
	double dv = ACCELERATION * Parametres::DT_2; //u/fr = u/s² * s²/fr² * 1fr
	m_vitesse += dv * QVector3D(cos(angle),sin(angle),0); //vec u/fr
}

void Helico::reculer()
{
	double dv = ACCELERATION * Parametres::DT_2; //u/fr = u/s² * s²/fr² * 1fr
	m_vitesse -= dv * QVector3D(cos(angle),sin(angle),0); //vec u/fr
}

void Helico::haut()
{
	double dv = ACCELERATION_HAUT * Parametres::DT_2; //u/fr = u/s² * s²/fr² * 1fr
	m_vitesse += dv * QVector3D(0,0,1); //vec u/fr
}

void Helico::bas()
{
	double dv  = ACCELERATION_HAUT * Parametres::DT_2; //u/fr = u/s² * s²/fr² * 1fr
	m_vitesse -= dv * QVector3D(0,0,1); //vec u/fr
}

void Helico::action()
{
    if(m_baseComplete)
        Vehicule::action();
}
