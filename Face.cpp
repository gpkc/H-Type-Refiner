#include "Face.h"

using namespace HMesh;
using namespace std;

Face::~Face() {
    centrais.clear();
    filhos.clear();
}

bool Face::parente(Face* filho) {
    Face* aux = filho->pai;

    while(aux != NULL) {
        if(aux == this) {
            return true;
        }
        aux = aux->pai;
    }
    return false;
}

unsigned int Face::Profundidade() {
    if(filhos.empty()) { //Se não tiver filhos, possui nível de refinamento 0
        return 0;
    }
    else {
        unsigned int p = 0;
        for(vector<Face*>::iterator it = filhos.begin(); it != filhos.end(); ++it) {
            p = max(p, (*it)->Profundidade());
        }
        return p+1;
    }
}

int Face::Level() {
    Face* aux = this;
    int l = 0;
    //Se for o ancião, l = 0
    while(aux->pai != NULL) {
        aux = aux->pai;
        ++l;
    }

    return l;
}

bool Face::Anciao() {
    return this->pai == NULL;
}

void Face::Destroy() {

    for(set<Face*>::iterator it = vizinhos.begin(); it != vizinhos.end(); ++it) {
         (*it)->vizinhos.erase(this);
    }
    for(vector<Face*>::iterator it = filhos.begin(); it != filhos.end(); ++it) {
        (*it)->Destroy();
    }
    for(vector<Ponto*>::iterator it = centrais.begin(); it != centrais.end(); ++it) {
        if(*it)
//            hMesh->delPontos.insert(*it);
        //hMesh->pontos.erase((*it)->ID);
            //delete *it;
        *it = NULL;
    }

//    if(hMesh->faces.erase(this))
//        hMesh->delFaces.insert(this);
}

Face::Face(int id1, int id2, int id3, Malha* m) {

    deleted = false;
    pai = NULL;
    hMesh = m;

    centrais.resize(3, NULL);

    op1 = m->ponto(id1);
    op2 = m->ponto(id2);
    op3 = m->ponto(id3);

    for(list<Face*>::iterator it = op1->faces.begin(); it != op1->faces.end(); ++it) {
        int Vertices=1; //Vértices iguais (2 vértices = face adjacente)
        if( (*it)->temPonto(op2) ) Vertices++;
        if( (*it)->temPonto(op3) ) Vertices++;

        if(Vertices == 2) {
            vizinhos.insert(*it);
            (*it)->vizinhos.insert(this);
        }
    }

    for(list<Face*>::iterator it = op2->faces.begin(); it != op2->faces.end(); ++it) {
        int Vertices=1; //Vértices iguais (2 vértices = face adjacente)
        if( (*it)->temPonto(op1) ) Vertices++;
        if( (*it)->temPonto(op3) ) Vertices++;

        if(Vertices == 2) {
            vizinhos.insert(*it);
            (*it)->vizinhos.insert(this);
        }
    }

    for(list<Face*>::iterator it = op3->faces.begin(); it != op3->faces.end(); ++it) {
        int Vertices=1; //Vértices iguais (2 vértices = face adjacente)
        if( (*it)->temPonto(op1) ) Vertices++;
        if( (*it)->temPonto(op2) ) Vertices++;

        if(Vertices == 2) {
            vizinhos.insert(*it);
            (*it)->vizinhos.insert(this);
        }
    }

    op1->faces.push_back(this);
    op2->faces.push_back(this);
    op3->faces.push_back(this);

}

Face::Face(Ponto* id1, Ponto* id2, Ponto* id3, Malha* m, Face* pai) {
    if((id1->ID == 30) || (id1->ID == 11) || (id1->ID == 43)) {
        if((id2->ID == 30) || (id2->ID == 11) || (id2->ID == 43)) {
            if((id3->ID == 30) || (id3->ID == 11) || (id3->ID == 43)) {
                //throw;
            }
        }
    }

    deleted = false;
    hMesh = m;
    this->pai = pai;

    op1 = id1;
    op2 = id2;
    op3 = id3;

    centrais.resize(3, NULL);

    for(list<Face*>::iterator it = op1->faces.begin(); it != op1->faces.end(); ++it) {
        int Vertices=1; //Vértices iguais (2 vértices = face adjacente)
        if( (*it)->temPonto(op2) ) Vertices++;
        if( (*it)->temPonto(op3) ) Vertices++;

        if(Vertices == 2) {
            if( !((this->parente(*it)) || ((*it)->parente(this))) ) {
                vizinhos.insert(*it);
                (*it)->vizinhos.insert(this);
            }
        }
    }

    for(list<Face*>::iterator it = op2->faces.begin(); it != op2->faces.end(); ++it) {
        int Vertices=1; //Vértices iguais (2 vértices = face adjacente)
        if( (*it)->temPonto(op1) ) Vertices++;
        if( (*it)->temPonto(op3) ) Vertices++;

        if(Vertices == 2) {
            if( !((this->parente(*it)) || ((*it)->parente(this))) ) {
                vizinhos.insert(*it);
                (*it)->vizinhos.insert(this);

            }
        }
    }

    for(list<Face*>::iterator it = op3->faces.begin(); it != op3->faces.end(); ++it) {
        int Vertices=1; //Vértices iguais (2 vértices = face adjacente)
        if( (*it)->temPonto(op1) ) Vertices++;
        if( (*it)->temPonto(op2) ) Vertices++;

        if(Vertices == 2) {
            if( !((this->parente(*it)) || ((*it)->parente(this))) ) {
                vizinhos.insert(*it);
                (*it)->vizinhos.insert(this);
            }
        }
    }

    op1->faces.push_back(this);
    op2->faces.push_back(this);
    op3->faces.push_back(this);

}

bool Face::temPonto(Ponto* p) {
    if(op1 == p) return true;
    else if(op2 == p) return true;
    else if(op3 == p) return true;
    else return false;
}

void Centro(Ponto* p1, Ponto* p2, double centro[3]) {

    centro[0] = 0.5 * ( p1->get_x() + p2->get_x() );
    centro[1] = 0.5 * ( p1->get_y() + p2->get_y() );
    centro[2] = 0.5 * ( p1->get_z() + p2->get_z() );
}
void Face::Centro_f(double xyz[]) {

    double fxyz[3][3];

    fxyz[0][0] = op1->get_x();
    fxyz[0][1] = op1->get_y();
    fxyz[0][2] = op1->get_z();
    fxyz[1][0] = op2->get_x();
    fxyz[1][1] = op2->get_y();
    fxyz[1][2] = op2->get_z();
    fxyz[2][0] = op3->get_x();
    fxyz[2][1] = op3->get_y();
    fxyz[2][2] = op3->get_z();

    double fact = 1./3.0;
    for (int k=0;k<3;k++) xyz[k] = 0;
    for (int i=0;i<3;i++) {
        for (int k=0;k<3;k++) xyz[k] += fact * fxyz[i][k];
    }
}

Face* Face::ultimoPai() {
    Face* retorno = this;
    while(retorno->pai != NULL)
        retorno = retorno->pai;

    return retorno;
}

vector<Face*>* Face::Subdivide() {
    if(deleted) return NULL;

    double centro[3];

    //Ponto* p1, *p2, *p3;
    Face* f1, *f2, *f3, *f4;

    this->deleted = true;

    if(!centrais[0]) {
        Centro(op1, op2, centro);
        centrais[0] = hMesh->adiciona_ponto(centro[0], centro[1], centro[2]);
    }
    else {
        //Tira da lista de hang
    }
    if(!centrais[1]) {
        Centro(op2, op3, centro);
        centrais[1] = hMesh->adiciona_ponto(centro[0], centro[1], centro[2]);
    }
    else {
        //Tira da lista de hang
    }
    if(!centrais[2]) {
        Centro(op1, op3, centro);
        centrais[2] = hMesh->adiciona_ponto(centro[0], centro[1], centro[2]);
    }
    else {
        //Tira da lista de hang
    }

    f1 = hMesh->adiciona_face(centrais[0], centrais[1], centrais[2], this);
    filhos.push_back(f1);

    f2 = hMesh->adiciona_face(centrais[0], centrais[1], op2, this);
    filhos.push_back(f2);

    f3 = hMesh->adiciona_face(centrais[0], centrais[2], op1, this);
    filhos.push_back(f3);

    f4 = hMesh->adiciona_face(centrais[1], centrais[2], op3, this);
    filhos.push_back(f4);

    for(set<Face*>::iterator it = vizinhos.begin(); it != vizinhos.end(); ++it) {
        if(op1 == (*it)->op1) {
            if(op2 == (*it)->op2) {
                (*it)->centrais[0] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op2 == (*it)->op3) {
                (*it)->centrais[2] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op2) {
                (*it)->centrais[0] = centrais[2];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op3) {
                (*it)->centrais[2] = centrais[2];
                hMesh->hangFaces.push(*it);
            }
        }
        else if(op1 == (*it)->op2) {
            if(op2 == (*it)->op1) {
                (*it)->centrais[0] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op2 == (*it)->op3) {
                (*it)->centrais[1] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op1) {
                (*it)->centrais[0] = centrais[2];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op3) {
                (*it)->centrais[1] = centrais[2];
                hMesh->hangFaces.push(*it);
            }
        }
        else if(op1 == (*it)->op3) {
            if(op2 == (*it)->op1) {
                (*it)->centrais[2] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op2 == (*it)->op2) {
                (*it)->centrais[1] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op1) {
                (*it)->centrais[2] = centrais[2];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op2) {
                (*it)->centrais[1] = centrais[2];
                hMesh->hangFaces.push(*it);

            }
        }
        else if(op2 == (*it)->op1) {
            if(op1 == (*it)->op2) {
                (*it)->centrais[0] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op1 == (*it)->op3) {
                (*it)->centrais[2] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op2) {
                (*it)->centrais[0] = centrais[1];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op3) {
                (*it)->centrais[2] = centrais[1];
                hMesh->hangFaces.push(*it);
            }
        }
        else if(op2 == (*it)->op2) {
            if(op1 == (*it)->op1) {
                (*it)->centrais[0] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op1 == (*it)->op3) {
                (*it)->centrais[1] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op1) {
                (*it)->centrais[0] = centrais[1];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op3) {
                (*it)->centrais[1] = centrais[1];
                hMesh->hangFaces.push(*it);
            }
        }
        else if(op2 == (*it)->op3) {
            if(op1 == (*it)->op1) {
                (*it)->centrais[2] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op1 == (*it)->op2) {
                (*it)->centrais[1] = centrais[0];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op1) {
                (*it)->centrais[2] = centrais[1];
                hMesh->hangFaces.push(*it);
            }
            else if(op3 == (*it)->op2) {
                (*it)->centrais[1] = centrais[1];
                hMesh->hangFaces.push(*it);
            }
        }
    }

    return &filhos;
}

bool Face::ConectaHangNode() {
    if(deleted) return false;

    this->deleted = true;

    if(centrais[0]) {
        filhos.push_back(hMesh->adiciona_face(centrais[0], op3, op2, this));

        filhos.push_back(hMesh->adiciona_face(centrais[0], op3, op1, this));
    }
    else if(centrais[1]) {
        filhos.push_back(hMesh->adiciona_face(centrais[1], op1, op2, this));

        filhos.push_back(hMesh->adiciona_face(centrais[1], op1, op3, this));
    }
    else if(centrais[2]) {
        filhos.push_back(hMesh->adiciona_face(centrais[2], op2, op3, this));

        filhos.push_back(hMesh->adiciona_face(centrais[2], op2, op1, this));
    }


    return true;
}
