#include "RenduOpenGL.h"
#include "VilleInterface.h"

#include <QDebug>
#include <QTime>
#include <GL/glu.h>
#include <cassert>

const QVector3D RenduOpenGL::HAUT(0,0,1);

RenduOpenGL::RenduOpenGL(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    , m_ville(nullptr)
    , m_camera(nullptr)
    , m_controle(nullptr)
    , m_timerOnEnterFrame(this)
    , m_modeCarte(PETITE)
    , m_buffer(QGLBuffer::VertexBuffer)
{
    m_timerOnEnterFrame.setInterval(Parametres::DT_INT_MS);
    connect(&m_timerOnEnterFrame, SIGNAL(timeout()), this, SLOT(onEnterFrame()));
    connect(&m_timerOnEnterFrame, SIGNAL(timeout()), this, SLOT(updateGL()));

    setPause(false);
    setMouseTracking(true);
}

RenduOpenGL::~RenduOpenGL()
{
    for(ToucheVirtuelle* t : m_mesTouches)
        delete t;
}

void RenduOpenGL::setOptionsDeJeu(const OptionsDeJeu * cible)
{
    m_optionsDeJeu = cible;
    resetKeys();
    if(m_ville)
        m_ville->setOptionsDeJeu(m_optionsDeJeu);
}

void RenduOpenGL::resetKeys()
{
    Qt::Key haut, bas, gauche, droite;

    bas = Qt::Key_S, droite = Qt::Key_D;
    if(m_optionsDeJeu->commandes.mode == Options::Commandes::AZERTY)
        haut = Qt::Key_Z, gauche = Qt::Key_Q;
    else
        haut = Qt::Key_W, gauche = Qt::Key_A;

    m_keyInput.clear();

    int n = 0;
    m_keyInput[Qt::Key_Up] = m_keyInput[haut] = m_mesTouches[n++] = new ToucheVirtuelle(&ObjetControlable::avancer);
    m_keyInput[Qt::Key_Down] = m_keyInput[bas] = m_mesTouches[n++] = new ToucheVirtuelle(&ObjetControlable::reculer);
    m_keyInput[Qt::Key_Right] = m_keyInput[droite] = m_mesTouches[n++] = new ToucheVirtuelle(&ObjetControlable::droite);
    m_keyInput[Qt::Key_Left] = m_keyInput[gauche] = m_mesTouches[n++] = new ToucheVirtuelle(&ObjetControlable::gauche);
    m_keyInput[Qt::Key_T] = m_mesTouches[n++] = new ToucheVirtuelle(&ObjetControlable::haut);
    m_keyInput[Qt::Key_G] = m_mesTouches[n++] = new ToucheVirtuelle(&ObjetControlable::bas);
}

void RenduOpenGL::setVille(VilleInterface * cible)
{
    if(m_ville != cible)
    {
        if(m_ville)
            disconnect(m_ville, SIGNAL(changementControle(ObjetControlable*)), this, SLOT(setControle(ObjetControlable *)));

        m_ville = cible;
        m_ville->setOptionsDeJeu(m_optionsDeJeu);
        m_ville->setTextureSet(&m_textureSet);

        connect(m_ville, SIGNAL(changementControle(ObjetControlable*)), this, SLOT(setControle(ObjetControlable *)));

        setControle(m_ville->getObjetControle());
        m_camera = m_ville->getCamera();

        updateGL();
    }
}

void RenduOpenGL::setControle(ObjetControlable * cible)
{
    m_controle = cible;
}

void RenduOpenGL::initializeGL()
{
    //Definition de la couleur du fond
    glClearColor(49/255.0, 207/255.0, 240/255.0, 1);

    // Augmentation de la qualité du calcul de perspective
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    // Choix du shader
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Paramètre des l'éclairage
    GLfloat lightAmbient[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat lightDiffuse[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat lightSpecular[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //Paramètres de brouillard
    glDisable(GL_FOG);
    GLfloat fogcolor[4] = {1, 1, 1, 1};
    glFogfv(GL_FOG_COLOR, fogcolor);

    //glFogi(GL_FOG_MODE, GL_EXP);
    //glFogf(GL_FOG_DENSITY, 0.10);

    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 30);
    glFogf(GL_FOG_END, 60);

    m_textureSet.JARDIN = bindTexture(QPixmap("Ressources/herbe.jpg"), GL_TEXTURE_2D);
    shader_vao = new ShaderVAO();
}

void RenduOpenGL::onEnterFrame()
{
    QPoint pos = mapFromGlobal(QCursor::pos());
    //QPoint diff = m_lastPos - pos;

    /*if(m_camera)
    {
        m_camera->avancer( m_camera->vitesse() );

        const double sensibilite = 0.3;
        m_camera->tourner(diff.x() * sensibilite, diff.y() * sensibilite);
    }*/

    pos = QPoint(this->width()/2,this->height()/2);
    QCursor::setPos(mapToGlobal(pos));

    m_lastPos = pos;

    if(m_controle)
    {
        for(int i = 0 ; i < 6 ; i++)
            if(m_mesTouches[i]->compteur > 0)
                (m_controle->*(m_mesTouches[i]->action))();
    }

    if(m_ville && m_ville->generee())
        m_ville->enterFrame();
}

void RenduOpenGL::setPause(bool pause)
{
    if(pause)
    {
        m_timerOnEnterFrame.stop();
        setCursor(Qt::PointingHandCursor);
        releaseKeyboard();
    }
    else
    {
        if(! m_timerOnEnterFrame.isActive())
        {
            m_timerOnEnterFrame.start();
            m_lastPos = mapFromGlobal(QCursor::pos());
        }
        setCursor(Qt::BlankCursor);
        grabKeyboard();
    }
}

void RenduOpenGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if(! m_ville) {
        qDebug() << "Pas de ville pour moi!";
    } else if(! m_ville->generee()) {
        qDebug() << "Ville en generation";
    } else {
        /*glViewport(0, 0, this->width(), this->height());
        shader_vao->dessiner();*/

        glViewport(0, 0, this->width(), this->height());

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70, 1.0*this->width()/this->height(), 0.01, m_optionsDeJeu->performance.farPlane);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        m_ville->drawGL();

        if(m_modeCarte != AUCUNE_CARTE)
        {
            double x,y,w,h;
            if(m_modeCarte == PETITE)
                x = 20, y = 20, w = 300, h = 300;
            else
                x = 20, y = 20, w = this->width() - 40, h = this->height() - 40;

            glViewport(x, y, w, h);

            glDisable(GL_LIGHTING);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, w, 0, h);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslated(w/2, h/2, 0); //pixels on the screen
            glScaled(15,15,15); //pixels / unite
            //Dessiner ici le cadre en 0,0,300,300
            m_ville->drawMiniCarte();

            glEnable(GL_LIGHTING);
        }

        /*
        glDisable(GL_LIGHTING);

        glViewport(0, 0, this->width(), this->height());

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3d(0,0,1);
        glVertex3d(0.7, 0.7, 0.0);
        glVertex3d(-0.5, 0.7, 0.0);
        glVertex3d(0.1, -0.5, 0.0);
        glEnd();

        glEnable(GL_LIGHTING);

        VertexArray va;
        va.utiliserBuffer();
        va.nouvelleCouleur(1,0,0);
        va.nouvelleCouleur(1,0,0);
        va.nouvelleCouleur(1,0,0);
        va.nouvellePosition(0.8, 0.8, 0.0);
        va.nouvellePosition(-0.4, 0.8, 0.0);
        va.nouvellePosition(0.2, -0.4, 0.0);
        va.nouvelleNormale(0,0,1);
        va.nouvelleNormale(0,0,1);
        va.nouvelleNormale(0,0,1);
        va.dessiner();

        glViewport(0, 0, this->width(), this->height());
        shader_vao->dessiner();
        */
    }
}

void RenduOpenGL::mousePressEvent(QMouseEvent *)
{
    setPause( m_timerOnEnterFrame.isActive() );
}

void RenduOpenGL::keyPressEvent(QKeyEvent * event)
{
    switch(event->key())
    {
        case Qt::Key_P :
        case Qt::Key_Escape :
            setPause(true);
            break;
        case Qt::Key_M :
        case Qt::Key_Tab :
            m_modeCarte = (ModeCarte)((m_modeCarte + 1) % NOMBRE_MODE_CARTE);
			break;
		default :
            break;
    }

    if(event->isAutoRepeat())
        return;
    MapClavier::iterator it = m_keyInput.find(event->key());
    if(it != m_keyInput.end())
        it.value()->compteur++;

    if(m_controle)
    {
		switch(event->key())
		{
			case Qt::Key_Space:
				m_controle->saut();
			break;
			case Qt::Key_Return:
			case Qt::Key_Enter:
			case Qt::Key_Backspace:
				m_controle->action();
			break;
			case Qt::Key_Control:
				m_controle->action2();
			break;
		}
    }
}

void RenduOpenGL::keyReleaseEvent(QKeyEvent * event)
{
    if(event->isAutoRepeat())
        return;
    MapClavier::iterator it = m_keyInput.find(event->key());
    if(it != m_keyInput.end())
    {
        it.value()->compteur--;
        if(it.value()->compteur < 0)
            it.value()->compteur = 0;
    }
}

void RenduOpenGL::focusOutEvent(QFocusEvent *)
{
    for(MapClavier::iterator it = m_keyInput.begin() ; it != m_keyInput.end() ; ++it)
        it.value()->compteur = 0;
}
