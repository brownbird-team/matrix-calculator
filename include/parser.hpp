#ifndef INCLUDE_PARSER_HPP
#define INCLUDE_PARSER_HPP

#include <matrix.hpp>
#include <iostream>

class parser_operator {
    public:
        int id;
        char character;

        parser_operator(const int oper_id, const char oper_char) {
            int i;
            id = oper_id;
            character = oper_char;
        }
        parser_operator(const parser_operator &other_oper) {
            int i;
            id = other_oper.id;
            character = other_oper.character;
        }
};

// Kodovi operatora parser varijable
#define PARSER_OPERATOR_ADD   0         // Zbrajanje operanata
#define PARSER_OPERATOR_SUB   1         // Oduzimanje operanata
#define PARSER_OPERATOR_MUL   2         // Množenje operanata
#define PARSER_OPERATOR_DIV   3         // Dijeljenje operanata
#define PARSER_OPERATOR_EXP   4         // Drugi operant je eksponent prvog

// Definiraj znak svakog od operatora
const parser_operator parser_operators[] {
    parser_operator(PARSER_OPERATOR_ADD, '+'),
    parser_operator(PARSER_OPERATOR_SUB, '-'),
    parser_operator(PARSER_OPERATOR_MUL, '*'),
    parser_operator(PARSER_OPERATOR_DIV, '/'),
    parser_operator(PARSER_OPERATOR_EXP, '^')
};


/*******************************************************************************
 * Parser variable (parser_var) class                                          *
 *                                                                             *
 * Definirane member funkcije:                                                 *
 *     parser_var()             // Default constructor                         *
 *     parser_var(parser_var)   // Copy constructor                            *
 *                                                                             *
 *     type()                   // Dobavlja trenutni tip varijable, može       *
 *                              // biti undefined/matrica/broj                 *
 *                                                                             *
 *     is_defined()             // Provjerava nalazi li se išta u varijabli    *
 *                                                                             *
 *     mat(matrica)             // Postavlja tip varijable na matricu i u      *
 *                              // varijablu kopira danu matricu               *
 *                                                                             *
 *     mat(niz_znakova)         // Postavlja tip varijable na matricu i        *
 *                              // popunjava matricu prema obliku zadanom u    *
 *                              // nizu znakova npr. {{1, 2}{3, 4}}            *
 *                                                                             *
 *     mat()                    // Dobavlja matricu iz varijable ako je tip    *
 *                              // varijable matrica, u suprotnom vraća        *
 *                              // grešku: parser_var::error_undefined         *
 *                                                                             *
 *     num(double)              // Postavlja tip varijable na broj i u nju     *
 *                              // upisuje dani broj                           *
 *                                                                             *
 *     num(niz_znakova)         // Postavlja tip varijable na broj, pretvara   *
 *                              // niz znakova u broj i zapisuje ga u nju, ako *
 *                              // pretvorba znakova u broj ne uspije vraća    *
 *                              // grešku: parser_var::error_parse             *
 *                                                                             *
 *     num()                    // Dobavlja broj iz varijable ako je tip       *
 *                              // varijable broj, u suprotnom vraća           *
 *                              // grešku: parser_var::error_undefined         *
 *                                                                             *
 * Definirane funkcije operatora:                                              *
 *                                         v -- varijabla (parser_var)         *
 *                                         s -- skalar (double)                *
 *                                         m -- matrica (matrix)               *
 *     v = v                                                                   *
 *     v++                                                                     *
 *     ++v                                                                     *
 *     v--                                                                     *
 *     --v                                                                     *
 *                                                                             *
 * Napomena: operatori poput += -= /= i slični nisu definirani te se stoga     *
 *           ne mogu koristiti                                                 *
 *                                                                             *
 * Definirane aritmetičke operacije:                                           *
 *                                                                             *
 *     v = v + v       v = v * v                                               *
 *     v = s + v       v = s * v                                               *
 *     v = v + s       v = v * s                                               *
 *     v = m + v       v = m * v                                               *
 *     v = v + m       v = v * m                                               *
 *     v = v - v       v = v / v                                               *
 *     v = s - v       v = s / v                                               *
 *     v = v - s       v = v / s                                               *
 *     v = m - v       -v                                                      *
 *     v = v - m       +v                                                      *
 *                                                                             *
 * Resultat svake operacije sa parser varijablom je parser varijabla, naravno  *
 * nije garantirano da će se svaka od operacija gore moći izvršiti sa svakom   *
 * instancom varijable, sve ovisi o tipu podatka pohranjenom u varijabli.      *
 * npr. ako je varijabla matrica operacija v = v / v nije moguća jer matrice   *
 * ne možemo dijeliti. Stoga će ta operacija rezultirati greškom:              *
 * parser_var::error_calculation.                                              *
 *                                                                             *
 * Definirani cast operatori:                                                  *
 *                                                                             *
 *     (int)                Naravno mogućnost korištenja operatora ovisi o     *
 *     (double)             tipu pohranjenog podatka, npr. ako je pohranjena   *
 *     (float)              matrica (int) će rezultirati greškom:              *
 *     (matrix)             parser_var::error_cast                             *
 *                                                                             *
 *                                                                             *
 *******************************************************************************/

// Maksimalna duljina opisa greške parser varijable
#define MAX_PARSER_VAR_ERR_DESC 70

// Tipovi parser varijable
#define PARSER_VAR_UNDEFINED  0
#define PARSER_VAR_NUMBER     1
#define PARSER_VAR_MATRIX     2
#define PARSER_VAR_FUNCTION   3
#define PARSER_VAR_OPERATOR   4

class parser_var {
    private:
        int var_type;         // Tip podatka pohranjenog u varijabli undefined/matrica/broj
        int opr_code;         // Kod operatora ako je pohranjen
        int fun_code;         // Kod funkcije ako je pohranjena
        double *number_part;  // Pointer na broj ako je pohranjen
        matrix *matrix_part;  // Pointer na matricu ako je pohranjena
    protected:
        parser_var *next_var; // Pointer na sljedeću varijablu u listi
                              // koristi ju parser klasa kod povezivanja varijabli u listu (linked list)
    public:
        // Constructor
        parser_var() {
            next_var = NULL;
            var_type = PARSER_VAR_UNDEFINED;
            number_part = NULL;
            matrix_part = NULL;
        }
        // Copy constructor
        parser_var(const parser_var &other_parser_var) {
            next_var = other_parser_var.next_var;
            var_type = other_parser_var.var_type;
            switch(var_type) {
                case PARSER_VAR_MATRIX:
                    mat(*other_parser_var.matrix_part);
                    break;
                case PARSER_VAR_NUMBER:
                    num(*other_parser_var.number_part);
                    break;
            }
        }
        // Destructor
        ~parser_var() {
            if (number_part != NULL)
                delete number_part;
            if (matrix_part != NULL)
                delete matrix_part;
        }
        // Dobavi tip varijable undefined/matrica/broj/operator
        inline int type() const {
            return var_type;
        }
        // Provjeri je li vrijednost varijable definirana
        inline int is_defined() const {
            return (var_type != PARSER_VAR_UNDEFINED) ? 1 : 0;
        }
        // Postavi tip varijable na operator i kopiraj dani operator u varijablu
        inline int opr(const int operator_code) {
            switch(var_type) {
                case PARSER_VAR_NUMBER:
                    delete number_part;
                    number_part = NULL;
                    break;
                case PARSER_VAR_MATRIX:
                    delete matrix_part;
                    matrix_part = NULL;
                    break;
            }
            var_type = PARSER_VAR_OPERATOR;
            opr_code = operator_code;
        }
        // Postavi tip varijable na operator i u nju spremi operator danog stringa
        inline int opr();
        // Postavi tip varijable na matricu i kopiraj dani objekt matrice u nju
        inline matrix &mat(const matrix &matrix_object) {
            switch(var_type) {
                case PARSER_VAR_NUMBER:
                    delete number_part;
                    number_part = NULL;
                    break;
                case PARSER_VAR_MATRIX:
                    delete matrix_part;
                    matrix_part = NULL;
                    break;
            }

            var_type = PARSER_VAR_MATRIX;
            matrix_part = new matrix(matrix_object.rows, matrix_object.cols);
            *matrix_part = matrix_object;

            return *matrix_part;
        }
        // Postavi tip varijable na matricu i popuni matricu prema zadanom obliku
        matrix &mat(const char matrix_string[]);
        // Dobavi matricu pohranjenu u varijabli
        inline matrix &mat() const {
            if (var_type == PARSER_VAR_MATRIX)
                return *matrix_part;
            else
                throw error_undefined("Matrix is undefined");
        }
        // Postavi tip varijable na broj i upiši dani broj u varijablu
        inline double num(double number) {
            switch(var_type) {
                case PARSER_VAR_NUMBER:
                    delete number_part;
                    number_part = NULL;
                    break;
                case PARSER_VAR_MATRIX:
                    delete matrix_part;
                    matrix_part = NULL;
                    break;
            }

            var_type = PARSER_VAR_NUMBER;
            number_part = new double;
            *number_part = number;

            return *number_part;
        }
        // Postavi tip varijable na broj, pretvori dani string u broj i pohrani ga
        double num(const char num_string[]);
        // Dobavi broj pohranjen u varijabli
        inline double num() const {
            if (var_type == PARSER_VAR_NUMBER)
                return *number_part;
            else
                throw error_undefined("Number is undefined");
        }

        // Općenito greška vezana uz parser_var
        class error {
            private:
                char description[MAX_PARSER_VAR_ERR_DESC];
            public:
                error(const char desc[]) {
                    int i;
                    for (i = 0; desc[i] != '\0' && i < MAX_PARSER_VAR_ERR_DESC - 1; i++)
                        description[i] = desc[i];
                    description[i] = '\0';
                }
                inline const char * info() const {
                    return description;
                }
        };

        // Greška se javlja pri dobavljanju vrijednosti koja nije definirana
        class error_undefined : public error {
            public:
                error_undefined(const char desc[]) : error(desc) {}
        };
        // Greška se javlja pri računanju vrijednosti koje je nemoguće izračunati
        class error_calculation : public error {
            public:
                error_calculation(const char desc[]) : error(desc) {}
        };
        // Greška se javlja pri neuspjelom pokušaju izvlačenja podataka iz niza znakova
        class error_parse : public error {
            public:
                error_parse(const char desc[]) : error(desc) {}
        };
        // Greška se javlja pri pokušaju pretvorbe u tip u koji nije moguće pretvoriti
        // sadržaj varijable npr. matrica >> double
        class error_cast : public error {
            public:
                error_cast(const char desc[]) : error(desc) {}
        };

        // Definicija operatora dodjeljivanja
        parser_var &operator = (const parser_var &other_parser_var) {
            var_type = other_parser_var.var_type;
            switch(var_type) {
                case PARSER_VAR_MATRIX:
                    mat(*other_parser_var.matrix_part);
                    break;
                case PARSER_VAR_NUMBER:
                    num(*other_parser_var.number_part);
                    break;
            }
            return *this;
        }

        // Klasa parser je prijatelj kako bi mogla koristiti pointer next_var kako
        // bi kreirala linked list
        friend class parser;
};

// Prototipi operatora +
extern parser_var operator + (const parser_var &a, const parser_var &b);
extern parser_var operator + (const parser_var &a, const double n);
extern parser_var operator + (const double n, const parser_var &a);
// Prototipi operatora -
extern parser_var operator - (const parser_var &a, const parser_var &b);
extern parser_var operator - (const parser_var &a, const double n);
extern parser_var operator - (const double n, const parser_var &a);
// Prototipi operatora *
extern parser_var operator * (const parser_var &a, const parser_var &b);
extern parser_var operator * (const parser_var &a, const double n);
extern parser_var operator * (const double n, const parser_var &a);
// Prototipi operatora /
extern parser_var operator / (const parser_var &a, const parser_var &b);
extern parser_var operator / (const parser_var &a, const double n);
extern parser_var operator / (const double n, const parser_var &a);


/*
class parser {
    private:
        parser_var result;
    public:
        // Ako ne postoji kreiraj varijablu, zatim vrati referencu na istu
        parser_var &var(const char name[]);
        // Obriši varijablu danog imena
        void delvar(const char name[]);
        // Izračunaj izraz
        void calculate(const char expression[]);
        // Provjeri postoji li nešto u varijabli rezultata računa
        int result_is_defined();
        // Dobavi rezultat računa
        parser_var &result();
};*/

#endif