#ifndef HTYPEMANAGER_H_INCLUDED
#define HTYPEMANAGER_H_INCLUDED

#include "Malha.h"

using namespace HMesh;

class HTypeManager { //Classe de gerenciamento de refinamento
protected:
    static queue< pair<Face*, int> > faces;
//    static set<Face> setFaces; //Todas as faces refinadas
//
//    static MAd::MeshAdapter* pAdapt;
    static Malha* hMesh;
//
    static map<Face*, int, FaceOperador> mapaNiveis;

public:
    HTypeManager() {;}

    static void Initialize(Malha* hMesh);

    static void Insert(Face* face, int nivel);
    static void Insert_Uneven(Face* face, int nivel);

    static void Regularize();

    static void run();

    static void clean();
//
//    static void CBFunction(MAd::pPList, MAd::pPList, void*, MAd::operationType, MAd::pEntity);
};

#endif // HTYPEMANAGER_H_INCLUDED
