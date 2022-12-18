#include <parser.hpp>
#include <matrix.hpp>
#include <iostream>

/*******************************************************************************
 * number_parse -- pretvorba niza znakova koji predstavlja broj u double broj  *
 *                                                                             *
 * Parametri                                                                   *
 *     string      -- niz znakova u kojem se nalazi reprezentacija broja       *
 *     string_end  -- pointer na znak nakon zadnjega znaka koji treba biti     *
 *                    obrađen, postaviti na NULL ako želite da se obradi       *
 *                    cijeli niz znakova (ide do \0)                           *
 * Vraća                                                                       *
 *     double koji je dobiven iz niza znakova                                  *
 *                                                                             *
 * Ako pretvorba ne uspije bacit će grešku parser_var::error_parse             *
 *******************************************************************************/

double number_parse(const char string[], const char *string_end) {
    int i;                     // Index stringa
    int prefix = 1;            // Prefix broja + ili - odnosno -1 ili +1
    int prefix_set = 0;        // Pokazuje je li nađen prefix broja +/-
    int is_decimal = 0;        // Pokazuje jesmo li u cijelom ili decimalnom dijelu broja prije/nakon točke
    int decimal_divider = 10;  // Na kojoj smo znamenki decimalnog dijela, gdje je 10 prva, a 1000 treća
    double result = 0;         // Krajnji rezultat, broj

    // Parsaj broj od početka do \0 ili krajnjeg pointera
    for (i = 0; string[i] != '\0' && (string_end == NULL || string_end > string + i); i++) {
        // Preskoči bjeline
        if (string[i] == ' ' || string[i] == '\t')
            continue;
        // Ako prefix nije postavljen provjeri jel znak prefix, ako je postavi ga
        if (!prefix_set) {
            if (string[i] == '-') {
                prefix *= -1;
                prefix_set = 1;
                continue;
            }
            else if (string[i] == '+') {
                prefix_set = 1;
                continue;
            }
        }
        // Ako je znak točka prelazimo na decimale
        if (string[i] == '.') {
            is_decimal = 1;
            continue;
        }
        // Ako je znak znamenka zbroj ga na odgovarajući način
        if (string[i] >= '0' && string[i] <= '9') {
            if (!is_decimal) {
                result *= 10;
                result += string[i] - '0';
            } else {
                result += (string[i] - '0') / (double)decimal_divider;
                decimal_divider *= 10;
            }
            continue;
        }
        // Ako znak nije ništa od navedenog, neš ne valja
        throw parser_var::error_parse("Failed to parse number");
    }
    // Vrati dobiveni rezultat
    return result * prefix;
}

/*******************************************************************************
 * mat -- postavlja tip varijable parser_var na matricu, iz zadanog niza       *
 *        znakova iščitava oblik matrice i sprema danu matricu u varijablu     *
 *                                                                             *
 * Primjer oblika matrice u stringu                                            *
 *     {{1, 2, 3},{4, 5, 6},{7, 8, 9}}                                         *
 *                                                                             *
 * Parametri                                                                   *
 *     matrix_string -- niz znakova formatiran u zadanom obliku koji           *
 *                      predstavlja matricu                                    *
 *                                                                             *
 * Vraća                                                                       *
 *     referencu na novokreiranu matricu                                       *
 *                                                                             *
 * U slučaju greške pri pretvorbi formatiranog stringa u matricu funkcija baca *
 * grešku: parser_var::error_parse                                             *
 *******************************************************************************/

matrix &parser_var::mat(const char matrix_string[]) {
    int i;                     // Counter
    int length;                // Duljina matrix_string niza
    int level = 0;             // Koliko smo dobuko u zagradama npr {{ovdje}} je level 2
    int n_rows = 0;            // Koliko redova ima matrica opisana u stringu
    int n_cols = -1;           // Koliko stupaca ima matrica opisana u stringu
    int n_cols_temp = 0;       // Koristi se za prebrojavanje stupaca svakog reda kako bi se uvjerili da su svi isti

    // Provjeri duljinu matrix stringa
    for (length = 0; matrix_string[length] != '\0'; length++);

    // Prebroji redove i stupce, provjeri jel zapis ispravan
    for (i = 0; i < length; i++) {
        // Ako je znak otvorena vitica povećaj level, ako smo ušli u novi redak
        // povećaj broj broj redaka za 1
        if (matrix_string[i] == '{') {
            ++level;
            if (level == 2)
                ++n_rows;
        }
        // Ako je znak zatvorena vitica smanji level, provjeri je li broj stupaca u
        // ovom retku jednak broju stupaca u ostalima, ako nije javi grešku, zatim povećaj level,
        // također ako je ovo prvi red postavi koliko je stupaca bilo
        else if (matrix_string[i] == '}') {
            if (level == 2) {
                if (n_cols != -1 && n_cols_temp != n_cols) {
                    throw error_parse("Number of cols is not equal for all rows");
                } else {
                    n_cols = n_cols_temp;
                    n_cols_temp = 0;
                }
            }
            --level;
        }
        // Ako je znak zarez povećaj privremeni brojač stupaca ako smo u retku
        else if (matrix_string[i] == ',') {
            if (level == 2) {
                ++n_cols_temp;
            }
        }
        // Ako je level veći od 2 javi grešku jer je to nemoguće, matrice nemaju 3 dimenzije
        if (level > 2) {
            throw error_parse("Something is wrong with brackets");
        }
    }
    // Ako je level ne jednak 0 nešto nije zatvoreno kak treba
    if (level != 0) {
        throw error_parse("Number of opening and closing brackets is not equal");
    }

    // Obriši brojevni/matrični dio varijable ako postoji
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
    // S obzirom da brojimo zareze broj stupaca je za 1 pre mali
    ++n_cols;

    // Kreiraj novu matricu izbrojenih dimenzija
    var_type = PARSER_VAR_MATRIX;
    matrix_part = new matrix(n_rows, n_cols);

    int current_row = 1;  // Prilicno su self explanatory
    int current_col = 1;  // ovo dvoje
    
    // Izvadi elemente iz matrice
    for (i = 0; i < length; i++) {
        if (matrix_string[i] == '{') {
            ++level;
            continue;
        }
        else if (matrix_string[i] == '}') {
            --level;
        }
        // Ako smo u retku
        if (level == 2 && matrix_string[i] != ',') {
            double result;                               // Vrijednost elementa matrice
            const char *end_ptr = matrix_string + i;     // Znak nakon zadnjeg znaka elementa
            // Pronađi znak nakon zadnjeg znaka elementa
            while (*end_ptr != ',' && *end_ptr != '}')
                ++end_ptr;
            // Probaj parsat broj
            try {
                result = number_parse(matrix_string + i, end_ptr);
            } catch (error_parse err) {
                throw err;
            }
            // Postavi element matrice
            matrix_part->set(current_row, current_col, result);
            // Prijeđi na sljedeći element
            current_row += (current_col) / n_cols;
            current_col = current_col % n_cols + 1;
            i = end_ptr - matrix_string - 1;
        }
    }

    return *matrix_part;
}

double parser_var::num(const char num_string[]) {
    try {
        return number_parse(num_string, NULL);
    } catch (error_parse err) {
        throw err;
    }
}

parser_var operator + (const parser_var &a, const parser_var &b) {
    parser_var result; // Rezultat izračuna

    // Ako je a matrica i b matrica
    if (a.type() == PARSER_VAR_MATRIX && b.type() == PARSER_VAR_MATRIX) {
        try {
            result.mat(a.mat() + b.mat());
        } catch (matrix::calculation_error err) {
            throw parser_var::error_calculation(err.info());
        }
    }
    // Ako je a skalar i b skalar
    else if (a.type() == PARSER_VAR_NUMBER && b.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() + b.num());
    }
    // Ako su varijable različitih tipova ili je jedna od njih undefined
    else {
        throw parser_var::error_calculation("It's impossible to ADD variables of different types");
    }

    return result;
}
parser_var operator + (const parser_var &a, const double n) {
    parser_var result; // Rezultat izračuna

    // Ako je a skalar i n je skalar (double)
    if (a.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() + n); 
    }
    // Ako je a bilo što osim broja
    else {
        throw parser_var::error_calculation("Number can only be added to a number");
    }

    return result;
}
parser_var operator + (const double n, const parser_var &a) {
    try {
        return a + n;
    } catch (parser_var::error_calculation err) {
        throw err;
    }
}


parser_var operator - (const parser_var &a, const parser_var &b) {
    parser_var result; // Rezultat izračuna

    // Ako je a matrica i b matrica
    if (a.type() == PARSER_VAR_MATRIX && b.type() == PARSER_VAR_MATRIX) {
        try {
            result.mat(a.mat() - b.mat());
        } catch (matrix::calculation_error err) {
            throw parser_var::error_calculation(err.info());
        }
    }
    // Ako je a skalar i b skalar
    else if (a.type() == PARSER_VAR_NUMBER && b.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() - b.num());
    }
    // Ako su varijable različitih tipova ili je jedna od njih undefined
    else {
        throw parser_var::error_calculation("It's impossible to SUBTRACT variables of different types");
    }

    return result;
}
parser_var operator - (const parser_var &a, const double n) {
    parser_var result; // Rezultat izračuna

    // Ako je a skalar i n je skalar (double)
    if (a.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() - n);
    }
    // Ako je a bilo što osim broja
    else {
        throw parser_var::error_calculation("Number can only be subracted by a number");
    }

    return result;
}
parser_var operator - (const double n, const parser_var &a) {
    try {
        return a - n;
    } catch (parser_var::error_calculation err) {
        throw err;
    }
}


parser_var operator * (const parser_var &a, const parser_var &b) {
    parser_var result; // Rezultat izračuna

    // Ako je a matrica i b matrica
    if (a.type() == PARSER_VAR_MATRIX && b.type() == PARSER_VAR_MATRIX) {
        try {
            result.mat(a.mat());
        } catch (matrix::calculation_error err) {
            throw parser_var::error_calculation(err.info());
        }
    }
    // Ako je a matrica i b skalar
    else if (a.type() == PARSER_VAR_MATRIX && b.type() == PARSER_VAR_NUMBER) {
        result.mat(a.mat() * b.num());
    }
    // Ako je a matrica i b skalar
    else if (a.type() == PARSER_VAR_NUMBER && b.type() == PARSER_VAR_MATRIX) {
        result.mat(a.num() * b.mat());
    }
    // Ako je a skalar i b skalar
    else if (a.type() == PARSER_VAR_NUMBER && b.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() * b.num());
    }
    // Jedna matrica je nedefinirana
    else {
        throw parser_var::error_calculation("One of variables is undefined");
    }

    return result;
}
parser_var operator * (const parser_var &a, const double n) {
    parser_var result; // Rezultat izračuna

    // Ako je a matrica
    if (a.type() == PARSER_VAR_MATRIX) {
        result.mat(a.mat() * n);
    }
    // Ako je a broj
    else if (a.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() * n);
    }
    else {
        throw parser_var::error_calculation("Variable is undefined");
    } 

    return result;
}
parser_var operator * (const double n, const parser_var &a) {
    try {
        return a * n;
    } catch (parser_var::error_calculation err) {
        throw err;
    }
}


parser_var operator / (const parser_var &a, const parser_var &b) {
    parser_var result; // Rezultat izračuna

    // Ako netko pokušava podijeliti matrice javi grešku
    if (a.type() == PARSER_VAR_MATRIX || b.type() == PARSER_VAR_MATRIX) {
        throw parser_var::error_calculation("Matrices can't be divided");
    }
    // Ako su oba broj nastavi
    else if (a.type() == PARSER_VAR_NUMBER && b.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() / b.num());
    }
    // Jedna matrica je nedefinirana
    else {
        throw parser_var::error_calculation("One of variables is undefined");
    }

    return result;
}
parser_var operator / (const parser_var &a, const double n) {
    parser_var result; // Rezultat izračuna

    // Ako netko pokušava podijeliti matrice javi grešku
    if (a.type() == PARSER_VAR_MATRIX) {
        throw parser_var::error_calculation("Matrices can't be divided");
    }
    // Ako su oba broj nastavi
    else if (a.type() == PARSER_VAR_NUMBER) {
        result.num(a.num() / n);
    }
    // Jedna matrica je nedefinirana
    else {
        throw parser_var::error_calculation("One of variables is undefined");
    }

    return result;
}
parser_var operator / (const double n, const parser_var &a) {
    parser_var result; // Rezultat izračuna

    // Ako netko pokušava podijeliti matrice javi grešku
    if (a.type() == PARSER_VAR_MATRIX) {
        throw parser_var::error_calculation("Matrices can't be divided");
    }
    // Ako su oba broj nastavi
    else if (a.type() == PARSER_VAR_NUMBER) {
        result.num(n / a.num());
    }
    // Jedna matrica je nedefinirana
    else {
        throw parser_var::error_calculation("One of variables is undefined");
    }

    return result;
}