#ifndef PONTO_H_INCLUDED
#define PONTO_H_INCLUDED

#include <list>

namespace HMesh {

class Face;
//Pontos são elementos que irão sempre existir (nunca são apagados) durante um refinamento
//Só serão apagados em desrefinamentos
class Ponto {
private:
    double x, y, z; //Coordenadas

public:
    std::list<Face*> faces;

    double get_x() {return x;}
    double get_y() {return y;}
    double get_z() {return z;}

    int ID;
    inline bool operator < (const Ponto & outro) const
    {
      return ID < outro.ID;
    }
    inline bool operator == (const Ponto & outro) const
    {
        return ID == outro.ID;
    }

    Ponto(int num , double x, double y,double z) : x(x), y(y), z(z), ID(num) {}
};

class PontoOperador { //Operador para containers
public:
    bool operator()(const Ponto* ent1, const Ponto* ent2) const
    {
      return *ent1 < *ent2;
    }
};

}
#endif // PONTO_H_INCLUDED
