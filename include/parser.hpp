#ifndef INCLUDE_PARSER_HPP
#define INCLUDE_PARSER_HPP

#include <matrix.hpp>
#include <calculator_error.hpp>
#include <iostream>

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

// Tipovi parser varijable
#define PARSER_VAR_UNDEFINED  0
#define PARSER_VAR_NUMBER     1
#define PARSER_VAR_MATRIX     2
#define PARSER_VAR_FUNCTION   3
#define PARSER_VAR_OPERATOR   4

class parser_var {
    private:
        int var_type;         // Tip podatka pohranjenog u varijabli undefined/matrica/broj
        double *number_part;  // Pointer na broj ako je pohranjen
        matrix *matrix_part;  // Pointer na matricu ako je pohranjena
    protected:
        parser_var *next_var; // Pointer na sljedeću varijablu u listi
                              // koristi ju parser klasa kod povezivanja varijabli u listu (linked list)

        int opr_code;         // Kod operatora ako je pohranjen, samo parser klasa (interno) koristi ovaj tip varijable
        int fun_code;         // Kod funkcije ako je pohranjena, samo parser klasa (interno) koristi ovaj tip varijable

        char *variable_name;  // Pointer na ime varijable, koristi se u parser klasi
                              // ako je postavljen na neku vrijednost, pri uništavanju objekta lokacija na koju pointer
                              // pokazuje bit će oslobođena delete operatorom
    public:
        // Constructor
        parser_var() {
            next_var = NULL;
            var_type = PARSER_VAR_UNDEFINED;
            number_part = NULL;
            matrix_part = NULL;
            variable_name = NULL;
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
            if (variable_name != NULL)
                delete variable_name;
        }
        // Dobavi tip varijable undefined/matrica/broj/operator
        inline int type() const {
            return var_type;
        }
        // Provjeri je li vrijednost varijable definirana
        inline int is_defined() const {
            return (var_type != PARSER_VAR_UNDEFINED) ? 1 : 0;
        }
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

        // Greška se javlja pri dobavljanju vrijednosti koja nije definirana
        class error_undefined : public calculator_error {
            public:
                error_undefined(const char desc[]) : calculator_error(desc) {}
        };
        // Greška se javlja pri računanju vrijednosti koje je nemoguće izračunati
        class error_calculation : public calculator_error {
            public:
                error_calculation(const char desc[]) : calculator_error(desc) {}
        };
        // Greška se javlja pri neuspjelom pokušaju izvlačenja podataka iz niza znakova
        class error_parse : public calculator_error {
            public:
                error_parse(const char desc[]) : calculator_error(desc) {}
        };
        // Greška se javlja pri pokušaju pretvorbe u tip u koji nije moguće pretvoriti
        // sadržaj varijable npr. matrica >> double
        class error_cast : public calculator_error {
            public:
                error_cast(const char desc[]) : calculator_error(desc) {}
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
// Prototipi operatora ^
extern parser_var operator ^ (const parser_var &a, const parser_var &b);
extern parser_var operator ^ (const parser_var &a, const double n);
extern parser_var operator ^ (const double n, const parser_var &a);


// Kodovi operatora parsera
#define PARSER_OPERATOR_ADD   0         // Zbrajanje operanata
#define PARSER_OPERATOR_SUB   1         // Oduzimanje operanata
#define PARSER_OPERATOR_MUL   2         // Množenje operanata
#define PARSER_OPERATOR_DIV   3         // Dijeljenje operanata
#define PARSER_OPERATOR_EXP   5         // Drugi operant je eksponent prvog
#define PARSER_OPERATOR_OPN   6         // Otvorena zagrada
#define PARSER_OPERATOR_CLS   7         // Zatvorena zagrada

// Kodovi funkcija parsera
#define PARSER_FUNCTION_TRANS 0         // Transponiranje matrice, dostupno samo za matrice

class parser {
    private:
        parser_var *variables;          // Pointer na prvu varijablu u listi definiranih varijabli
        parser_var res_var;             // Varijabla u koju se sprema rezultat operacije

        parser_var *operator_stack;     // Pointer na stack koji se koristi prilikom pretvorne u RPN
        parser_var *output_queue;       // Pointer na queue u koji se sprema rezultat pretvorbe u RPN
        parser_var *temp_stack;         // Pointer na stack koji se koristi pri izračunavanju izraza pohranjenog u queueu

        // Dodaj operator na operator stack
        parser_var *add_to_stack(const int operator_code);
        // Dobavi operator sa operator_stacka
        parser_var *pop_from_stack();

        // Dodaj operator u output_queue
        parser_var *add_operator_to_queue(const int operator_code);
        // Dodaj funkciju u output_queue
        parser_var *add_function_to_queue(const int function_code);
        // Dodaj parser_var objekt u output_queue
        // funkcija dodaje TAJ objekt, što znači da modificira njegov next_var property
        parser_var *add_variable_to_queue(parser_var *new_var_ptr);
        // Dobavi varijablu sa početka queuea
        parser_var *pop_from_queue();

        // Dodaj parser_var objekt na temp_stack
        // funkcija dodaje TAJ objekt, što znači da modificira njegov next_var property
        parser_var *add_to_temp_stack(parser_var *new_var_ptr);
        // Dobavi varijablu sa temp_stacka
        parser_var *pop_from_temp_stack();
        
        // Pronađi parser varijablu danog imena, ako je definirana vrati pointer na nju
        // ako nije vrati NULL
        parser_var *find_variable(const char *name, const char *name_end);

        // Provjeri ima li išta u jednom od internih stack/queue-ova, ako ima oslobodi svu
        // memoriju koju zauzimaju i postavi sve stack/queue pointere na NULL
        void clear_all_stacks();

    public:
        // Default constructor
        parser() {
            variables = NULL;
            operator_stack = NULL;
            temp_stack = NULL;
            output_queue = NULL;
        }
        // Destructor
        ~parser() {
            clear_all_stacks();
        }
        // Ako ne postoji kreiraj varijablu, zatim vrati referencu na istu
        parser_var &variable(const char name[]);
        // Obriši varijablu danog imena
        void del_variable(const char name[]);
        // Broj kreiranih varijabli parsera
        int length();
        // Izračunaj izraz
        parser_var &calculate(const char expression[]);
        // Dobavi rezultat računa
        parser_var &result() {
            return res_var;
        }
        // Koristi se za pristupanje pojedinoj varijabli prema indexu
        // NAPOMENA: index varijable se mijenja ako je obrisana varijabla prije nje
        parser_var &operator [] (int index);

        // Greška se javlja pri dobavljanju varijable koja nije definirana
        class error_undefined : public calculator_error {
            public:
                error_undefined(const char desc[]) : calculator_error(desc) {}
        };
        // Greška se javlja pri računanju vrijednosti koje je nemoguće izračunati
        class error_calculation : public calculator_error {
            public:
                error_calculation(const char desc[]) : calculator_error(desc) {}
        };
        // Greška se javlja pri neuspjelom pokušaju 
        class error_invalid : public calculator_error {
            public:
                error_invalid(const char desc[]) : calculator_error(desc) {}
        };
};

#endif