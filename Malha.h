#ifndef MALHA_H_INCLUDED
#define MALHA_H_INCLUDED

#include <map>
#include <string>
#include <queue>
#include "Face.h"
#include "Ponto.h"

//GMSH
#include "gmsh/Gmsh.h"
#include "gmsh/GModel.h"
#include "gmsh/GEntity.h"
#include "gmsh/MElement.h"
#include "gmsh/MVertex.h"
#include "gmsh/SPoint3.h"

using namespace std;

namespace HMesh {

class Malha;
class FaceOperador;

class Face {
private:

//    double length1;
//    double length2;
//    double length3;

    //MAd::MeshAdapter* pAdapt;
    Malha* hMesh;
protected:

    Ponto* op1; //Vértices
    Ponto* op2;
    Ponto* op3;

    int Nivel;

    vector<Ponto*> centrais;

    vector<Face*> filhos; //Para desrefinamento
    Face* pai;

public:
    bool deleted; //Deletada
    int ID;

    friend class HTypeManager;
    friend class Malha;
    //Face(MAd::pFace face, int nivel, MAd::MeshAdapter* adapter, MAd::pMesh mesh);
    //Face(const Face&);
    Face(int, int, int, Malha*); //IDS
    Face(Ponto*, Ponto*, Ponto*, Malha*, Face*);
    ~Face();

    vector<Face*>* Subdivide();
    bool ConectaHangNode();
    bool Desrefina();

    inline bool operator < (const Face & other) const
    {
        if(ID < other.ID)
            return true;
        else
            return false;
    }

    class FaceOperador { //Operador para containers
    public:
        bool operator()(const Face* ent1, const Face* ent2) const
        {
          return *ent1 < *ent2;
        }
    };

    set<Face*, FaceOperador> vizinhos;

    bool temPonto(Ponto*);

    void Centro_f(double xyz[]);

    Face* ultimoPai();
    bool parente(Face*); //Se possui algum parentesco (paî, avô, etc) em relação ao argumento

    unsigned int Profundidade(); //Profundidade de refinamento (quantas vezes esse elemento foi refinado)
    int Level(); //Nível de refinamento (quantas vezes o ancião foi refinado até chegar nesse elemento)
    bool Anciao(); //Raiz

    void Destroy();
};

class FaceOperador { //Operador para containers
public:
    bool operator()(const Face* ent1, const Face* ent2) const
    {
      return *ent1 < *ent2;
    }
};

class Triangulo {
public:
    SPoint3 id1;
    SPoint3 id2;
    SPoint3 id3;

    SPoint3 setId(int id, SPoint3 ponto) {
        if(id == 0) {
            id1 = ponto;
        }
        else if(id == 1)  {
            id2 = ponto;
        }
        else {
            id3 = ponto;
        }

        return ponto;
    }
};

typedef set<Face*, FaceOperador>* Faces;
typedef set<Face*, FaceOperador>::iterator FacesIter;

class Malha {

//Atributos privados
private:
    int maxId; //Maior ID
    int faceId;

//Métodos privados
private:
    Ponto* adiciona_ponto(int num , double x, double y,double z);
    Ponto* adiciona_ponto(double x, double y,double z);
    Face* adiciona_face(int id1, int id2, int id3);
    Face* adiciona_face(Ponto* id1, Ponto* id2, Ponto* id3, Face* pai);

protected:
    map<int, Ponto*> pontos;
    list<Face*> Ancioes; //Faces anciãs
    set<Face*, FaceOperador> faces;
    queue<Face*> hangFaces; //Faces com hanging node
    int numPontosRemovidos;
//    set<Ponto*> delPontos;
//    set<Face*> delFaces;


public:
    friend class Face;
    Malha(GModel* theModel);
    ~Malha();

    Ponto* ponto(int id);

    void Resolve(); //Resolve hanging nodes

    void EscreveMsh(const char*);

    Faces getFaces() { return &faces; }

    void gb2EstadoInicial();
};


}

#endif // MALHA_H_INCLUDED
