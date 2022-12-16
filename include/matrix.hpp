#include <iostream>
#include <iomanip>

#ifndef INCLUDE_MATRIX_HPP
#define INCLUDE_MATRIX_HPP

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
        int rows;           // Broj redaka matrica
        int cols;           // Broj stupaca matrice

        // Copy Constructor
        matrix(const matrix &other_mat);
        // Constructor kreira matricu
        matrix(int number_rows, int number_cols);
        // Oslobodi memoriju matrice
        ~matrix();
        // Postavi vrijednost elementa matrice u danom retku i stupcu
        void set(int row, int col, int value);
        // Zatraži vrijednost elementa matrice u danom retku i stupcu
        int get(int row, int col) const;
        //Provjera je li matrica kvadratna
        int is_squared();
        //Provjerava je li nul matrica
        int is_nul();
        //Provjerava je li jedinična matrica
        int is_identity();
        //Provjerava je li gornje trokutasta
        int is_upper_triangular();
        //Provjerava je li donje trokutasta
        int is_lower_triangular();
        //Provjerava je li dijagonalna
        int is_diagonal();
        //Provjerava je li simetricna
        int is_simetrical();
        //Provjerava je li antisimetricna
        int is_antisimetrical();
        // Operacija dodjeljivanja
        matrix operator = (const matrix &other_mat) {
            int i, j;

            for (i = 1; i <= rows && i <= other_mat.rows; i++) {
                for (j = 1; j <= cols && j <= other_mat.cols; j++) {
                    set(i, j, other_mat.get(i, j));
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
extern matrix operator + (const matrix &a, const matrix &b);
// Oduzimanje dvaju matrica
extern matrix operator - (const matrix &a, const matrix &b);
// Množenje dvaju matrica
extern matrix operator * (const matrix &a, const matrix &b);
// Množenje skalara matricom
extern matrix operator * (const int n, const matrix &b);
// Množenje matrice skalarom
extern matrix operator * (const matrix &a, const int n);
// Ispisivanje matrice u konzolu
extern std::ostream &operator << (std::ostream &out_file, const matrix &mat);

#endif