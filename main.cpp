#include <iostream>
//GMSH
#include "gmsh/Gmsh.h"
#include "gmsh/GModel.h"
#include "gmsh/GEntity.h"
#include "gmsh/MElement.h"
#include "gmsh/MVertex.h"
#include "gmsh/SPoint3.h"

//Geral
#include <utility>
#include <set>
#include <queue>
#include <map>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <algorithm>

//HTypeRefiner
#include "HTypeManager.h"
#include "Malha.h"

#define Dim 2

using namespace std;

int main(int argc, char **argv) {
    GmshInitialize(argc, argv);
    GModel* theModel = new GModel("theModel");
    theModel->readMSH("theMesh.msh");

    theModel->mesh(Dim);

    HMesh::Malha* m = new HMesh::Malha(theModel);

    HMesh::Faces faces = m->getFaces();

    HTypeManager::Initialize(m);

    char saida[30];
    char vtk[30];
    int i = 0;
    for(double y = -10; y < 10; y += 0.1) {
//    double y = 3;
        sprintf(saida, "%s%d%s", "mesh/adapted_mesh", i, ".msh");
        sprintf(vtk, "%s%d%s", "mesh/adapted_mesh", i++, ".vtk");
        for(HMesh::FacesIter it = faces->begin(); it != faces->end(); ++it) {
            double centro[3];

            (*it)->Centro_f(centro);
            if(( centro[1] > y-0.5 ) && ( centro[1] < y+0.5 ))
                HTypeManager::Insert_Uneven(*it, rand()%1+3);
            else
                HTypeManager::Insert_Uneven(*it, 0);

        }

        HTypeManager::Regularize();

        HTypeManager::run();

        m->EscreveMsh(saida);

        HTypeManager::clean();

        faces->clear();

        //m->gb2EstadoInicial();
        delete m;

        m = new HMesh::Malha(theModel);

        faces = m->getFaces();

        HTypeManager::Initialize(m);

        GModel* VTK = new GModel("VTK");
        VTK->readMSH(saida);
        VTK->mesh(Dim);
        VTK->writeVTK(vtk);
        delete VTK;

        //m->EscreveMsh("adapted_mesh.msh");
    }


    return 0;
}
