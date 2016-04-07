#include "VoiturePNJ.h"
#include "Ville.h"
#include "Element.h"
#include "Dessinateur.h"

#include <cmath>
#include <set>
#include <QVector2D>

int VoiturePNJ::s_count = 0;
ImportateurObj * VoiturePNJ::OBJ[VoiturePNJ::MAX_QUALITY] = {0};
QVector3D VoiturePNJ::m_rouesCoord[4];

const double VoiturePNJ::VITESSE_MAX = 210 /5.0; //unites / secondes
const double VoiturePNJ::DISTANCE_ARRIVEE = 0.20;
const double VoiturePNJ::ANGLE_OK = Math::enRadians(3);
const double VoiturePNJ::SCALE = 0.05;
const double VoiturePNJ::FROTTEMENT_FREINAGE = 0.90;
const double VoiturePNJ::VITESSE_MIN_FREINAGE = 1.0; //unites / secondes

VoiturePNJ::VoiturePNJ(Ville * v)
    : m_ville(v)
    , m_qualiteDessin(MAX_QUALITY)
{
    if(s_count++ == 0)
    {
        for(int i = 0 ; i < MAX_QUALITY ; i++)
        {
            OBJ[i] = new ImportateurObj("Ressources/voitureQualite" + QString::number(i+1) + ".obj", "Ressources/");
            OBJ[i]->utiliserBuffer();
        }

        for(int i = 0 ; i < 4 ; i++)
        {
            m_rouesCoord[i] = QVector3D(
                (i < 2 ? 1 : -1) * 3.300,
                (i % 2 == 1 ? 1 : -1) * 5.500,
                1.127
            );
        }
    }

    m_vitesse = QVector2D(0,0);

    m_GPS = NULL;
}

VoiturePNJ::~VoiturePNJ()
{
    if(--s_count == 0)
    {
        for(int i = 0 ; i < MAX_QUALITY ; i++)
        {
            delete OBJ[i];
            OBJ[i] = NULL;
        }
    }
    delete m_GPS;
}

void VoiturePNJ::setGPS(GPS *gps)
{
    m_GPS = gps;
    m_cible = GPS::Iterateur(m_GPS);
}

void VoiturePNJ::enterFrame()
{
    effectuerChoixGPS();
    deplacer();
    actualiserAngleX();
}

namespace {
bool collisionSegmentCercle(QVector2D seg_a, QVector2D seg_b, QVector2D centre, double rayon)
{
    QVector2D seg_v = seg_b - seg_a,
              pt_v = centre - seg_a,
              seg_v_n = seg_v.normalized();
    qreal longueur_proj_v = QVector2D::dotProduct(pt_v, seg_v_n);
    QVector2D closest_point =
        longueur_proj_v <= 0 ? seg_a
        : longueur_proj_v >= seg_v.length() ? seg_b
        : seg_a + longueur_proj_v * seg_v_n;

    return (centre - closest_point).lengthSquared() < rayon * rayon;
}
}

void VoiturePNJ::effectuerChoixGPS()
{
    if(m_GPS)
    {
        if(m_GPS->arrivee())
        {
            QVector2D pos2D = position.toVector2D();
            QVector2D cible2D;

            forever
            {
                cible2D = m_cible->position3D().toVector2D();
                //if((cible2D - pos2D).lengthSquared() > DISTANCE_ARRIVEE * DISTANCE_ARRIVEE)
                if(! collisionSegmentCercle(pos2D, pos2D + m_vitesse, cible2D, DISTANCE_ARRIVEE))
                    break; //La destination est loin
                else
                {
                    //position = m_cible->position3D();
                    ++m_cible;
                    if(! m_cible.valide())
                    {
                        m_GPS->calculer(m_GPS->arrivee(), m_ville->cibleGPSAleatoire());
                        m_cible = GPS::Iterateur(m_GPS);
                    }
                }
            }
            QVector2D directionActuelle(cos(angle), sin(angle));
            QVector2D directionSouhaitee = (cible2D - pos2D).normalized();

            double signeVitesseAngulaire = 0;
            const int AVANCER(0), RALENTIR(1), RIEN(2);
			int action = RIEN;
            if( QVector2D::dotProduct(directionActuelle, directionSouhaitee) < cos(ANGLE_OK) )
            {
                //On doit tourner
                QVector2D rotated(-directionActuelle.y(), directionActuelle.x());
                signeVitesseAngulaire = QVector2D::dotProduct(rotated, directionSouhaitee) > 0 ?
                    1 : -1;

                //Et ralentir
				action = RALENTIR;
            }
            else
			{
                //Bonne direction
                action = AVANCER;
                //On fait attention aux voitures devant nous
                QPoint maPosition = m_ville->calculerZone(this);
                std::vector<Voiture*> voituresProches;

                for(int i = -2; i < 2; i++)
                {
                    for(int j = -2; j < 2; j++)
                    {
                        std::set<Voiture*> voituresDansZone = m_ville->voituresA( maPosition + QPoint(i,j) );
                        voituresProches.insert(voituresProches.end(), voituresDansZone.begin(), voituresDansZone.end());
                    }
                }

                QVector2D directionDroite(directionActuelle.y(), -directionActuelle.x());
				const double ANGLE_PRIO_OK( Math::enRadians(45) );
                for(Voiture* voiture : voituresProches)
                {
                    QVector2D positionRelative(voiture->position - position);
                    double loin = QVector2D::dotProduct(directionActuelle, positionRelative);
                    double side = QVector2D::dotProduct(directionDroite, positionRelative);
                    QVector2D autreDirectionActuelle(cos(voiture->angle), sin(voiture->angle));
                    double cosDifferenceDirection = QVector2D::dotProduct(autreDirectionActuelle, directionActuelle);
					if((loin > 0 && loin < 2 && side > -0.5 && side < 0.5) && cosDifferenceDirection > cos(ANGLE_PRIO_OK))
					{
						action = RALENTIR;
						break;
					}
                }

				if(action == AVANCER)
                {
                    //On regarde les panneaux de signalisation
					//Si la cible est une entrée de carrefour
                    if(EntreeCarrefour* entree = dynamic_cast<EntreeCarrefour*>((NoeudDijkstra *)m_cible))
                    {
                        if(! entree->getAvertissements().empty())
                        {
                            QVector2D direction( entree->getRoute()->directionVers( entree->getCarrefour()->zone() ) );
							double produitVitesse = QVector2D::dotProduct(direction, getVitesse()); //u/fr
							double produitPosition = QVector2D::dotProduct(direction, QVector2D(entree->position3D() - position)); //u
							double tempsSecondes = (produitPosition / produitVitesse) * Parametres::DT; //fr = fr * s/fr
							//Vec Projection = direction * produit => Projection.length() == abs(produit)
							if(tempsSecondes > 0 && (abs(produitPosition) < 1.0 || tempsSecondes < 2) )
								action = RALENTIR;
                        }
					}
                }
            }

			m_vitesseAngle = signeVitesseAngulaire * (Mustang::VITESSE_ROTATION * Parametres::DT); //rad/fr = rad/s * s/fr
			if(action == RALENTIR)
			{
				m_vitesse *= (m_vitesse.lengthSquared() * Parametres::FPS_2 > VITESSE_MIN_FREINAGE * VITESSE_MIN_FREINAGE) ? // u²/fr² * fr²/s²
					FROTTEMENT_FREINAGE : 0.0;
			}
			else if(action == AVANCER)
			{
				if(m_vitesse.lengthSquared() * Parametres::FPS_2 < VITESSE_MAX * VITESSE_MAX)
					m_vitesse += Mustang::ACCELERATION * Parametres::DT_2 * directionActuelle; // u/fr = u/s² * s²/fr² * 1fr
			}
        }
    }
    else
    {
        m_vitesse *= 0;
        m_vitesseAngle = 0;
    }
}

void VoiturePNJ::setQualiteDessin(int q)
{
    m_qualiteDessin = q;
}

void VoiturePNJ::dessiner()
{
    Repere R;
    R.translate(position).rotate(angle + Math::PI/2, Repere::Z).rotate(angleX, Repere::X).scale(SCALE);

    OBJ[m_qualiteDessin - 1]->dessiner();
}

Ville* VoiturePNJ::getVille()
{
    return m_ville;
}

void VoiturePNJ::calculerCoordRoues()
{
    using namespace Math::Matrice;

    double matriceX[16], matriceZ[16], matriceS[16];
    rotationX(matriceX, angleX);
    rotationZ(matriceZ, angle + Math::PI/2.0);
    scale(matriceS, SCALE);

    double matriceTotale[16];
    identity(matriceTotale);
    multiplicationMatrice(matriceTotale, matriceZ);
    multiplicationMatrice(matriceTotale, matriceX);
    multiplicationMatrice(matriceTotale, matriceS);

    for(int i = 0; i < 4; i++)
    {
        double rel[3] = { m_rouesCoord[i].x(), m_rouesCoord[i].y(), m_rouesCoord[i].z()};
        multiplication(matriceTotale, rel, 3);

        m_coordRouesAbsolues[i] = position + QVector3D(rel[0], rel[1], rel[2]);
    }
}

void VoiturePNJ::actualiserAngleX()
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
