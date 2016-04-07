#ifndef VILLEINTERFACE_H
#define VILLEINTERFACE_H

#include "TextureSet.h"
#include "VertexArray.h"
#include "CameraFreeFly.h"
#include "ObjetControlable.h"
#include "Parametres.h"

#include <QObject>
#include <QString>

class VilleInterface : public QObject
{
        Q_OBJECT
    public :
        virtual ~VilleInterface() {}
        virtual void afficherDansFichierTexte(QString fileName = "map.txt") = 0;
        virtual void setTextureSet(TextureSet *) = 0;
        virtual void drawGL() = 0;
        virtual void drawMiniCarte() = 0;
        virtual void enterFrame() = 0;
        virtual void afficherArretes(bool) = 0;
        virtual CameraFreeFly * getCamera() = 0;
        virtual bool generee() = 0;
        virtual ObjetControlable * getObjetControle() = 0;
        virtual Parametres const& params() = 0;
        virtual void setOptionsDeJeu(OptionsDeJeu const*) = 0;
    public slots:
        virtual void changerObjetControle(ObjetControlable *) = 0;
         //Attention : Oublier les = 0 fait que ces fonctions n'ont pas ete definies => undef ref to vtable
    signals:
        void changementControle(ObjetControlable *);
        void generated();
};

#endif // VILLEINTERFACE_H
