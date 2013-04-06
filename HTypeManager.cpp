#include "HTypeManager.h"

using namespace HMesh;

Malha* HTypeManager::hMesh;
map<Face*, int, FaceOperador> HTypeManager::mapaNiveis;
queue< pair<Face*, int> > HTypeManager::faces;

void HTypeManager::Initialize(Malha* hMesh) {
    HTypeManager::hMesh = hMesh;
}

void HTypeManager::Insert_Uneven(Face* face, int nivel) {
    mapaNiveis.insert(pair<Face*, int>(face, nivel));
}

void HTypeManager::Insert(Face* face, int nivel) {
    faces.push(pair<Face*, int>(face, nivel));
}


void HTypeManager::Regularize() {
    list<Face*> elementos;

    for(map<Face*, int, FaceOperador>::iterator it = mapaNiveis.begin(); ; it++) { //Itera sobre elementos
        if(it == mapaNiveis.end()) break;
        elementos.push_back(it->first);
    }

    while(!elementos.empty()) {
        if(elementos.front() == mapaNiveis.end()->first) {
            elementos.pop_front();
            continue;
        }

        map<Face*, int, FaceOperador>::iterator it = mapaNiveis.find(elementos.front());

        if(it == mapaNiveis.end()) {
            elementos.pop_front();
            continue;
        }
        if(it->first->deleted) {
            elementos.pop_front();
            mapaNiveis.erase(it);
            continue;
        }

        bool modificado = false;
        int nivelMaior = 0;

        //Faces adjacentes (desnível unitário)
        for(set<Face*>::iterator it2 = (it->first)->vizinhos.begin(); it2 != (it->first)->vizinhos.end(); ++it2) {
            int nivel;
            Face* pf = *it2;

            map<Face*, int>::iterator adj = mapaNiveis.find(pf);
            if(adj == mapaNiveis.end()) continue;
            if(it->second < (nivel = adj->second - 1)) {
                if(nivel > nivelMaior) nivelMaior = nivel;
                modificado = true;
                it->second = nivelMaior;
            }
        }

        //Faces adjacentes (Regularização)
        int n = 0;
        for(set<Face*>::iterator it2 = (it->first)->vizinhos.begin(); it2 != (it->first)->vizinhos.end(); ++it2) {
            int nivel;
            Face* pf = *it2;

            map<Face*, int>::iterator adj = mapaNiveis.find(pf);
            if(adj == mapaNiveis.end()) continue;
            if(adj->first->deleted) continue;

            //cout << (it->first)->vizinhos.size() << endl;

            if(it->second == (nivel = adj->second - 1)) {
                n++;
                if(n>1) {
                    it->second = nivel+1;
                    modificado = true;
                    break;
                }
            }
        }

        if(modificado) {

            //Faces das faces adjacentes
            for(set<Face*>::iterator it2 = (it->first)->vizinhos.begin(); it2 != (it->first)->vizinhos.end(); ++it2) {
                Face* pf = *it2;

                map<Face*, int>::iterator desnivelado;

                if( ( desnivelado = mapaNiveis.find(pf) )->second + 1 < it->second) {
                    if(desnivelado == mapaNiveis.end()) continue;
                    bool insere = true;
                    for(list<Face*>::iterator it = elementos.begin(); it != elementos.end(); it++) { //Se não já estiver na fila
                        if(*it == desnivelado->first) {
                            insere = false;
                            break;
                        }
                    }
                    if(insere) {
                        if(!desnivelado->first->deleted)
                            elementos.push_back(desnivelado->first);
                    }

                }
            }

            //Verifica faces adjacentes às faces adjacentes (regularização)
            for(set<Face*>::iterator it2 = (it->first)->vizinhos.begin(); it2 != (it->first)->vizinhos.end(); ++it2) {
                Face* pf = *it2;

                map<Face*, int>::iterator atual = mapaNiveis.find(pf);
                if(atual == mapaNiveis.end()) continue;

                int n = 0;

                //itera sobre as faces adjacentes à face adjacente
                for(set<Face*>::iterator it3 = pf->vizinhos.begin(); it3 != pf->vizinhos.end(); ++it3) {
                    Face* pf2 = *it3;

                    if(atual->second == (mapaNiveis.find(pf2)->second - 1)) {
                        n++;
                        if(n>1) {
                            bool insere = true;
                            for(list<Face*>::iterator it = elementos.begin(); it != elementos.end(); it++) { //Se não já estiver na fila
                                if(*it == atual->first) {
                                    insere = false;
                                    break;
                                }
                            }
                            if(insere) {
                                if(!atual->first->deleted)
                                    elementos.push_back(atual->first);
                            }
                            break;
                        }
                    }

                }
            }


        }
        elementos.pop_front();
    }

    for(map<Face*, int>::iterator it = mapaNiveis.begin(); it != mapaNiveis.end(); ++it) {
        Insert(it->first, it->second);
    }
}

void HTypeManager::run() {
    int i = 0;

    while(!faces.empty()) {

        vector<Face*>* filhos;

        //char caminho[80];
        //char arquivo[4];

        if(faces.front().second >= 1) {
            filhos = faces.front().first->Subdivide();

            if(!filhos) {
                faces.pop();
                continue;
            }
        }
//        else if(faces.front()->Nivel <= -1) {
//            set<Face>::iterator it = setFaces.find(*(faces.front()));
//
//            if( it != setFaces.end() ) {
//                Face novaFace(*it);
//                setFaces.erase(it);
//                novaFace.Desrefina();
//                setFaces.insert(novaFace);
//            }
//        //else if(faces.front()->Nivel == 0)
//        //    faces.push_back(DESREFINAMENTO)
//
//
//        }


        if(faces.front().second >= 1) {
            for(vector<Face*>::iterator it = filhos->begin(); it != filhos->end(); ++it) {
                Insert(*it, faces.front().second - 1);
            }
        }


        faces.pop();
        //strcpy(caminho, "malhas/adapted_mesh");
        //sprintf(arquivo, "%d", i);
        //strcat(caminho, arquivo);
        //strcat(caminho, ".msh");

        //M_writeMsh(MAdMesh,caminho,2, NULL);
        i++;
    }
    hMesh->Resolve();
}

void HTypeManager::clean() {
    while(!faces.empty()) {
        faces.pop();
    }
    mapaNiveis.clear();
}
