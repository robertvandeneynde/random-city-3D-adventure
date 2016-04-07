#ifndef PARAMETRES_H
#define PARAMETRES_H

#include <QString>
#include <QObject>
#include <vector>
#include <QColor>

struct Parametres
{
    int minBlockSize;
    int maxBlockSize;
    int citySizeX;
    int citySizeY;
    double moyenneH;
    double ecartTypeH;
    bool activerJardins;
    int tailleRouteMaximale;
    int nombreDeVoituresPNJ;
    QString nom;
    std::vector< std::vector<QColor> > couleursBatiments;

	//u = unites
	//s = secondes
	//fr = frame
    static const int DT_INT_MS; //ms / fr
	static const double DT; //s / fr
	static const double DT_2; //DT * DT = s² / fr²
	static const double FPS; //fr / s
	static const double FPS_2; //fr² / s²
};



namespace Options
{

struct ParametresPerformance
{
    double farPlane;
    double distanceVoiture;
};
struct Commandes
{
    enum Mode {AZERTY, QWERTY};
    Mode mode;
};

}

struct OptionsDeJeu
{
    public :
        Options::ParametresPerformance performance;
        Options::Commandes commandes;
};


#endif // PARAMETRES_H
