#ifndef PERSO_H
#define PERSO_H

#include "Vehicule.h"
#include "ElementRotatif.h"
#include "ImportateurObj.h"
#include "Chargement.h"
#include "OscillateurHarmonique.h"

class Ville;
class Vehicule;

class Perso : public ObjetControlable
{
    public:
        Perso(Ville*, Chargement * = NULL);
        virtual ~Perso();

        void rentrerDansVehicule(Vehicule *);
        void sortirDuVehicule();

        void enterFrame();
        void dessiner();
        void placerCamera(CameraFreeFly *);
        void avancer();
        void reculer();
        void gauche();
        void droite();
        void saut();
        void action();
		void action2();
    public :
        double vitesse;
    private :
        QVector3D m_deplacement;
        bool m_auSol;
        Ville * m_ville;
        Vehicule * m_vehicule;
        OscillateurHarmonique m_oscCamera;
    private :
        void animation();
        void dynamique();
        bool deplacementCorrect();

        //Affichage
    private :
        struct Tete : public ElementRotatif
        {
                Tete();
        };
        struct Main : public ElementRotatif
        {
                Main();
        };
        struct Bras : public ElementRotatif
        {
                Bras();
                void dessinerEnfants();
                Main maMain;
        };
        struct Torse : public ElementRotatif
        {
                Torse();
                void dessinerEnfants();
                Bras bras[2];
        };
        struct Jambe : public ElementRotatif
        {
                Jambe();
        };
        struct Ombre : public ElementRotatif
        {
                Ombre();
                ~Ombre();
		};

        ImportateurObj m_monObj;
        Jambe m_jambes[2];
        Torse m_torse;
        Tete m_tete;
		Ombre m_ombre;

        class Animation
        {
            public :
                Animation(Perso * cible) : m_perso(cible) {}
                virtual ~Animation() {}
                virtual void enterFrame() = 0;
            protected :
                Perso * m_perso;
        };
        class AnimationCourse : public Animation
        {
            public :
                AnimationCourse(Perso * cible);
                ~AnimationCourse();
                void enterFrame();
            private :
                double m_temps;
        };
        class AnimationBras : public Animation
        {
            public :
                AnimationBras(Perso * cible, double angleCible, double vitesse);
                ~AnimationBras();
                void enterFrame();
                void descente();
            private :
                double m_angleCible;
                double m_vitesse;
        };

        class AnimationTete : public Animation
        {
            public :
                AnimationTete(Perso * cible, OscillateurHarmonique);
                void enterFrame();
                void appliquerForce(double);
            private :
                OscillateurHarmonique m_osc;
        };
        class AnimationMains : public Animation
        {
            public :
                AnimationMains(Perso * cible, int nombreDeTours, int dureeUnTour);
                ~AnimationMains();
                void enterFrame();
            private :
                int m_nombreFramesRestants;
                double m_pas;
        };
        class AnimationTorse : public Animation
        {
            public :
                AnimationTorse(Perso * cible, OscillateurHarmonique);
                void enterFrame();
                void appliquerForce(double);
            private :
                OscillateurHarmonique m_osc;
        };
        struct AnimationTete2 : public Animation
        {
            public :
                AnimationTete2(Perso * cible, int dureeAller, int dureeRegard);
                ~AnimationTete2();
                void enterFrame();
            private :
                double m_pas;
                int m_compteur;
                int m_etape;
                int m_temps[3];
        };

        bool m_deplace;
        int m_nombreDeSauts;
        Animation * m_animationCourse;
        AnimationBras * m_animationBras;
        AnimationTete * m_animationTete;
        AnimationMains * m_animationMains;
        AnimationTorse * m_animationTorse;
        AnimationTete2 * m_animationTete2;

        static const double SCALE;
};

#endif // PERSO_H
