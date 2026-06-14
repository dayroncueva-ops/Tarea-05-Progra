#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>

using namespace std;


struct Vote {
    string voto_id;
    string opcion_candidato;

    Vote(const string& voto,
         const string& opcion)
        : voto_id(voto),
          opcion_candidato(opcion) {}
};

class Block {
public:
    int index;
    string previous_hash;
    vector<Vote> votos;
    int nonce;
    string current_hash;

    // Cada bloque almacena votos
    // Cada bloque tiene un hash
    Block(int index,
          const string& previous_hash,
          const vector<Vote>& votes_)
        : index(index),
          previous_hash(previous_hash),
          votos(votes_),
          nonce(0)
    {
        current_hash = crearHash();
    }

    string crearHash() const {

        stringstream ss;

        ss << index;
        ss << previous_hash;
        ss << nonce;

        for(const auto& vote : votos)
        {
            ss << vote.voto_id;
            ss << vote.opcion_candidato;
        }

        hash<string> hasher;

        size_t hashValue =
            hasher(ss.str());

        stringstream hexStream;
        // el hex esta definido en la libreria iostream y sstream
        // entonces, a partir de alli el hashValue lo convertimos a hexadecimal, teniendo asi un hash mas compacto
        hexStream << hex << hashValue;

        string resultado =
            hexStream.str();
        // definimos que nuestro hash tenga siempre 16 caracteres
        // por ejemplo, si resultado=4af3, length(resultado)= 4; su salida luego sera algo como: 0000000000004af3
        while(resultado.length() < 16)
        {
            resultado =
                "0" + resultado;
        }

        return resultado;
    }

    void mineBlock(int dificultad)
    {
        string objetivo(  // string ob (cantidad, caracter) esto es como decir -> (3,'no'):nonono
            dificultad,
            '0'
        );
        //verificamos si el hash creado para el bloque cumple con el requisito
        while(current_hash.substr(0, dificultad)
              != objetivo)
        {
            nonce++;
            // cuando aumentamos el nonce, se crea un nuevo hash y se repite hasta que satisfaga la condicion (objetivo)
            current_hash =
                crearHash();
        }
        // lo que hemos hecho es minar el hash tal que se cumpla la dificultad requerida. El resultado termina siendo
        // el hash para dicho bloque con un nonce determinado.
        cout << "\nBloque minado correctamente\n";
        cout << "Nonce encontrado: "
             << nonce
             << endl;
        cout << "Hash generado: "
             << current_hash
             << endl;
    }
};


class Blockchain {

private:

    vector<Block> chain;

    static Blockchain* instancia;

    Blockchain() {} // con esto garantizamos no hacer Blockchain b porque es privado

public:
    // un unico blockchain en todo el programa :)
    static Blockchain& getInstance()
    {
        if(instancia == nullptr)
        {
            instancia =
                new Blockchain();
        }

        return *instancia;
    }

    void addBlock(Block block)
    {
        if(!chain.empty())
        {
            block.previous_hash =
                chain.back().current_hash; // back devuelve el ultimo bloque, tal que block añadido tiene como valor
                                            // de previous la del ultimo bloque

            block.current_hash =
                block.crearHash(); // recalculamos el hash de block
        }
        // si chain esta vacio entonces minamos el bloque y guardamos en chain
        block.mineBlock(2);

        chain.push_back(block);
    }

    bool isChainValid() const
    {
        // vamos a verificar si alguien modifico el blockchain
        for(size_t i = 1; i < chain.size(); i++)
        {
            // bloque actual [i] y bloque anterior [i-1]
            const Block& current =
                chain[i];

            const Block& previous =
                chain[i - 1];
            // el current del bloque es calculador con la funcion crearHash
            // entonces por logica deberian ser iguales a menos de que haya sido cambiado
            if(current.current_hash != current.crearHash())
            {
                return false;
            }

            if(current.previous_hash != previous.current_hash)
            {
                return false;
            }
        }

        return true;
    }

    const vector<Block>& getChain() const
    {
        return chain;
    }
};

Blockchain* Blockchain::instancia = nullptr;


class MesaElectoralObserver {
public:
    virtual void update(const Block& nuevoBloque) = 0;

    virtual ~MesaElectoralObserver() = default;
};

class MesaElectoral : public MesaElectoralObserver {

private:
    string nombre;
    vector<Block> copiaLocal;

public:

    MesaElectoral(const string& nombre): nombre(nombre) {}

    void update(const Block& nuevoBloque) override {
        if(nuevoBloque.current_hash ==
           nuevoBloque.crearHash())
        {
            copiaLocal.push_back(
                nuevoBloque
            );

            cout << "[" << nombre
                 << "] Bloque agregado. "
                 << "Total bloques: "
                 << copiaLocal.size()
                 << endl;
        }
    }

};

class CentroElectoralSubject {

private:

    vector<MesaElectoralObserver*> mesas;

public:

    void attach(MesaElectoralObserver* mesa)
    {
        mesas.push_back(mesa);
    }

    void notificarNuevoBloque(
        const Block& bloque)
    {
        cout << "\n================================";
        cout << "\nCENTRO ELECTORAL";
        cout << "\nNuevo bloque detectado";
        cout << "\nNotificando mesas...";
        cout << "\n================================\n";

        for(auto mesa : mesas)
        {
            mesa->update(bloque);
        }
    }
};

int main()
{
    CentroElectoralSubject centro;

    MesaElectoral mesaA("Mesa A");

    MesaElectoral mesaB("Mesa B");

    MesaElectoral mesaC("Mesa C");

    centro.attach(&mesaA);
    centro.attach(&mesaB);
    centro.attach(&mesaC);




    Blockchain& blockchain =
        Blockchain::getInstance();

    vector<Vote> votosBloque1;

    votosBloque1.push_back(
        Vote(
            "id_01_hash",
            "Candidato A"
        )
    );

    votosBloque1.push_back(
        Vote(
            "id_02_hash",
            "Candidato B"
        )
    );

    Block bloque1(0, "0", votosBloque1);

    blockchain.addBlock(bloque1);

    centro.notificarNuevoBloque(blockchain.getChain().back());

    vector<Vote> votosBloque2;

    votosBloque2.push_back(
        Vote(
            "id_03_hash",
            "Candidato A"
        )
    );

    votosBloque2.push_back(
        Vote(
            "id_04_hash",
            "Candidato B"
        )
    );

    Block bloque2(1,"",votosBloque2);

    blockchain.addBlock(bloque2);

    centro.notificarNuevoBloque(blockchain.getChain().back());
    

    vector<Vote> votosBloque3;

    votosBloque3.push_back(Vote("id_05_hash", "Candidato A"));

    votosBloque3.push_back(Vote("id_06_hash", "Candidato B"));

    Block bloque3(2,"",votosBloque3);

    cout << "\nMesa A mina un nuevo bloque...\n";

    blockchain.addBlock(bloque3);

    centro.notificarNuevoBloque(
        blockchain.getChain().back()
    );
    
        cout << "\n=========== CADENA ===========\n";

    for(const auto& block :
        blockchain.getChain())
    {
        cout << "\nIndice: "
             << block.index
             << endl;

        cout << "Previous Hash: "
             << block.previous_hash
             << endl;

        cout << "Nonce: "
             << block.nonce
             << endl;

        cout << "Current Hash: "
             << block.current_hash
             << endl;
    }

    cout << "\n==============================\n";

    if(blockchain.isChainValid())
    {
        cout << "\nLa cadena es VALIDA\n";
    }
    else
    {
        cout << "\nLa cadena fue ALTERADA\n";
    }
    



    return 0;
}
