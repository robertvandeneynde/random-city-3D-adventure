#ifndef IMPORTATEUROBJ_H
#define IMPORTATEUROBJ_H

#include <vector>

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QHash>
#include <QStringList>

#include "VertexArray.h"

class ImportateurObj
{
    public :
        ImportateurObj(QString fullFileName, QString mtlLocation = "");
        ~ImportateurObj();

        bool erreurOuvertureFichier() { return m_erreurOuvertureFichier; }

        void utiliserBuffer(bool = true); //appelle la fonction de tous les VA actuellement dedans
        void dessiner(); //Fait un rendu de tous les objets

        VertexArray * operator[](QString nom) { return objet(nom); }
        VertexArray * objet(QString nom); //If not found return NULL
        VertexArray * enlever(QString nom); //Doit etre deleted //If not found return NULL
        VertexArray * enleverAffichage(QString nom); //If not found return NULL
    private :
        struct Material
        {
            std::vector<float> ambiant;
            std::vector<float> diffuse;
            std::vector<float> specular;
            double specularCoefficient;
            float transparency;
            float illumination;

            std::vector<float> diffuseTransparent() const;
        };
    private :
        void chargerMtl(QString fileName);
    public :
        //Format chaine
        static bool formatStr(QString ligne, QString nom, QString &);

        //Format vecteur. v sera de taille maxi au finals
        template <typename T>
        static bool formatVec(QString ligne, QString nom,
                              std::vector<T> & v,
                              size_t obligatoires,
                              size_t maxi,
                              T defaut = 0);
        //Convenience functions
        template <typename T>
        static bool formatNum(QString ligne, QString nom, T & v);
        static bool formatTex(QString ligne, QString nom, std::vector<double> & texture);
        static bool formatVer(QString ligne, QString nom, std::vector<double> & vertex);
        static bool formatNor(QString ligne, QString nom, std::vector<double> & normale);
        static bool formatCoul(QString ligne, QString nom, std::vector<float> & couleur);

        //Format faces. Les tableaux seront tous du nombre de points utilises dans la face
        static bool formatFac(QString ligne, QString nom,
                              std::vector<size_t> & vertices,
                              std::vector<size_t> & textureCoordinates,
                              std::vector<size_t> & normales,
                              size_t defautIndice);
    private :
        typedef QHash<QString, Material*> MapMat;
        typedef QHash<QString, VertexArray *> MapObj;

    private :
        bool m_erreurOuvertureFichier;

        MapObj m_objets;
        MapObj m_objetsAAfficher;
        MapMat m_materials;

        VertexArray * m_vertexArray;
        Material    * m_materialActif;

        std::vector< std::vector<double> > m_positions;
        std::vector< std::vector<double> > m_normales;

        QFile m_fichierLog;
        QTextStream log;
};

template <typename T>
bool ImportateurObj::formatVec(QString ligne, QString nom, std::vector<T> & v, size_t obligatoires, size_t maxi, T defaut)
{
    v.resize(maxi);
    QString nomLigne;
    QTextStream stream(&ligne);
    stream >> nomLigne;
    if(nomLigne == nom)
    {
        size_t i = 0;
        while(i < obligatoires)
            stream >> v[i++];
        while(i < maxi)
        {
            stream >> v[i++];
            if(stream.status() != QTextStream::Ok)
                while(i < maxi)
                    v[i++] = defaut;
        }
        return true;
    }
    return false;
}

template <typename T>
bool ImportateurObj::formatNum(QString ligne, QString nom, T & num)
{
    std::vector<T> v;
    bool match = formatVec(ligne, nom, v, 1, 1);
    if(match)
        num = v.front();
    return match;
}

#endif // IMPORTATEUROBJ_H
