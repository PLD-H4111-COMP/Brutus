#include "writer.h"

Writer::Writer(string options){
    m_stream = ofstream(name, ios::out);  // on ouvre le fichier en ecriture
    if(!file){  // si l'ouverture a réussi
           cerr << "Could not open the file" << name << endl;
    }
}


std::ostream& Writer::stream() {
    return m_stream;
}


