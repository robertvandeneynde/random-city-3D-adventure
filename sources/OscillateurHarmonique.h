#ifndef OSCILLATEURHARMONIQUE_H
#define OSCILLATEURHARMONIQUE_H

struct OscillateurHarmonique
{
    OscillateurHarmonique(double laRaideur, double lAmortissement, double laValeurCible = 0, double laValeurInitiale = 0, double laVitesseInitiale = 0)
        : valeur(laValeurInitiale)
        , valeurCible(laValeurCible)
        , raideur(laRaideur)
        , amortissement(lAmortissement)
        , vitesse(laVitesseInitiale)
    {

    }
    void enterFrame()
    {
        vitesse -= raideur * (valeur - valeurCible) + vitesse * amortissement;
        valeur += vitesse;
    }
    void appliquerForce(double f)
    {
        vitesse += f;
    }
    void resetTo(double v)
    {
        valeur = valeurCible = v;
    }
    double valeur; // u
    double valeurCible; // u
    double raideur; // 1/frame²
    double amortissement; // 1/frame
    double vitesse; // u/frame
};

#endif // OSCILLATEURHARMONIQUE_H
