#include <iostream>
#include <iomanip>

#include <matrix.hpp>
#include <parser.hpp>

using namespace std;

int main() {

    // Testiranje parser_var klase (bez operator overloadinga, jos nije gotov)
/*
    parser_var pvar;

    try {
        pvar.mat("neki random text{{000,      1, 2},{43, 5, 2},{120, 1, 2},{23, 4, 2}}");
    } catch (parser_var::error_parse err) {
        cout << "Parser ERROR: " << err.info() << "\n";
        return 0;
    }

    matrix matA(3, 3);
    matA.set(1, 1, 0);
    matA.set(1, 2, -3);
    matA.set(1, 3, 4);
    matA.set(2, 1, 3);
    matA.set(2, 2, 0);
    matA.set(2, 3, 0);
    matA.set(3, 1, -4);
    matA.set(3, 2, 0);
    matA.set(3, 3, 0);
    */
    matrix matA(2,2);
    matA.set(1, 1, 0);
    matA.set(1, 2, 1);
    matA.set(2, 1, -1);
    matA.set(2, 2, 0);
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

    
    pvar.mat(matA);
    
    pvar.mat(matA);
    cout << "Matrica dodjeljena varijablom: \n" << pvar.mat();
*/
    cout << "Matrica dodjeljena varijablom: \n" << matA.trans();
    cout << matA.is_squared() << "\n";
    cout << matA.is_identity() << "\n";
    cout << matA.is_upper_triangular() << "\n";
    cout << matA.is_lower_triangular() << "\n";
    cout << matA.is_diagonal() << "\n";
    cout << matA.is_simetrical() << "\n";
    cout << matA.is_antisimetrical() << "\n";
    cout << matA.is_ortogonal() << "\n";
    cout << "---------------------" << "\n";
    
    cout << "Matrica dodjeljena varijablom: \n" << matA.trans();

    cout << "------------- Testiranje parser vara -------------\n";
    parser_var parvar;
    parser_var res;

    parvar.mat("{{1, 2, 3},{4, 1234567890'5, 6},{7, 8, 9}}");

    cout << "Matrica na pocetku:\n" << parvar.mat();

    parvar = parvar + parvar;

    cout << "Matrica + Matrica:\n" << parvar.mat();

    parvar = parvar * 2;

    cout << "Matrica * 2:\n" << parvar.mat();
}