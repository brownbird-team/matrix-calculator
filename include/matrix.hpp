#include <iostream>
#include <iomanip>
#include <calculator_error.hpp>

#ifndef INCLUDE_MATRIX_HPP
#define INCLUDE_MATRIX_HPP

/********************************************************************
 * Matrix kalsa                                                     *
 *                                                                  *
 * Koristi se za izvođenje jednostavnih operacija nad matricama,    *
 * podržava zbrajanje, množenje i oduzimanje dvaju matrica, te      *
 * množenje matrica skalarom                                        *
 ********************************************************************/

// Maksimalna duljina opisa greške parser varijable
#define MAX_MATRIX_ERR_DESC 70

class matrix {
    private:
        double **mat_ptr;   // Pointer na matricu
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
        void set(int row, int col, double value);
        // Podrška za int value u setu
        void set(int row, int col, int value);
        // Zatraži vrijednost elementa matrice u danom retku i stupcu
        double get(int row, int col) const;
        // Transponira matricu
        matrix trans() const;
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
        //Provjerava je li ortogonalna
        int is_ortogonal();
        class data{
            public:
                int square;
                int nul;
                int identity;
                int upper_triangular;
                int lower_triangular;
                int diagonal;
                int simetrical;
                int antisimetrical;
                int ortogonal;

                
        };
        data analize_matrix();
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
        class calculation_error : public calculator_error {
            public:
                calculation_error(const char desc[]) : calculator_error(desc) {}
        };

        // Traženi element ne postoji u matrici
        class element_not_found : public calculator_error {
            public:
                element_not_found(const char desc[]) : calculator_error(desc) {}
        };
};

// Zbrajanje dvaju matrica
extern matrix operator + (const matrix &a, const matrix &b);
// Oduzimanje dvaju matrica
extern matrix operator - (const matrix &a, const matrix &b);
// Množenje dvaju matrica
extern matrix operator * (const matrix &a, const matrix &b);
// Množenje skalara matricom
extern matrix operator * (const double n, const matrix &b);
// Množenje matrice skalarom
extern matrix operator * (const matrix &a, const double n);
// Ispisivanje matrice u konzolu
extern std::ostream &operator << (std::ostream &out_file, const matrix &mat);

#endif