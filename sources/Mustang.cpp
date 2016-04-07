#include "Mustang.h"

#include <QDebug>
#include "Dessinateur.h"
#include "Ville.h"

#include "fonctionsMathematiques.h"

const double Mustang::ACCELERATION = 10; //Unités par seconde par seconde
const double Mustang::VITESSE_ROTATION = Math::enRadians(90); //Radians par seconde
const double Mustang::DELTA_Z_MAX = 1; //Unité
const double Mustang::SCALE = 0.06;

Mustang::Mustang(Ville* v, Chargement * chargement)
    : Vehicule(v)
    , m_monObj("Ressources/mustang.obj", "Ressources/")
    , m_oscCamera(0.035, 0.300)
{
    if(m_monObj.erreurOuvertureFichier())
        if(chargement)
            chargement->afficherTexte("Erreur ouverture mustang.obj");

    m_monObj.utiliserBuffer();

    QString noms[] = {"TireBR_Plane", "TireFR_Plane.001", "TireBL_Plane.002", "TireFL_Plane.003"};
    for(int i = 0 ; i < 4 ; i++)
    {
        m_roues[i].donnees = m_monObj.enleverAffichage(noms[i]);
        m_roues[i].coord = QVector3D(
            (i < 2 ? 1 : -1) * 3.300,
            (i % 2 == 1 ? 1 : -1) * 5.500,
            1.127);
        m_coinsCoord[i] = QVector3D(
            (i < 2 ? 1 : -1) * 3.600,
            (i % 2 == 1 ? 1 : -1) * 9.000,
            0);
        m_roues[i].translater();
    }

    m_vitesse = QVector2D(0,0);
}

Ville* Mustang::getVille()
{
    return m_ville;
}

Mustang::Roue::Roue()
{
    setVitesseMax(1);
    angle = 0;
    axeRotation = QVector3D(1,0,0);
}

void Mustang::enterFrame()
{
    calculerCoordCoins();
    deplacer();
    actualiserAngleX();
    Vehicule::enterFrame();

    m_oscCamera.valeurCible = angle;
    m_oscCamera.enterFrame();
    for(int i = 0 ; i < 4 ; i++)
        m_roues[i].appliquerVitesse();
}
void Mustang::dessiner()
{
    Repere repere;
    repere.translate(position).rotate(angle + Math::PI/2, Repere::Z).rotate(angleX, Repere::X).scale(SCALE);

    m_monObj.dessiner();
    for(int i = 0 ; i < 4 ; i++)
        m_roues[i].dessiner();
}

void Mustang::placerCamera(CameraFreeFly * camera)
{
    double angleAUtiliser = m_oscCamera.valeur;
    double recul = 2.0, hauteur = 1.5, avancement = 2.0;

    QVector3D direction(cos(angleAUtiliser), sin(angleAUtiliser), 0);
    QVector3D posCamera = position - recul * direction + QVector3D(0, 0, hauteur);
    camera->setPosition(posCamera);
    camera->regarderVers(position + avancement * direction);

    camera->avancer(0.25);
}

void Mustang::deplacer()
{
    QVector2D normaleMur;
    if(! deplacementCorrect(position + cos(angleX) * m_vitesse.toVector3D(), m_vitesseAngle + angle, normaleMur))
    {
        gererRebond(normaleMur);
        m_vitesseAngle = 0;
    }

    Voiture::deplacer();
}

bool Mustang::deplacementCorrect(QVector3D newPosition, double newAngle, QVector2D &normaleMur)
{
    const int indices[4] = {2, 0, 3, 1};

    QVector3D newCoordCoins[4];
    calculerCoordCoins(newPosition, newCoordCoins, newAngle);

    std::vector<QVector3D> interpolationActuelle, interpolationFuture;

    for(int i = 0; i < 4; i++)
    {
        QVector3D a = m_coordCoinsAbsolus[i];
        QVector3D b = m_coordCoinsAbsolus[indices[i]];
        for(double j = 0; j < 1; j += 0.1)
            interpolationActuelle.push_back(a + (b-a)*j);
    }

    for(int i = 0; i < 4; i++)
    {
        QVector3D a = newCoordCoins[i];
        QVector3D b = newCoordCoins[indices[i]];
        for(double j = 0; j < 1; j += 0.1)
            interpolationFuture.push_back(a + (b-a)*j);
    }
	
	normaleMur = QVector2D(0,0);
    bool pasDeCollision = true;

    for(size_t i = 0; i < interpolationActuelle.size(); i++)
    {
        double z = m_ville->hauteurEffectiveVoiture(interpolationActuelle[i].x(), interpolationActuelle[i].y());
        double newZ = m_ville->hauteurEffectiveVoiture(interpolationFuture[i].x(), interpolationFuture[i].y());

        if(newZ - z > DELTA_Z_MAX)
        {
            normaleMur += determinerNormaleMur(interpolationActuelle[i].toVector2D(),interpolationFuture[i].toVector2D());
            pasDeCollision = false;
        }

    }

    return pasDeCollision;
}

void Mustang::actualiserAngleX()
{
    calculerCoordRoues();

    QVector2D arriere = (m_coordRouesAbsolues[0] + m_coordRouesAbsolues[2]).toVector2D()/2.0;
    QVector2D avant = (m_coordRouesAbsolues[1] + m_coordRouesAbsolues[3]).toVector2D()/2.0;
    double longueur = (arriere - avant).length();

    double hauteurs[4];
    for(int i = 0; i < 4; i++)
        hauteurs[i] =  m_ville->hauteurEffective(m_coordRouesAbsolues[i].x(), m_coordRouesAbsolues[i].y());

    double arriereZ = std::max(hauteurs[0],hauteurs[2]);
    double avantZ = std::max(hauteurs[1],hauteurs[3]);

    angleX = atan((avantZ - arriereZ)/longueur);
}

void Mustang::gauche()
{
	m_vitesseAngle = VITESSE_ROTATION * Parametres::DT; // u/fr = u/s * s/fr
}

void Mustang::droite()
{
	m_vitesseAngle = -VITESSE_ROTATION * Parametres::DT; // u/fr = u/s * s/fr
}

void Mustang::avancer()
{
	m_vitesse += ACCELERATION * Parametres::DT_2 * QVector2D(cos(angle), sin(angle)); // u/fr = u/s² * s²/fr² * 1fr
}

void Mustang::reculer()
{
	m_vitesse -= ACCELERATION * Parametres::DT_2 * QVector2D(cos(angle), sin(angle)); // u/fr = u/s² * s²/fr² * 1fr
}

void Mustang::calculerCoordRoues()
{
    calculerCoordRoues(position, m_coordRouesAbsolues, angle);
}

void Mustang::calculerCoordCoins()
{
    calculerCoordRoues(position, m_coordCoinsAbsolus, angle);
}

void Mustang::calculerCoordRoues(QVector3D laPosition, QVector3D absolu[4], double lAngle)
{
    using namespace Math::Matrice;

    double matriceX[16], matriceZ[16], matriceS[16];
    rotationX(matriceX, angleX);
    rotationZ(matriceZ, lAngle + Math::PI/2.0);
    scale(matriceS, SCALE);

    double matriceTotale[16];
    identity(matriceTotale);
    multiplicationMatrice(matriceTotale, matriceZ);
    multiplicationMatrice(matriceTotale, matriceX);
    multiplicationMatrice(matriceTotale, matriceS);

    for(int i = 0; i < 4; i++)
    {
        double rel[3] = { m_roues[i].coord.x(), m_roues[i].coord.y(), m_roues[i].coord.z()};
        multiplication(matriceTotale, rel, 3);

        absolu[i] = laPosition + QVector3D(rel[0],rel[1],rel[2]);
    }
}

void Mustang::calculerCoordCoins(QVector3D laPosition, QVector3D absolu[4], double lAngle)
{
    using namespace Math::Matrice;

    double matriceX[16], matriceZ[16], matriceS[16];
    rotationX(matriceX, angleX);
    rotationZ(matriceZ, lAngle + Math::PI/2.0);
    scale(matriceS, SCALE);
    for(int i = 0; i < 4; i++)
    {
        double rel[3] = { m_coinsCoord[i].x(), m_coinsCoord[i].y(), m_coinsCoord[i].z()};

        multiplication(matriceZ, rel, 3);
        multiplication(matriceX, rel, 3);
        multiplication(matriceS, rel, 3);

        absolu[i] = laPosition + QVector3D(rel[0],rel[1],rel[2]);
    }
}

QVector2D Mustang::determinerNormaleMur(QVector2D pos, QVector2D newPos)
{
    if(m_ville->hauteurEffectiveVoiture(pos.x(),pos.y()) > 1e9 && m_ville->hauteurEffectiveVoiture(newPos.x(),newPos.y()) < 1e9)
    {
        QVector2D temp = pos;
        pos = newPos;
        newPos = temp;
    }
    // cas 2
    /*   pos1    1|     newPos
        0         |_____________2
         pos     3      pos2       */

    // cas 1
    /*   pos1          newPos
        _______________________
         pos           pos2       */


    // murs seulement en 1x ou en 1y !

    QVector2D pos1 = QVector2D(pos.x(),newPos.y());
    QVector2D pos2 = QVector2D(newPos.x(),pos.y());

    // pos, pos1, newPos, pos2
    double hauteurs[4] = {   m_ville->hauteurEffectiveVoiture(pos.x(),pos.y())
                           , m_ville->hauteurEffectiveVoiture(pos1.x(),pos1.y())
                           , m_ville->hauteurEffectiveVoiture(newPos.x(),newPos.y())
                           , m_ville->hauteurEffectiveVoiture(pos2.x(),pos2.y())
                         };

    bool murs[4];

    for(int i = 0; i < 4; i++)
    {
        murs[i] = std::abs(hauteurs[i] - hauteurs[(i+1)%4]) > DELTA_Z_MAX;
    }

    bool enCoin = false;
    int compteur = 0;
    for(int i = 0;i < 4; i++)
    {
        if(murs[i])
            compteur++;
        else if(compteur > 0)
            compteur--;

        if(compteur == 2)
            enCoin = true;
    }

    //qDebug() << enCoin;

    if(enCoin)
    {
        return (pos - newPos);
    }
    else if(murs[0] && murs[2] && !murs[1] && !murs[3])
    {
        return QVector2D(0, pos.y() - newPos.y());

    }
    else if(!murs[0] && !murs[2] && murs[1] && murs[3])
    {
        return QVector2D(pos.x() - newPos.x(), 0);
    }
    else
    {
		qDebug() << "Mustang::determinerNormaleMur erreur !";
        return QVector2D(0, 0);
    }
}

void Mustang::gererRebond(QVector2D normale)
{
    normale.normalize();
    m_vitesse -= 1.5 * normale * (QVector2D::dotProduct(m_vitesse,normale) - 0.01);
    m_vitesse *= 0.95;
}

void Mustang::action2()
{
	m_perso->action2();
}
