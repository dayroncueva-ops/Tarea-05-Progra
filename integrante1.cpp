//
// Created by Dayron Cueva on 11/06/2026.
//
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>

using namespace std;

// Creamos una structura simple del voto
struct Vote {
    string voto_id;
    string opcion_candidato;

    Vote(const string& voter_id_hash,
         const string& option)
        : voto_id(voter_id_hash),
          opcion_candidato(option) {}
    // se crea Vote (id, opcion)
    // cada voto se almacena en un block
    // por ejemplo, el bloque 1 puede tener 5 votos (de este tipo) almacenados.
};

class Block {
public:
    int index; // indica qué bloque fue creado. bloque1->index=1, bloque2->index=2...
    string previous_hash;
    vector<Vote> votos; // almacena la informacion sobre los votos de cada bloque
    int nonce; // se utiliza para crear el hash del bloque, es como un
    string current_hash;

    Block(int index,
          const string& previous_hash,
          const vector<Vote>& votes)
        : index(index),
          previous_hash(previous_hash),
          votos(votes),
          nonce(0)
    {
        current_hash = crearHash();
        // cuando creamos un Block, inmediatamente hacemos que se cree el hahsh correspondiente
    }
    string crearHash() const {
        stringstream ss;
        // usamos ss para almacenar los demas valores
        ss << index;
        ss << previous_hash;
        ss << nonce;
        // hasta aqui tenemos el 000 del hash
        for (const auto& vote : votos) {
            ss << vote.voto_id;
            ss << vote.opcion_candidato;
            // al terminar tendremos 000[id1][opciondeVotante1]+id2+[opciondeVotante2]+...
        }

        hash<string> hasher; // objeto para hashear los strings
        size_t hashValue = hasher(ss.str()); // el ss.str obtiene todo el string almacenado en ss
        // el hasher es un algoritmo que internamente crea un numero

        stringstream hexStream; // convertimos a hexadecimal, pero incluso sin este, ya estaria bien.
        hexStream << hex << hashValue;

        return hexStream.str();
    }
};

int main() {
    vector<Vote> votes;
    votes.push_back(
        Vote("id_01_hash", "Candidato A")
    );
    votes.push_back(
        Vote("id_02_hash", "Candidato B")
    );
    Block genesisBlock(
        0,
        "0",
        votes
    );
    cout << "Indice: "
         << genesisBlock.index
         << endl;
    cout << "Previous Hash: "
         << genesisBlock.previous_hash
         << endl;
    cout << "Nonce: "
         << genesisBlock.nonce
         << endl;
    cout << "Current Hash: "
         << genesisBlock.current_hash
         << endl;

    return 0;
}
