#include <iostream>
#include <iomanip>

/********************************************************************
 * Matrix kalsa                                                     *
 *                                                                  *
 * Koristi se za izvođenje jednostavnih operacija nad matricama,    *
 * podržava zbrajanje, množenje i oduzimanje dvaju matrica, te      *
 * množenje matrica skalarom                                        *
 ********************************************************************/

class matrix {
    private:
        int **mat_ptr;   // Pointer na matricu
    public:
        int x;           // Broj stupaca
        int y;           // Broj redova

        // Copy Constructor
        matrix(const matrix &other_mat);
        // Constructor kreira matricu
        matrix(int dim_x, int dim_y);
        // Oslobodi memoriju matrice
        ~matrix();
        // Postavi vrijednost elementa matrice
        void set(int dim_x, int dim_y, int value);
        // Zatraži vrijednost elementa matrice
        int get(int dim_x, int dim_y) const;
        // Operacija dodjeljivanja
        matrix operator = (const matrix &other_mat) {
            int i, j;

            for (i = 1; i <= y && i <= other_mat.y; i++) {
                for (j = 1; j <= x && j <= other_mat.x; j++) {
                    set(j, i, other_mat.get(j, i));
                }
            }

            return *this;
        }

        // Došlo je do greške pri računanju sa danim matricama
        class calculation_error {};
        // Traženi element ne postoji u matrici
        class element_not_found {};
};

// Zbrajanje dvaju matrica
matrix operator + (const matrix &a, const matrix &b);
// Oduzimanje dvaju matrica
matrix operator - (const matrix &a, const matrix &b);
// Množenje dvaju matrica
matrix operator * (const matrix &a, const matrix &b);
// Množenje skalara matricom
matrix operator * (const int n, const matrix &b);
// Množenje matrice skalarom
matrix operator * (const matrix &a, const int n);
// Ispisivanje matrice u konzolu
std::ostream &operator << (std::ostream &out_file, const matrix &mat);