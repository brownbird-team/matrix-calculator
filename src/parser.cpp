#include <parser.hpp>
#include <matrix.hpp>
#include <iostream>
#include <string.h>
#include <ctype.h>

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
                continue;
            }
            else if (string[i] == '+') {
                // Do nothing
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
            prefix_set = 1;
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
    double number;
    try {
        number = number_parse(num_string, NULL);
    } catch (error_parse err) {
        throw err;
    }
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

parser_var operator + (const parser_var &a, const parser_var &b) {
    parser_var result; // Rezultat izračuna

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (
        a.type() == PARSER_VAR_OPERATOR ||
        a.type() == PARSER_VAR_FUNCTION ||
        b.type() == PARSER_VAR_OPERATOR ||
        b.type() == PARSER_VAR_FUNCTION
    ) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (a.type() == PARSER_VAR_OPERATOR || a.type() == PARSER_VAR_FUNCTION) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (
        a.type() == PARSER_VAR_OPERATOR ||
        a.type() == PARSER_VAR_FUNCTION ||
        b.type() == PARSER_VAR_OPERATOR ||
        b.type() == PARSER_VAR_FUNCTION
    ) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (a.type() == PARSER_VAR_OPERATOR || a.type() == PARSER_VAR_FUNCTION) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (
        a.type() == PARSER_VAR_OPERATOR ||
        a.type() == PARSER_VAR_FUNCTION ||
        b.type() == PARSER_VAR_OPERATOR ||
        b.type() == PARSER_VAR_FUNCTION
    ) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

    // Ako je a matrica i b matrica
    if (a.type() == PARSER_VAR_MATRIX && b.type() == PARSER_VAR_MATRIX) {
        try {
            result.mat(a.mat() * b.mat());
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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (a.type() == PARSER_VAR_OPERATOR || a.type() == PARSER_VAR_FUNCTION) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (
        a.type() == PARSER_VAR_OPERATOR ||
        a.type() == PARSER_VAR_FUNCTION ||
        b.type() == PARSER_VAR_OPERATOR ||
        b.type() == PARSER_VAR_FUNCTION
    ) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (a.type() == PARSER_VAR_OPERATOR || a.type() == PARSER_VAR_FUNCTION) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (a.type() == PARSER_VAR_OPERATOR || a.type() == PARSER_VAR_FUNCTION) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

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


extern parser_var operator ^ (const parser_var &a, const parser_var &b) {
    parser_var result; // Rezultat izračuna

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (
        a.type() == PARSER_VAR_OPERATOR ||
        a.type() == PARSER_VAR_FUNCTION ||
        b.type() == PARSER_VAR_OPERATOR ||
        b.type() == PARSER_VAR_FUNCTION
    ) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }
    // Provjeri je li eksponent broj
    if (b.type() == PARSER_VAR_MATRIX) {
        throw parser_var::error_calculation("Matrix cannot be an exponent");
    }
    // Provjeri je li baza broj
    else if (a.type() == PARSER_VAR_MATRIX) {
        throw parser_var::error_calculation("Matrix cannot be potentiated");
    }
    else if (a.type() == PARSER_VAR_NUMBER && b.type() == PARSER_VAR_NUMBER) {
        int exp = (int)b.num();             // Eksponent
        int exp_prefix = exp < 0 ? -1 : 1;  // Predznak eksponenta
        double temp_result = 1;             // Rezultat nakon fora
        int i;                              // Brojac
        // Ako eksponent nije cijeli broj
        if (exp != b.num()) {
            throw parser_var::error_calculation("Sorry, but only whole numbers are supported as an exponent");
        }
        // Ako je predznak eksponente - makni ga
        exp = exp < 0 ? -exp : exp;
        // Izracunaj rezultat
        for (i = 0; i < exp; i++) {
            temp_result *= a.num();
        }
        // Ako je predznak bio izracunaj 1 / temp_result
        if (exp_prefix == -1) {
            result.num(1.0 / temp_result);
        } else {
            result.num((double)temp_result);
        }
    } else {
        throw parser_var::error_calculation("One of variables is undefined");
    }

    return result;
}
extern parser_var operator ^ (const parser_var &a, const double n) {
    parser_var result; // Rezultat izračuna

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (a.type() == PARSER_VAR_OPERATOR || a.type() == PARSER_VAR_FUNCTION) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

    // Provjeri je li baza broj
    if (a.type() == PARSER_VAR_MATRIX) {
        throw parser_var::error_calculation("Matrix cannot be potentiated");
    }
    else if (a.type() == PARSER_VAR_NUMBER) {
        int exp = (int)n;                   // Eksponent
        int exp_prefix = exp < 0 ? -1 : 1;  // Predznak eksponenta
        double temp_result = 1;             // Rezultat nakon fora
        int i;                              // Brojac
        // Ako eksponent nije cijeli broj
        if (exp != n) {
            throw parser_var::error_calculation("Sorry, but only whole numbers are supported as an exponent");
        }
        // Ako je predznak eksponente - makni ga
        exp = exp < 0 ? -exp : exp;
        // Izracunaj rezultat
        for (i = 0; i < exp; i++) {
            temp_result *= a.num();
        }
        // Ako je predznak bio izracunaj 1 / temp_result
        if (exp_prefix == -1) {
            result.num(1.0 / temp_result);
        } else {
            result.num((double)temp_result);
        }
    } else {
        throw parser_var::error_calculation("One of variables is undefined");
    }

    return result;
}
extern parser_var operator ^ (const double n, const parser_var &a) {
    parser_var result; // Rezultat izračuna

    // Ako je u varijabli funkcija ili operator, baci grešku. Tip funkcija i operator
    // su namjenjeni isključivo internu uporabu unutar parsera, te je stoga računanje nemoguće
    if (a.type() == PARSER_VAR_OPERATOR || a.type() == PARSER_VAR_FUNCTION) {
        throw parser_var::error_calculation("Variables of type operator/function are for internal use only");
    }

    // Provjeri je li eksponent broj
    if (a.type() == PARSER_VAR_MATRIX) {
        throw parser_var::error_calculation("Matrix cannot be an exponent");
    }
    else if (a.type() == PARSER_VAR_NUMBER) {
        int exp = (int)a.num();             // Eksponent
        int exp_prefix = exp < 0 ? -1 : 1;  // Predznak eksponenta
        double temp_result = 1;             // Rezultat nakon fora
        int i;                              // Brojac
        // Ako eksponent nije cijeli broj
        if (exp != a.num()) {
            throw parser_var::error_calculation("Sorry, but only whole numbers are supported as an exponent");
        }
        // Ako je predznak eksponente - makni ga
        exp = exp < 0 ? -exp : exp;
        // Izracunaj rezultat
        for (i = 0; i < exp; i++) {
            temp_result *= n;
        }
        // Ako je predznak bio izracunaj 1 / temp_result
        if (exp_prefix == -1) {
            result.num(1.0 / temp_result);
        } else {
            result.num(temp_result);
        }
    } else {
        throw parser_var::error_calculation("One of variables is undefined");
    }

    return result;
}


parser_var &parser::variable(const char name[])  {
    int i;
    parser_var *var_ptr = variables;

    // Ako već postoji varijabla sa danim imenom vrati referencu na nju
    while (1) {
        if (var_ptr == NULL)
            break;
        if (!strcmp(name, var_ptr->variable_name))
            return *var_ptr;

        var_ptr = var_ptr->next_var;
    }

    // Testiraj je li ime varijable ispravno
    if (isdigit(name[0]))
        throw error_invalid("Variable name cannot begin with digit");
    for (i = 0; name[i] != '\0'; i++) {
        if (!isdigit(name[i]) && !isupper(name[i]) && !islower(name[i]))
            throw error_invalid("Variable name is invalid, it can only contain letters and numbers");
    }
    
    // Ako varijabla ne postoji kreiraj je i vrati referencu na nju
    var_ptr = variables;
    while (1) {
        if (variables == NULL) {
            variables = new parser_var;
            variables->variable_name = new char[strlen(name)];
            strcpy(variables->variable_name, name);
            return *variables;
        } else if (var_ptr->next_var == NULL) {
            var_ptr->next_var = new parser_var;
            var_ptr->next_var->variable_name = new char[strlen(name)];
            strcpy(var_ptr->next_var->variable_name, name);
            return *var_ptr->next_var;
        }
        var_ptr = var_ptr->next_var;
    }
}

void parser::del_variable(const char name[]) {
    parser_var *var_ptr = variables;

    // Ako je prva varijabla ona koju treba obrisati
    if (!strcmp(name, variables->variable_name)) {
        parser_var *temp_ptr = variables;
        variables = variables->next_var;
        delete temp_ptr;
        return;
    }
    // Ako nije prva nastavljamo dalje tražeći onu koju treba obrisati
    while (1) {
        // Ako je nismo našli ništa
        if (var_ptr->next_var == NULL)
            break;
        // Ako je sljedeća varijabla tražena
        if (!strcmp(name, var_ptr->next_var->variable_name)) {
            parser_var *temp_ptr = var_ptr->next_var;
            var_ptr->next_var = var_ptr->next_var->next_var;
            delete temp_ptr;
            return;
        }
        var_ptr = var_ptr->next_var;
    }
}

void parser::del_all_variables() {
    while (1) {
        if (variables == NULL)
            break;
        
        parser_var *var_ptr = variables;
        variables = variables->next_var;
        delete var_ptr;
    }
}

int parser::length() {
    int counter = 0;
    parser_var *var_ptr = variables;

    while (1) {
        if (var_ptr == NULL)
            return counter;
        ++counter;
        var_ptr = var_ptr->next_var;
    }
}

parser_var &parser::operator [] (int index) {
    int counter = 0;
    parser_var *var_ptr = variables;

    while (1) {
        if (var_ptr == NULL)
            throw error_undefined("Specified element does not exist");
        if (counter == index)
            return *var_ptr;
        var_ptr = var_ptr->next_var;
        ++counter;
    }
}

/*
 * Privatne funkcije za upravljanje stackom i queueom za calculate funkciju
 */

parser_var *parser::add_to_stack(const int operator_code) {
    parser_var *temp_ptr = operator_stack;
    operator_stack = new parser_var;
    operator_stack->next_var = temp_ptr;
    operator_stack->var_type = PARSER_VAR_OPERATOR;
    operator_stack->opr_code = operator_code;
    return operator_stack;
}
parser_var *parser::pop_from_stack() {
    if (operator_stack == NULL) {
        return NULL;
    } else {
        parser_var *pop_result = operator_stack;
        operator_stack = operator_stack->next_var;
        return pop_result;
    }
}
parser_var *parser::pop_from_queue() {
    if (output_queue == NULL) {
        return NULL;
    } else {
        parser_var *pop_result = output_queue;
        output_queue = output_queue->next_var;
        return pop_result;
    }
}
parser_var *parser::add_operator_to_queue(const int operator_code) {
    parser_var *var_ptr = output_queue;
    // Ako je queue prazan dodaj prvi element
    if (output_queue == NULL) {
        output_queue = new parser_var;
        output_queue->var_type = PARSER_VAR_OPERATOR;
        output_queue->opr_code = operator_code;
    // Ako queue nije prazan nađi kraj queuea i stavi ga tam
    } else {
        while (1) {
            if (var_ptr->next_var == NULL) {
                var_ptr->next_var = new parser_var;
                var_ptr->next_var->var_type = PARSER_VAR_OPERATOR;
                var_ptr->next_var->opr_code = operator_code;
                break;
            }
            var_ptr = var_ptr->next_var;
        }
    }
    return output_queue;
}
parser_var *parser::add_function_to_queue(const int function_code) {
    parser_var *var_ptr = output_queue;
    // Ako je queue prazan dodaj prvi element
    if (output_queue == NULL) {
        output_queue = new parser_var;
        output_queue->var_type = PARSER_VAR_FUNCTION;
        output_queue->opr_code = function_code;
    // Ako queue nije prazan nađi kraj queuea i stavi ga tam
    } else {
        while (1) {
            if (var_ptr->next_var == NULL) {
                var_ptr->next_var = new parser_var;
                var_ptr->next_var->var_type = PARSER_VAR_FUNCTION;
                var_ptr->next_var->opr_code = function_code;
                break;
            }
            var_ptr = var_ptr->next_var;
        }
    }
    return output_queue;
}
parser_var *parser::add_variable_to_queue(parser_var *new_var_ptr) {
    parser_var *var_ptr = output_queue;
    // Ako je queue prazan dodaj prvi element
    if (output_queue == NULL) {
        output_queue = new_var_ptr;
    // Ako queue nije prazan nađi kraj queuea i stavi ga tam
    } else {
        while (1) {
            if (var_ptr->next_var == NULL) {
                var_ptr->next_var = new_var_ptr;
                var_ptr->next_var->next_var = NULL;
                break;
            }
            var_ptr = var_ptr->next_var;
        }
    }
    return output_queue;
}
parser_var *parser::add_to_temp_stack(parser_var *new_var_ptr) {
    parser_var *temp_ptr = temp_stack;
    temp_stack = new_var_ptr;
    temp_stack->next_var = temp_ptr;
    return temp_stack;
}
parser_var *parser::pop_from_temp_stack() {
    if (temp_stack == NULL) {
        return NULL;
    } else {
        parser_var *pop_result = temp_stack;
        temp_stack = temp_stack->next_var;
        return pop_result;
    }
}

void parser::clear_all_stacks() {
    while (operator_stack != NULL) {
        parser_var *var_ptr = pop_from_stack();
        delete var_ptr;
    }
    while (output_queue != NULL) {
        parser_var *var_ptr = pop_from_queue();
        delete var_ptr;
    }
    while (temp_stack != NULL) {
        parser_var *var_ptr = pop_from_temp_stack();
        delete var_ptr;
    }
}


parser_var *parser::find_variable(const char *name, const char *name_end) {
    parser_var *var_ptr = variables;

    while (1) {
        int var_found = 1;
        
        if (var_ptr == NULL)
            return NULL;

        int i = 0, j = 0;

        while(*name_end == ' ' || *name_end == '\t')
            ++name_end;
        
        while (1) {
            if (name[i] == ' ' || name[i] == '\t') {
                ++i;
                continue;
            }
            if (name[i] == '\0' || (name_end != NULL && name + i >= name_end)) {
                break;
            }
            if (var_ptr->variable_name[j] == '\0') {
                break;
            }
            if (name[i] != var_ptr->variable_name[j]) {
                var_found = 0;
                break;
            }
            i++;
            j++;
        }

        if ((name[i] == '\0' || name + i == name_end) && var_ptr->variable_name[j] == '\0') {
            var_found = 1;
        } else {
            var_found = 0;
        }

        if (var_found) {
            return var_ptr;
        }

        var_ptr = var_ptr->next_var;
    }
}




parser_var &parser::calculate(const char expression[]) {
    int length;
    int i, j;
    int number_is_next = 1;
    int number_prefix = 1;
    const char *number_string_start = expression;

    // Počisti stack/queue-ove ak je ostalo neš od prošlog računa
    clear_all_stacks();

    for (length = 0; expression[length] != '\0'; length++);

    for (i = 0; i < length; i++) {
        if (expression[i] == ' ' || expression[i] == '\t')
            continue;

        if (number_is_next) {
            if (expression[i] == '*' || expression[i] == '/' || expression[i] == '^' || expression[i] == ')')
                throw error_calculation("Failed to parse expression");
            // Ako je znak funkcija
            else if (expression[i] == 'T' && !isdigit(expression[i + 1]) && !isupper(expression[i + 1]) && !islower(expression[i + 1])) {
                switch (expression[i]) {
                    case 'T':
                        add_function_to_queue(PARSER_FUNCTION_TRANS);
                        break;
                }
                continue;
            }
            // Ako je znak predznak +
            else if (expression[i] == '-') {
                number_prefix *= -1;
                continue;
            }
            // Ako je znak predznak -
            else if (expression[i] == '+') {
                // Do nothing
                continue;
            }
            // Ako je znak otvorena zagrada
            else if (expression[i] == '(') {
                // Ako je predznak zagrade negativan
                if (number_prefix < 0) {
                    // Dodaj -1 u output queue
                    parser_var *var_ptr = new parser_var;
                    var_ptr->num(number_prefix);
                    add_variable_to_queue(var_ptr);
                    // Dodaj * na operator stack
                    add_to_stack(PARSER_OPERATOR_MUL);
                    // Vrati prefix na 1
                    number_prefix = 1;
                }
                add_to_stack(PARSER_OPERATOR_OPN);
                number_string_start = expression + i + 1;
                continue;
            }
            // Ako je znak znamenka
            else if (isdigit(expression[i])) {
                parser_var * var_ptr;
                double parsed_number;
                number_is_next = 0;
                // Pomakni se na sljedeći operator
                while (isdigit(expression[i]) || expression[i] == '.')
                    ++i;
                // Probaj parsat broj do operatora
                try {
                    parsed_number = number_parse(number_string_start, expression + i);
                } catch (calculator_error err) {
                    throw error_calculation(err.info());
                }
                // Spremi ga u queue
                var_ptr = new parser_var;
                var_ptr->num(parsed_number);
                add_variable_to_queue(var_ptr);
                --i;
                number_is_next = 0;
                continue;
            }
            // Ako je znak slovo
            else if (islower(expression[i]) || isupper(expression[i])) {
                // Ako je predznak varijable negativan
                if (number_prefix < 0) {
                    // Dodaj -1 u output queue
                    parser_var *var_ptr = new parser_var;
                    var_ptr->num(number_prefix);
                    add_variable_to_queue(var_ptr);
                    // Dodaj * na operator stack
                    add_to_stack(PARSER_OPERATOR_MUL);
                    // Vrati prefix na 1
                    number_prefix = 1;
                }
                number_string_start = expression + i;
                parser_var *var_ptr;
                // Pronađi kraj riječi
                while (islower(expression[i]) || isupper(expression[i]) || isdigit(expression[i]))
                    ++i;
                // Pogledaj postoji li varijabla tog naziva
                var_ptr = find_variable(number_string_start, expression + i);
                // Ako ne postoji baci grešku
                if (var_ptr == NULL) {
                    throw error_calculation("Variable not found");
                }
                parser_var *new_var = new parser_var;
                *new_var = *var_ptr;
                // u suprotnom dodaj ju u queue
                add_variable_to_queue(new_var);
                --i;
                number_is_next = 0;
                continue;
            }
        }
        else {
            if (expression[i] == ')') {
                while (1) {
                    if (operator_stack == NULL)
                        throw error_calculation("Failed to parse expression");
                    
                    parser_var *var_ptr = pop_from_stack();
                    if (var_ptr->opr_code == PARSER_OPERATOR_OPN)
                        break;
                    add_variable_to_queue(var_ptr);
                }
            }
            else if (expression[i] == '^') {
                add_to_stack(PARSER_OPERATOR_EXP);
                number_is_next = 1;
            }
            else if (expression[i] == '*') {
                while (operator_stack != NULL && operator_stack->opr_code == PARSER_OPERATOR_EXP) {
                    add_variable_to_queue(pop_from_stack());
                }
                add_to_stack(PARSER_OPERATOR_MUL);
                number_is_next = 1;
            }
            else if (expression[i] == '/') {
                while (operator_stack != NULL && operator_stack->opr_code == PARSER_OPERATOR_EXP) {
                    add_variable_to_queue(pop_from_stack());
                }
                add_to_stack(PARSER_OPERATOR_DIV);
                number_is_next = 1;
            }
            else if (expression[i] == '+') {
                while (
                    operator_stack != NULL && (
                    operator_stack->opr_code == PARSER_OPERATOR_EXP ||
                    operator_stack->opr_code == PARSER_OPERATOR_MUL ||
                    operator_stack->opr_code == PARSER_OPERATOR_DIV ||
                    operator_stack->opr_code == PARSER_OPERATOR_SUB )    // << ovo smo dodali
                ) {
                    add_variable_to_queue(pop_from_stack());
                }
                add_to_stack(PARSER_OPERATOR_ADD);
                number_is_next = 1;
            }
            else if (expression[i] == '-') {
                while (
                    operator_stack != NULL && (
                    operator_stack->opr_code == PARSER_OPERATOR_EXP ||
                    operator_stack->opr_code == PARSER_OPERATOR_MUL ||
                    operator_stack->opr_code == PARSER_OPERATOR_DIV ||
                    operator_stack->opr_code == PARSER_OPERATOR_SUB )    // << ovo smo dodali
                ) {
                    add_variable_to_queue(pop_from_stack());
                }
                add_to_stack(PARSER_OPERATOR_SUB);
                number_is_next = 1;
            }
            else {
                throw error_calculation("Failed to parse expression");
            }
            number_string_start = expression + i + 1;
            number_prefix = 1;
        }
    }

    while (operator_stack != NULL) {
        add_variable_to_queue(pop_from_stack());
    }

    // probni ispis
    /*while (output_queue != NULL) {
        parser_var *var_ptr = pop_from_queue();

        switch (var_ptr->type()) {
            case PARSER_VAR_OPERATOR:
                std::cout << "OPR: " << var_ptr->opr_code << "\n";
                break;
            case PARSER_VAR_FUNCTION:
                std::cout << "FUN: " << var_ptr->fun_code << "\n";
                break;
            case PARSER_VAR_NUMBER:
                std::cout << var_ptr->num() << "\n";
                break;
            case PARSER_VAR_MATRIX:
                std::cout << var_ptr->mat() << "\n";
                break;
        }
    }*/

    // Izračunaj expression pohranjen u queue-u
    while (output_queue != NULL) {
        parser_var *var_ptr;              // Sljedeći token iz queue
        var_ptr = pop_from_queue();

        switch (var_ptr->type()) {
            // Ako je token broj
            case PARSER_VAR_NUMBER:
                add_to_temp_stack(var_ptr);
                break;
            // Ako je token funkcija
            case PARSER_VAR_FUNCTION:
                add_to_temp_stack(var_ptr);
                break;
            // Ako je token matrica
            case PARSER_VAR_MATRIX:
                add_to_temp_stack(var_ptr);
                break;
            case PARSER_VAR_UNDEFINED:
                throw error_calculation("One of the variables is undefined");
                break;
            // Ako je token operator
            case PARSER_VAR_OPERATOR:
                parser_var *var_1, *var_2;
                // Ako stack operanada nije NULL uzmi operand 1 sa njega
                if (temp_stack == NULL) {
                    throw error_calculation("Something is wrong with operands");
                } else {
                    var_1 = pop_from_temp_stack();
                }
                // Ako stack operanada nije NULL uzmi operand 2 sa njega
                if (temp_stack == NULL) {
                    throw error_calculation("Something is wrong with operands");
                } else {
                    var_2 = pop_from_temp_stack();
                }
                // Pointer na varijablu u koju je pohranjen rezultat operacije
                parser_var *res_ptr = new parser_var;
                // Ako je ovo
                switch (var_ptr->opr_code) {
                    // Operator zbrajanja
                    case PARSER_OPERATOR_ADD:
                        // Probaj izračunat
                        try {
                            *res_ptr = (*var_2) + (*var_1);
                        } catch (calculator_error err) {
                            // Ako račun nije uspio, pobriši nove varijable i vrati grešku
                            delete var_1;
                            delete var_2;
                            delete res_ptr;
                            throw error_calculation(err.info());
                        }
                        // Uspjelo je pa spremi rezultat
                        delete var_1;
                        delete var_2;
                        add_to_temp_stack(res_ptr);
                        break;
                    // Operator oduzimanja
                    case PARSER_OPERATOR_SUB:
                        // Probaj izračunat
                        try {
                            *res_ptr = (*var_2) - (*var_1);
                        } catch (calculator_error err) {
                            // Ako račun nije uspio, pobriši nove varijable i vrati grešku
                            delete var_1;
                            delete var_2;
                            delete res_ptr;
                            throw error_calculation(err.info());
                        }
                        // Uspjelo je pa spremi rezultat
                        delete var_1;
                        delete var_2;
                        add_to_temp_stack(res_ptr);
                        break;
                    // Operator množenja
                    case PARSER_OPERATOR_MUL:
                        // Probaj izračunat
                        try {
                            *res_ptr = (*var_2) * (*var_1);
                        } catch (calculator_error err) {
                            // Ako račun nije uspio, pobriši nove varijable i vrati grešku
                            delete var_1;
                            delete var_2;
                            delete res_ptr;
                            throw error_calculation(err.info());
                        }
                        // Uspjelo je pa spremi rezultat
                        delete var_1;
                        delete var_2;
                        add_to_temp_stack(res_ptr);
                        break;
                    // Operator dijeljenja
                    case PARSER_OPERATOR_DIV:
                        // Probaj izračunat
                        try {
                            *res_ptr = (*var_2) / (*var_1);
                        } catch (calculator_error err) {
                            // Ako račun nije uspio, pobriši nove varijable i vrati grešku
                            delete var_1;
                            delete var_2;
                            delete res_ptr;
                            throw error_calculation(err.info());
                        }
                        // Uspjelo je pa spremi rezultat
                        delete var_1;
                        delete var_2;
                        add_to_temp_stack(res_ptr);
                        break;
                    // Operator eksponenta
                    case PARSER_OPERATOR_EXP:
                        // Ako je u eksponenti funkcija
                        if (var_1->type() == PARSER_VAR_FUNCTION) {
                            switch (var_1->fun_code) {
                                // Ako je to funkcija za transponiranje matrice
                                case PARSER_FUNCTION_TRANS:
                                    // Provjeri je li drugi operand matrica
                                    if (var_2->type() == PARSER_VAR_MATRIX) {
                                        parser_var var_2_trans;
                                        var_2_trans.mat(
                                            var_2->mat().trans()
                                        );
                                        *res_ptr = var_2_trans;
                                        delete var_1;
                                        delete var_2;
                                        add_to_temp_stack(res_ptr);
                                    } else {
                                        delete var_1;
                                        delete var_2;
                                        delete res_ptr;
                                        throw error_calculation("Function TRANS can only be used on matrices");
                                    }
                                    break;
                                default:
                                    delete var_1;
                                    delete var_2;
                                    delete res_ptr;
                                    throw error_calculation("Internal error, function not found");
                                    break;
                            }
                        } else {
                            // Probaj izračunat
                            try {
                                *res_ptr = (*var_2) ^ (*var_1);
                            } catch (calculator_error err) {
                                // Ako račun nije uspio, pobriši nove varijable i vrati grešku
                                delete var_1;
                                delete var_2;
                                delete res_ptr;
                                throw error_calculation(err.info());
                            }
                            // Uspjelo je pa spremi rezultat
                            delete var_1;
                            delete var_2;
                            add_to_temp_stack(res_ptr);
                        }
                        break;
                }
                break;
        }
    }

    res_var = *pop_from_temp_stack();
    clear_all_stacks();

    return res_var;
}