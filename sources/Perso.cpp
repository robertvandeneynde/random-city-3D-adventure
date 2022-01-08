#include "Perso.h"

#include <QDebug>

#include <QMatrix4x4>
#include "fonctionsMathematiques.h"
#include "Ville.h"
#include "Dessinateur.h"
#include "Element.h"

#include "BoulePeinture.h"

const double Perso::SCALE = 0.042;

Perso::Perso(Ville * ville, Chargement * chargement)
    : m_ville(ville)
    , m_vehicule(NULL)
    , m_oscCamera(0.035, 0.300)
    , m_monObj("Ressources/perso.obj", "Ressources/")
{
    m_deplace = false;
    m_auSol = true;
    m_nombreDeSauts = 2;
    vitesse = 0;
    m_deplacement = QVector3D(0,0,0);

    m_animationCourse = NULL;
    m_animationBras = NULL;
    m_animationTete = new AnimationTete(this, OscillateurHarmonique(0.010, 0.050) );
    m_animationMains = NULL;
    m_animationTorse = new AnimationTorse(this, OscillateurHarmonique(0.007, 0.050) );
    m_animationTete2 = NULL;

    if(m_monObj.erreurOuvertureFichier())
        if(chargement)
            chargement->afficherTexte("Erreur ouverture perso.obj");

    m_monObj.utiliserBuffer();

    m_tete.donnees = m_monObj.enleverAffichage("Tete");
    m_tete.coord = QVector3D(0.000, 0.000, 2.950+2.8);
    m_tete.axeRotation = QVector3D(0,0,1);

    m_torse.donnees = m_monObj.enleverAffichage("Torse");
    m_torse.coord = QVector3D(0.000, 0.000, 3.741);
    m_torse.axeRotation = QVector3D(0,0,1);

    m_torse.bras[0].donnees = m_monObj.enleverAffichage("BrasGauche");
    m_torse.bras[1].donnees = m_monObj.enleverAffichage("BrasDroit");
    m_torse.bras[0].coord = QVector3D(1.700, -0.023, 0.945+2.8);
    m_torse.bras[1].coord = QVector3D(-1.700, -0.023, 0.945+2.8);

    m_torse.bras[0].maMain.donnees = m_monObj.enleverAffichage("MainGauche");
    m_torse.bras[1].maMain.donnees = m_monObj.enleverAffichage("MainDroite");
    m_torse.bras[0].maMain.coord = QVector3D(2.947, -1.412, 0.684+2.8);
    m_torse.bras[1].maMain.coord = QVector3D(-2.947, -1.412, 0.684+2.8);
    m_torse.bras[0].maMain.axeRotation = QVector3D(0,1,0);
    m_torse.bras[1].maMain.axeRotation = QVector3D(0,-1,0);

    m_jambes[0].donnees = m_monObj.enleverAffichage("JambeGauche");
    m_jambes[1].donnees = m_monObj.enleverAffichage("JambeDroite");
    m_jambes[0].coord = QVector3D(0.800, 0.000, 1.940);
    m_jambes[1].coord = QVector3D(-0.800, 0.000, 1.940);

    m_tete.translater();
    m_torse.translater();
    for(int i = 0 ; i < 2 ; i++)
    {
        m_jambes[i].translater();
        m_torse.bras[i].translater();
        m_torse.bras[i].maMain.translater();

        m_torse.bras[i].maMain.coord -= m_torse.bras[i].coord;
        m_torse.bras[i].coord -= m_torse.coord;
    }
}

Perso::~Perso()
{
    delete m_animationCourse;
    delete m_animationBras;
    delete m_animationTete;
    delete m_animationMains;
    delete m_animationTorse;
}

void Perso::rentrerDansVehicule(Vehicule * cible)
{
    m_vehicule = cible;
    m_vehicule->setPerso(this);
}

void Perso::sortirDuVehicule()
{
    if(m_vehicule)
    {
        m_vehicule->setPerso(NULL);
        m_ville->changerObjetControle(this);
    }
    m_vehicule = NULL;
}

void Perso::enterFrame()
{
    if(m_vehicule)
    {
        m_oscCamera.resetTo(m_vehicule->angle);
    }
    else
    {
        dynamique();
        animation();
    }
}

void Perso::dynamique()
{
	double const GRAVITE = 0.01 / Parametres::DT_2; // u/s² //0.01;

    if( deplacementCorrect() )
        position += m_deplacement;

    m_deplacement = QVector3D(0,0,0);

	vitesse -= GRAVITE * Parametres::DT_2; // u/fr = u/s² * s²/fr² * 1fr
    position += vitesse * QVector3D(0,0,1);

    double hauteur = m_ville->hauteurEffective(position.x(), position.y());
    m_auSol = position.z() < hauteur;
    if(m_auSol)
    {
        position.setZ( hauteur );
        vitesse = 0;
		m_nombreDeSauts = 2;
		Element* elem = m_ville->at( position.toPointF() );
		if(Batiment* bat = dynamic_cast<Batiment*>(elem))
		{
            emit m_ville->batimentSteppedOn(bat);
            bat->setCouleur(QColor(255, 200, 0));
		}
	}
    m_oscCamera.valeurCible = angle;
    m_oscCamera.enterFrame();

    m_ombre.coord = QVector3D(position.x(), position.y(), hauteur+0.03);
}

bool Perso::deplacementCorrect()
{
    const double LIMITE_ESCALADE = 0.20;

    QVector3D nouvellePosition = position + m_deplacement;
    double nouvelleHauteur = m_ville->hauteurEffective(nouvellePosition.x(), nouvellePosition.y());

    return nouvelleHauteur - position.z() < LIMITE_ESCALADE;
}

void Perso::animation()
{
    if(! m_deplace)
    {
        delete m_animationCourse;
        m_animationCourse = NULL;
    }
    else
    {
        if(! m_animationCourse)
            m_animationCourse = new AnimationCourse(this);

        m_animationCourse->enterFrame();
    }

    m_deplace = false;

    if(m_auSol)
    {
        if(m_animationBras)
            m_animationBras->descente();
    }
    else
    {
        if(! m_animationBras)
            m_animationBras = new AnimationBras(this, 90, 8);
    }

    if(m_animationBras)
        m_animationBras->enterFrame();

    m_animationTete->enterFrame();

    if(m_animationMains)
        m_animationMains->enterFrame();
    else if(rand() % 75 == 0)
		m_animationMains = new AnimationMains(this, 2 + rand() % 3, 0.6 / Parametres::DT); // fr = s / (s/fr)

    if(m_animationTete2)
        m_animationTete2->enterFrame();
    else if(rand() % 150 == 0)
		m_animationTete2 = new AnimationTete2(this, rand() % 2 ? 5 : 10, (1 + Math::randUniform()) / Parametres::DT); // fr = s / (s/fr)

    m_animationTorse->enterFrame();
}

Perso::Tete::Tete()
{

}

Perso::Torse::Torse()
{

}

Perso::Bras::Bras()
{
    axeRotation = QVector3D(-1,0,0);
}

Perso::Jambe::Jambe()
{
    axeRotation = QVector3D(1,0,0);
}

Perso::Main::Main()
{
    angle = 0;
}

void Perso::dessiner()
{
    if(! m_vehicule)
        m_ombre.dessiner();
    Repere R;
    R.translate(position).rotate(angle + Math::PI/2, Repere::Z).scale(SCALE);

    m_monObj.dessiner();
    for(int i = 0 ; i < 2; i++)
        m_jambes[i].dessiner();
    m_tete.dessiner();
    m_torse.dessiner();
}

void Perso::Torse::dessinerEnfants()
{
    for(int i = 0 ; i < 2 ; i++)
        bras[i].dessiner();
}
void Perso::Bras::dessinerEnfants()
{
    maMain.dessiner();
}

Perso::Ombre::Ombre()
{
    donnees = new VertexArray();
    donnees->setModeAffichage(GL_TRIANGLES);

    for(int i = 0 ; i < 8 ; i++)
    {
        double a = i * Math::PI * 2 / 8;
        double a2 = (i+1)%8 * Math::PI * 2 / 8;
        donnees->nouvellePosition(0, 0, 0);
        donnees->nouvellePosition(0.05 * cos(a), 0.05 * sin(a), 0);
        donnees->nouvellePosition(0.05 * cos(a2), 0.05 * sin(a2), 0);

        for(int i = 0 ;  i < 3; i++)
        {
            donnees->nouvelleCouleur(0.1, 0.1, 0.1);
            donnees->nouvelleNormale(0,0,1);
        }
    }
}
Perso::Ombre::~Ombre()
{
    delete donnees;
}

void Perso::placerCamera(CameraFreeFly * camera)
{
    // placerCameraParamPos(camera, 2.0, 1.2, 1.0, m_oscCamera.valeur);
    placerCameraParamPos(camera, 2.0, 1.8, 1.5, m_oscCamera.valeur);
    // placerCameraParamAngle(camera, 2.0, 1.0, Math::enRadians(80));
    camera->avancer(0.80);
}

void Perso::avancer()
{
    m_deplace = true;
	m_deplacement += 2.5 * Parametres::DT * QVector3D(cos(angle), sin(angle), 0); //u = u/s * s/fr
    m_animationTete->appliquerForce(-0.010);
}

void Perso::reculer()
{
    m_deplace = true;
	m_deplacement -= 2.5 * Parametres::DT * QVector3D(cos(angle), sin(angle), 0); //u = u/s * s/fr
    m_animationTete->appliquerForce(0.005);
}

void Perso::gauche()
{
    angle += Math::enRadians(5);
    m_animationTorse->appliquerForce(-0.25);
}

void Perso::droite()
{
    angle -= Math::enRadians(5);
    m_animationTorse->appliquerForce(+0.25);
}

void Perso::saut()
{
    const double V_SAUT = 0.20;
    if(m_auSol)
    {
        vitesse = V_SAUT;
        m_nombreDeSauts--;
    }
    else
    {
        if(m_nombreDeSauts > 0)
        {
            vitesse = 0.5 * V_SAUT;
            m_nombreDeSauts = 0;
        }
    }
}

void Perso::action()
{
    m_ville->rentrerDansVehicule(this);
}

void Perso::action2()
{
	QMatrix4x4 mat;

	const double angleMontee = 10;
	const double forceLancer = 10; // u/s

	mat.rotate(Math::enDegres(angle) + 90, 0,0,1);
	mat.rotate(Math::enDegres(angleX) + 90 - angleMontee, 1,0,0);

	QVector3D vit = mat.map(QVector3D(0,0,forceLancer));

	BoulePeinture* boule = new BoulePeinture(m_ville, vit, Qt::red);
	boule->position = position + QVector3D(0, 0, m_tete.coord.z() * SCALE);
	boule->angle = angle;
}


Perso::AnimationCourse::AnimationCourse(Perso * cible)
    : Animation(cible)
{
    m_temps = 0;
}
void Perso::AnimationCourse::enterFrame()
{
    const double amplitude = 20;
    const double pulsation = 0.8;
    for(int i = 0 ; i < 2 ; i++)
        m_perso->m_jambes[i].angle = amplitude * std::sin(pulsation * (i == 0 ? 1 : -1) * m_temps);

    m_temps += 1;
}
Perso::AnimationCourse::~AnimationCourse()
{
    for(int i = 0 ; i < 2 ; i++)
        m_perso->m_jambes[i].angle = 0;
}


Perso::AnimationBras::AnimationBras(Perso * cible, double angleCible, double vitesse)
    : Animation(cible)
    , m_angleCible(angleCible)
    , m_vitesse(vitesse)
{

}
Perso::AnimationBras::~AnimationBras()
{

}
void Perso::AnimationBras::descente()
{
    if(m_vitesse > 0)
    {
        m_vitesse = -m_vitesse;
        m_angleCible = 0;
    }
}

void Perso::AnimationBras::enterFrame()
{
	if(m_perso->m_torse.bras[0].angle * m_vitesse < m_angleCible * m_vitesse)
        m_perso->m_torse.bras[1].angle = (m_perso->m_torse.bras[0].angle += m_vitesse);
    else
    {
        m_perso->m_torse.bras[0].angle = m_perso->m_torse.bras[1].angle = m_angleCible;
        if(m_vitesse < 0)
        {
            m_perso->m_animationBras = NULL;
            delete this;
        }
    }
}
Perso::AnimationTete::AnimationTete(Perso *cible, OscillateurHarmonique osc)
    : Animation(cible)
    , m_osc(osc)
{
    m_osc.valeur = m_perso->m_tete.coord.y();
}
void Perso::AnimationTete::enterFrame()
{
    m_osc.enterFrame();
    m_perso->m_tete.coord.setY( m_osc.valeur );
}
void Perso::AnimationTete::appliquerForce(double f)
{
    m_osc.appliquerForce(f);
}

Perso::AnimationMains::AnimationMains(Perso *cible, int nombreDeTours, int dureeUnTour)
    : Animation(cible)
{
    m_pas = 360.0 / dureeUnTour;
    m_nombreFramesRestants = nombreDeTours * dureeUnTour;
}
Perso::AnimationMains::~AnimationMains()
{
    m_perso->m_torse.bras[0].maMain.angle = m_perso->m_torse.bras[1].maMain.angle = 0;
}
void Perso::AnimationMains::enterFrame()
{
    m_perso->m_torse.bras[0].maMain.angle = (m_perso->m_torse.bras[1].maMain.angle += m_pas);
    m_nombreFramesRestants--;
    if(m_nombreFramesRestants <= 0)
    {
        m_perso->m_animationMains = NULL;
        delete this;
    }
}

Perso::AnimationTorse::AnimationTorse(Perso *cible, OscillateurHarmonique osc)
    : Animation(cible)
    , m_osc(osc)
{
    m_osc.valeur = m_perso->m_torse.angle;
}
void Perso::AnimationTorse::enterFrame()
{
    m_osc.enterFrame();
    m_perso->m_torse.angle = m_osc.valeur;
}

void Perso::AnimationTorse::appliquerForce(double f)
{
    m_osc.appliquerForce(f);
}

Perso::AnimationTete2::AnimationTete2(Perso *cible, int dureeAller, int dureeRegard)
    : Animation(cible)
{
    m_pas = 180.0 / dureeAller;
    m_compteur = 0;
    m_etape = 0;
    m_temps[0] = dureeAller;
    m_temps[1] = dureeRegard;
    m_temps[2] = dureeAller;
}
Perso::AnimationTete2::~AnimationTete2()
{
    m_perso->m_tete.angle = 0;
}

void Perso::AnimationTete2::enterFrame()
{
    if(m_compteur < m_temps[m_etape])
    {
        if(m_etape == 0 || m_etape == 2)
            m_perso->m_tete.angle += m_pas;

        m_compteur++;
    }
    else
    {
        m_etape++;
        m_compteur = 0;
        if(m_etape == 3)
        {
            m_perso->m_animationTete2 = NULL;
            delete this;
        }
    }
}
