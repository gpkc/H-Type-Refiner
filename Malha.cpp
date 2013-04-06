#include "Malha.h"
#include <queue>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace HMesh;
using namespace std;

Malha::~Malha() {
    for(map<int, Ponto*>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
        delete it->second;
    }
    for(set<Face*, FaceOperador>::iterator it = faces.begin(); it != faces.end(); ++it) {
        set<Face*, FaceOperador>::iterator it2 = it;
        it2++;
        delete *it;
        it = it2;
    }

    pontos.clear();
    faces.clear();
    Ancioes.clear();

    while(!hangFaces.empty()) {
        hangFaces.pop();
    }
}

Malha::Malha(GModel* theModel) {
    faceId = 0;
    maxId = 0;
    numPontosRemovidos = 0;

    //SÓ 2D (PARA 3D ADICIONAR TETRAS)

    //Pega a face do modelo (plane surface), insere a si mesma no set de faces, e insere seus vértices no set de vertices
    map<SPoint3, int> vertices;
    int index = 1;
    //A inserção de triângulos é feita dizendo os IDs dos vértices inseridos
    set<Triangulo*> triangulos;

    for(GModel::fiter it = theModel->firstFace(); it != theModel->lastFace(); ++it) {
        for(unsigned int i = 0; i < (*it)->getNumMeshElements(); ++i) {
            Triangulo* tri = new Triangulo;

            MElement* Element = (*it)->getMeshElement(i);

            int id = 0;
            for(int j = 0; j < Element->getNumVertices(); ++j, ++id) {
                if(vertices.insert( pair<SPoint3, int>(tri->setId(id, Element->getVertex(j)->point()), index) ).second) {
                    index++;
                }

            }
            triangulos.insert(tri);
        }
    }

    //Insere os dados no HMesh
    for(map<SPoint3, int>::iterator it = vertices.begin(); it != vertices.end(); ++it, ++index) {
        this->adiciona_ponto(it->second, it->first.x(), it->first.y(), it->first.z());
    }
    cout << "Quantidade inicial de vertices: " << this->pontos.size() << endl;

    index = 1;
    for(set<Triangulo*>::iterator it = triangulos.begin(); it != triangulos.end(); ++it, ++index) {
        this->adiciona_face( vertices.find((*it)->id1)->second, vertices.find((*it)->id2)->second, vertices.find((*it)->id3)->second);
    }
    cout << "Quantidade inicial de faces: " << this->faces.size() << endl;
}

Face* Malha::adiciona_face(int id1, int id2, int id3) {
    Face* f = new Face(id1, id2, id3, this);
    f->ID = faceId + 1;
    ++faceId;

    faces.insert(f);
    Ancioes.push_back(f);
    return f;
}

Face* Malha::adiciona_face(Ponto* id1, Ponto* id2, Ponto* id3, Face* pai) {
    Face* f = new Face(id1, id2, id3, this, pai);
    f->ID = faceId + 1;
    ++faceId;

    faces.insert(f);
    return f;
}

Ponto* Malha::adiciona_ponto(int num , double x, double y,double z) {
    if(maxId < num) {
        maxId = num;
    }
    Ponto* p = new Ponto(num, x, y, z);
    pontos.insert(pair<int, Ponto*>(num, p));
    return p;
}
Ponto* Malha::adiciona_ponto(double x, double y,double z) {
    int num = maxId + 1;
    ++maxId;

    Ponto* p = new Ponto(num, x, y, z);
    pontos.insert(pair<int, Ponto*>(num, p));
    return p;
}

Ponto* Malha::ponto(int id) {
    map<int, Ponto*>::iterator ret;
    return ((ret = pontos.find(id)) != pontos.end()) ? ret->second : NULL;
}

void Malha::Resolve() {
    set<Face*> elemEspecial; //Conjunto de faces que irão ter elemento especial

    while(!hangFaces.empty()) {
        pair<set<Face*>::iterator, const bool> has = elemEspecial.insert(hangFaces.front());

        if(!has.second) {
            if(*has.first != NULL) {
                (*has.first)->Subdivide();
                elemEspecial.erase(has.first);
            }
        }

        hangFaces.pop();
    }

    for(set<Face*>::iterator it = elemEspecial.begin(); it != elemEspecial.end(); ++it) {
        (*it)->ConectaHangNode();
    }
}

void Malha::EscreveMsh(const char* outFile) {
    ofstream out(outFile, ifstream::out | ifstream::trunc);

    out << "$MeshFormat" << endl << 2.1 << " " << 0 << " " << 8 << endl << "$EndMeshFormat" << endl << "$Nodes" << endl << pontos.size() - numPontosRemovidos << endl;

    for(map<int, Ponto*>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
        out << it->first << " " << setprecision(16) <<  it->second->get_x() << " " << it->second->get_y() << " " << it->second->get_z() << endl;
    }
    cout << "Quantidade Final de vertices: " << pontos.size() << endl;

    stringstream ss;

    int n = 0;
    for(set<Face*>::iterator it = faces.begin(); it != faces.end(); ++it) {
        if(!(*it)->deleted) {
                n++;
                ss << (*it)->ID << " " << 2 << " " << 3 << " " << 0 << " " << (*it)->ultimoPai()->ID << " " << 0 << " " << (*it)->op1->ID << " " << (*it)->op2->ID << " " << (*it)->op3->ID << endl;
        }
    }
    out << "$EndNodes" << endl << "$Elements" << endl << n << endl;
    out << ss.str();


    cout << "Quantidade Final de faces: " << n << endl;

    out << "$EndElements" << endl;

    out.close();
}

void Malha::gb2EstadoInicial() {

    faces.clear(); //Não limpa a memória!
    pontos.clear(); //Não limpa a memória!

    int id = 0;
    int fid = 0;
    for(list<Face*>::iterator it = Ancioes.begin(); it != Ancioes.end(); ++it) {
        fid = max(fid, (*it)->ID);
        faces.insert(*it);
        (*it)->deleted = false;

        (*it)->centrais[0] = NULL;
        (*it)->centrais[1] = NULL;
        (*it)->centrais[2] = NULL;

        (*it)->filhos.clear();

        id = max(id, (*it)->op1->ID);
        pontos.insert( pair<int, Ponto*>((*it)->op1->ID, (*it)->op1) );
        id = max(id, (*it)->op2->ID);
        pontos.insert( pair<int, Ponto*>((*it)->op2->ID, (*it)->op2) );
        id = max(id, (*it)->op3->ID);
        pontos.insert( pair<int, Ponto*>((*it)->op3->ID, (*it)->op3) );

//        if(((*it)->op1->ID == 30) || ((*it)->op1->ID == 11) || ((*it)->op1->ID == 43)) {
//            if(((*it)->op2->ID == 30) || ((*it)->op2->ID == 11) || ((*it)->op2->ID == 43)) {
//                if(((*it)->op3->ID == 30) || ((*it)->op3->ID == 11) || ((*it)->op3->ID == 43)) {
//                    throw;
//                }
//            }
//        }
    }
    faceId = fid;
    maxId = id;

    for(map<int, Ponto*>::iterator it = pontos.begin(); it != pontos.end(); ++it) {
        for(list<Face*>::iterator it2 = it->second->faces.begin(); it2 != it->second->faces.end(); ++it2) {
            if(faces.find(*it2) == faces.end()) {
                list<Face*>::iterator it3 = it2;
                --it3;
                it->second->faces.erase(it2);
                it2 = it3;
            }
        }
    }

    for(list<Face*>::iterator it = Ancioes.begin(); it != Ancioes.end(); ++it) {
        for(set<Face*>::iterator it2 = (*it)->vizinhos.begin(); it2 != (*it)->vizinhos.end(); ++it2) {
            if(faces.find(*it2) == faces.end()) {
                set<Face*>::iterator it3 = it2;
                --it3;
                (*it)->vizinhos.erase(it2);
                it2 = it3;
            }
        }
    }

}

//void Malha::gb2EstadoInicial() {
//
//    for(list<Face*>::iterator it = Ancioes.begin(); it != Ancioes.end(); ++it) {
//
//        for(vector<Face*>::iterator it2 = (*it)->filhos.begin(); it2 != (*it)->filhos.end(); ++it2) {
//            printf("%d\n", (*it2)->vizinhos.size());
//            (*it2)->Destroy();
//        }
//        //Centrais do ancião
//        for(vector<Ponto*>::iterator it2 = (*it)->centrais.begin(); it2 != (*it)->centrais.end(); ++it2) {
//            if(*it2)
//                delPontos.insert(*it2);
//            //pontos.erase((*it)->ID);
//                //delete *it;
//            *it2 = NULL;
//        }
//        if((*it)->deleted) {
//            (*it)->deleted = false;
//        }
//
//    }
//    for(set<Ponto*>::iterator it = delPontos.begin(); it != delPontos.end(); ++it) {
//        map<int, Ponto*>::iterator it2 = pontos.find((*it)->ID);
//        if(it2 != pontos.end())
//            pontos.erase(it2);
//            delete *it;
//    }
//    for(set<Face*>::iterator it = delFaces.begin(); it != delFaces.end(); ++it) {
//        delete *it;
//    }
//
//}
