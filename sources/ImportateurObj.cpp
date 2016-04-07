#include "ImportateurObj.h"

#include <QString>
#include <QDebug>
#include <stdexcept>
#include <cassert>

#include "Algos.h"

using namespace std;

ImportateurObj::ImportateurObj(QString fullFileName, QString mtlLocation)
    : m_fichierLog(fullFileName + ".log")
    , log(&m_fichierLog)
{
    m_fichierLog.open(QIODevice::WriteOnly | QIODevice::Text);

    QFile fichier(fullFileName);
    m_erreurOuvertureFichier = ! fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    if(m_erreurOuvertureFichier)
        return;

    const double defautPos[4] = {0.0, 0.0, 0.0, 1.0};
    const double defautNor[3] = {0.0, 0.0, 1.0};
    m_positions.assign(1, vector<double>(defautPos, defautPos + 4) );
    m_normales.assign(1, vector<double>(defautNor, defautNor + 3) );
    //Taille = 1 de telle sorte que les indices .obj correspondent aux indices C
    //De plus m_positions[0] et m_normales[0] sont des valeurs par défaut lors par exemple de f 1 2/2 3//3

    m_vertexArray = NULL;
    m_materialActif = NULL;
    QTextStream streamFichier(&fichier);

    QString nom;
    vector<size_t> vertices, normales, texturesCoordinate;
    vector<double> position(4);
    vector<double> normale(3);
    while(true)
    {
        QString ligne = streamFichier.readLine();

        if(ligne.isNull())
            break;
        //L'odre de verification est classe par probabilite d'apparation
        //Remarque que les commentaires sont bien ignorés car ils n'appartiennt à aucun des formats disponibles
        else if(formatFac(ligne, "f", vertices, texturesCoordinate, normales, 0))
        {
            if(m_vertexArray)
            {
                //assert(vertices.size() <= 3) // == 3 car on a triangulisé les faces
                for(size_t i = 0 ; i < vertices.size() ; i++)
                {
                    try {
                        m_vertexArray->nouvellePosition( m_positions.at( vertices[i] ) );
                        m_vertexArray->nouvelleNormale( m_normales.at( normales[i] ) );
                        m_vertexArray->nouvelleCouleur( m_materialActif ? m_materialActif->diffuseTransparent() : vector<float>(4, 1.0) );
                    }
                    catch(std::out_of_range &) {
                        log << "ImportateurObj::ImportateurObj(QString fileName) : vertex out of range" << endl;
                    }
                }
            }
        }
        else if(formatTex(ligne, "v", position))
        {
            m_positions.push_back( position );
        }
        else if(formatNor(ligne, "vn", normale))
        {
            m_normales.push_back( normale );
        }
        else if(formatStr(ligne, "usemtl", nom))
        {
            MapMat::const_iterator iterateurMat = m_materials.find(nom);
            if(iterateurMat != m_materials.end())
                m_materialActif = iterateurMat.value();
        }
        else if(formatStr(ligne, "o", nom) || formatStr(ligne, "g", nom))
        {
            MapObj::iterator it = m_objets.find(nom); //On va remplir le obj indiqué par nom
            if(it == m_objets.end()) //S'il nexiste pas.
                it = m_objets.insert(nom, new VertexArray); //On le cree
            m_vertexArray = it.value(); //Il devient le obj actif
            m_vertexArray->setModeAffichage(GL_TRIANGLES);
        }
        else if(formatStr(ligne, "mtllib", nom))
        {
           chargerMtl(mtlLocation + nom);
        }
    }
    m_objetsAAfficher = m_objets;
    size_t lengthTot = 0, weightTot = 0;
    log << "-----------------------------------" << endl;
    log << "||" << fullFileName << " bien cree :" << endl;
    for(MapObj::iterator it = m_objets.begin() ; it != m_objets.end() ; ++it)
    {
        log << "|-" << it.key() << "->" << it.value()->length() << "sommets" << "(" << Algos::strOctets(it.value()->weight()) << ")" << endl;
        lengthTot += it.value()->length();
        weightTot += it.value()->weight();
    }
    log << "|>" << m_objets.size() << "objets ont ete crees" << "pour un total de " << lengthTot << "sommets" << "(" << Algos::strOctets(weightTot) << ")" << endl;
    m_fichierLog.close();
}

ImportateurObj::~ImportateurObj()
{
    for(MapMat::iterator it = m_materials.begin() ; it != m_materials.end() ; ++it)
        delete it.value();
    for(MapObj::iterator it = m_objets.begin() ; it != m_objets.end() ; ++it)
        delete it.value();
}

void ImportateurObj::chargerMtl(QString fileName)
{
    QFile fichier(fileName);
    if(! fichier.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString ligne;
    QTextStream streamFichier(&fichier);

    Material * enDefinition = NULL;

    QString nomMaterial;
    while(true)
    {
        ligne = streamFichier.readLine();
        if(ligne.isNull())
            break;

        if(formatStr(ligne, "newmtl", nomMaterial))
            m_materials.insertMulti(nomMaterial, enDefinition = new Material);

        if(enDefinition)
        {
            //Remarquons que ces if sont imbriqués. ainsi on ne passe au suivant que si la valeur n'a pas été assignée
            if(! formatCoul(ligne, "Kd", enDefinition->diffuse))
            if(! formatCoul(ligne, "Ka", enDefinition->ambiant))
            if(! formatCoul(ligne, "Ks", enDefinition->specular))
            if(! formatNum(ligne, "Ns", enDefinition->specularCoefficient))
            if(! formatNum(ligne, "Tr", enDefinition->transparency))
            if(! formatNum(ligne, "d", enDefinition->transparency))
            if(! formatNum(ligne, "illum", enDefinition->illumination))
			{}
        }
    }
}

std::vector<float> ImportateurObj::Material::diffuseTransparent() const
{
    std::vector<float> vec = diffuse;
    vec.push_back(transparency);
    return vec;
}

bool ImportateurObj::formatTex(QString ligne, QString nom, std::vector<double> & texture) {
    return formatVec(ligne, nom, texture, 1, 3, 0.0);
}

bool ImportateurObj::formatVer(QString ligne, QString nom, std::vector<double> & vertex) {
    return formatVec(ligne, nom, vertex, 3, 4, 1.0);
}

bool ImportateurObj::formatNor(QString ligne, QString nom, std::vector<double> & normale) {
    return formatVec(ligne, nom, normale, 3, 3);
}

bool ImportateurObj::formatCoul(QString ligne, QString nom, std::vector<float> & couleur) {
    return formatVec(ligne, nom, couleur, 3, 3);
}

bool ImportateurObj::formatStr(QString ligne, QString nom, QString & nomMtl)
{
    QString nomLigne;
    QTextStream stream(&ligne);
    stream >> nomLigne;
    if(nomLigne == nom)
    {
        stream >> nomMtl;
        return true;
    }
    return false;
}

bool ImportateurObj::formatFac(QString ligne, QString nom,
    vector<size_t> & vertices,
    vector<size_t> & textureCoordinates,
    vector<size_t> & normales,
    size_t defautIndice)
{
    QString nomLigne;
    QTextStream stream(&ligne);
    stream >> nomLigne;
    if(nomLigne == nom)
    {
        vector<size_t> * tableaux[3] = {&vertices, &textureCoordinates, &normales};
        for(int i = 0 ; i < 3 ; i++)
            tableaux[i]->clear();

        //Exemple : ligne = "f 1/2/3 4/5/6 7/8/9 10 11/12 13//14 15// //"
        while(true)
        {
            QString str;
            stream >> str;
            if(stream.status() != QTextStream::Ok)
                break;

            // Ex : str = "1/2/3" -> "4/5/6" -> "7/8/9" -> "10" ->
            //            "11/12" -> "13//14" -> "//15" -> "//"

            QStringList list = str.split('/');

            // Ex : list = {"1","2","3"} -> {"4","5","6"} -> {"7","8","9"}} -> -> {"10"}
            //             {"11","12"} -> {"13","","14"}} -> {"","","15"} -> {"","",""}

            while(list.size() < 3)
                list << "";

            // Ex : list = {"1","2","3"} -> {"4","5","6"} -> {"7","8","9"} -> {"10","",""} ->
            //             {"11","12",""} -> {"13","","14"} -> {"","","15"} -> {"","",""}

            for(int i = 0 ; i < 3 ; i++)
                tableaux[i]->push_back(list[i].isEmpty() ? defautIndice : list[i].toUInt());
        }
        //Ex : tableaux = {1,4,7,10,11,13}, {2,5,8,def,12,def}, {3,6,9,def,def,14}
        return true;
    }
    return false;
}

void ImportateurObj::utiliserBuffer(bool val)
{
    for(MapObj::const_iterator it = m_objets.begin() ; it != m_objets.end() ; ++it)
        it.value()->utiliserBuffer(val);
}

void ImportateurObj::dessiner()
{
    for(MapObj::const_iterator it = m_objetsAAfficher.begin() ; it != m_objetsAAfficher.end() ; ++it)
        it.value()->dessiner();
}

VertexArray * ImportateurObj::objet(QString nom)
{
    MapObj::const_iterator it = m_objets.find(nom);
    return it == m_objets.end() ? NULL : it.value();
}

VertexArray * ImportateurObj::enleverAffichage(QString nom)
{
    MapObj::iterator it = m_objets.find(nom);
    VertexArray * res = NULL;
    if(it != m_objets.end())
    {
        res = it.value();
        MapObj::iterator it2 = m_objetsAAfficher.find(nom);
        if(it2 != m_objetsAAfficher.end())
            m_objetsAAfficher.erase(it2);
    }
    return res;
}
