#ifndef REMPLISSEURDESCENE_H
#define REMPLISSEURDESCENE_H

#include "Dessinateur.h"
#include "VertexArray.h"
#include "TextureSet.h"

class RemplisseurDeScene
{
    public:
        RemplisseurDeScene(VertexArray & va, Dessinateur & d, TextureSet & ts) : m_vertexArray(va), m_dessinateur(d), m_textureSet(ts) {}

        VertexArray & vertexArray() { return m_vertexArray; }
        Dessinateur & dessinateur() { return m_dessinateur; }
        TextureSet const & textureSet() { return m_textureSet; }
    private:
        VertexArray & m_vertexArray;
        Dessinateur & m_dessinateur;
        TextureSet & m_textureSet;
};

#endif // REMPLISSEURDESCENE_H
