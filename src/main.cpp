#include <iostream>
#include <iomanip>

#include <matrix.hpp>
#include <parser.hpp>

using namespace std;

int main() {

    // Testiranje parser_var klase (bez operator overloadinga, jos nije gotov)

    /*parser_var pvar;

    try {
        pvar.mat("neki random text{{000,      1, 2},{43, 5, 2},{120, 1, 2},{23, 4, 2}}");
    } catch (parser_var::error_parse err) {
        cout << "Parser ERROR: " << err.info() << "\n";
        return 0;
    }
*/
    matrix matA(2, 2);

    matA.set(1, 1, 1);
    matA.set(1, 2, 2);
    matA.set(2, 1, 3);
    matA.set(2, 2, 4);
    /*
    cout << "--- Hello it seems to be working ----\n\n";
    cout << "Matrica: \n" << pvar.mat();

    try {
        pvar.num(1234.1231);
    } catch (parser_var::error_parse err) {
        cout << "Parser ERROR: " << err.info() << "\n";
        return 0;
    }

    cout << "Broj : " << pvar.num() << "\n";

    
    pvar.mat(matA);*/
    
    cout << "Matrica dodjeljena varijablom: \n" << matA.trans();
}