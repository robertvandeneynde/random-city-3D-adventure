#-------------------------------------------------
#
# Project created by QtCreator 2012-04-03T22:54:06
#
#-------------------------------------------------

QT       += widgets opengl

TARGET = GenerateurDeVille3D
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    RenduOpenGL.cpp \
    VertexArray.cpp \
    Ville.cpp \
    Bloc.cpp \
    ImportateurObj.cpp \
    fonctionsGl.cpp \
    Element.cpp \
    fonctionsMathematiques.cpp \
    CameraFreeFly.cpp \
    Helico.cpp \
    Mustang.cpp \
    ElementRotatif.cpp \
    Vehicule.cpp \
    StructZone.cpp \
    NoeudDijkstra.cpp \
    GPS.cpp \
    Perso.cpp \
    ObjetControlable.cpp \
    Chargement.cpp \
    Dessinateur.cpp \
    VoiturePNJ.cpp \
    OscillateurHarmonique.cpp \
    TextureSet.cpp \
    RemplisseurDeScene.cpp \
    DessinateurCarte.cpp \
    Parametres.cpp \
    BoutonCouleur.cpp \
    Algos.cpp \
    Voiture.cpp \
    ObjetScene.cpp \
    SousVille.cpp \
    Zone.cpp \
    BoulePeinture.cpp

HEADERS  += MainWindow.h \
    RenduOpenGL.h \
    ListeChainee.h \
    VertexArray.h \
    Ville.h \
    Bloc.h \
    fonctionsMathematiques.h \
    VilleInterface.h \
    StructZone.h \
    Inter.h \
    Parametres.h \
    ImportateurObj.h \
    fonctionsGl.h \
    Element.h \
    CameraFreeFly.h \
    Helico.h \
    Mustang.h \
    ElementRotatif.h \
    Vehicule.h \
    NoeudDijkstra.h \
    GPS.h \
    Perso.h \
    ObjetControlable.h \
    Tas.h \
    Chargement.h \
    Dessinateur.h \
    VoiturePNJ.h \
    OscillateurHarmonique.h \
    TextureSet.h \
    RemplisseurDeScene.h \
    DessinateurCarte.h \
    BoutonCouleur.h \
    Algos.h \
    Voiture.h \
    ObjetScene.h \
    SousVille.h \
    Zone.h \
    BoulePeinture.h

LIBS += -lGL -lGLU

CONFIG += c++11

QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-reorder

# QMAKE_CXXFLAGS += -std=c++0x
# QMAKE_CXXFLAGS += -std=gnu++0x
# CXXFLAGS="-std=c++0x" ./configure #For Qt

FORMS    += MainWindow.ui \
    Chargement.ui
