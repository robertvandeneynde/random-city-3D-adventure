#ifndef RENDUOPENGL_H
#define RENDUOPENGL_H

#include <vector>

#include <QGLWidget>
#include <QGLBuffer>
#include <QVector3D>
#include <QTimer>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "TextureSet.h"
#include "VertexArray.h"
#include "CameraFreeFly.h"
#include "ObjetControlable.h"
#include <utility>
#include <QHash>

#include "Parametres.h"

#include "shadervao.h"

class VilleInterface;

class RenduOpenGL : public QGLWidget
{
        Q_OBJECT
    public:
        explicit RenduOpenGL(QWidget *parent = 0);
        virtual ~RenduOpenGL();

        void setOptionsDeJeu(OptionsDeJeu const *);
        void setVille(VilleInterface *);
        void setPause(bool);
        void remplirSommets();
    private slots:
        void onEnterFrame();
        void setControle(ObjetControlable *);
    protected:
        virtual void initializeGL();
        virtual void paintGL();

        virtual void keyPressEvent(QKeyEvent *);
        virtual void keyReleaseEvent(QKeyEvent *);
        virtual void mousePressEvent(QMouseEvent *);
        virtual void focusOutEvent(QFocusEvent *);

        void utiliserBuffer();
        void initialiserBuffer();
    private :
        void resetKeys();
    private :
        VilleInterface * m_ville;
        CameraFreeFly * m_camera;
        ObjetControlable * m_controle;
        QPoint m_lastPos;
        QTimer m_timerOnEnterFrame;
        enum ModeCarte {PETITE=0, GRANDE, AUCUNE_CARTE, NOMBRE_MODE_CARTE};
        ModeCarte m_modeCarte;

        OptionsDeJeu const * m_optionsDeJeu;

        VertexArray m_sommets;

        static const QVector3D HAUT;
        QGLBuffer m_buffer;

        struct ToucheVirtuelle
        {
            ToucheVirtuelle(void (ObjetControlable::*a)() = NULL) : compteur(0), action(a) {}
            int compteur;
            void (ObjetControlable::*action)();
        };

        typedef QHash<int, ToucheVirtuelle*> MapClavier;
        ToucheVirtuelle* m_mesTouches[6];
        MapClavier m_keyInput;
        TextureSet m_textureSet;

        ShaderVAO* shader_vao = nullptr;
};

#endif // RENDUOPENGL_H
