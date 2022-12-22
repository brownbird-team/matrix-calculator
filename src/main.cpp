#include <iostream>
#include <iomanip>
#include <string.h>

#include <matrix.hpp>
#include <parser.hpp>
#include <calculator_error.hpp>

#define MAX_INPUT 200

using namespace std;

int length;
char input_array[MAX_INPUT];
char *input;

void trim_input() {
    int i;
    input = input_array;

    while (*input == ' ' || *input == '\t')
        ++input;

    for (i = strlen(input) - 1; i > 0 && (input[i] == ' ' || input[i] == '\t'); i--)
        input[i] = '\0';
}

void take_input() {
    cin.getline(input_array, MAX_INPUT, '\n');
}

int main() {
    
    cout << " __  __       _        _         ____      _      \n";
    cout << "|  \\/  | __ _| |_ _ __(_)_  __  / ___|__ _| | ___ \n";
    cout << "| |\\/| |/ _` | __| '__| \\ \\/ / | |   / _` | |/ __|\n";
    cout << "| |  | | (_| | |_| |  | |>  <  | |__| (_| | | (__ \n";
    cout << "|_|  |_|\\__,_|\\__|_|  |_/_/\\_\\  \\____\\__,_|_|\\___|\n\n";

    cout << "Welcome to BrownBird matrix calculator\n";
    cout << "For list of all available commands type help\n\n";

    parser calc;

    while (1) {

        cout << "calculator> ";
        take_input();
        trim_input();

        //cout << "Upisali ste; \"" << input << "\"\n\n";
        
        if (strcmp(input, "help") == 0) {
            cout << "List of commands supported by matrix calculator:\n";
            cout << "- help                -- print help message\n";
            cout << "- set <variable>      -- set variable name and value\n";
            cout << "- del <variable>      -- delete variable of given name\n";
            cout << "- variables           -- list all existing variables\n";
            cout << "- print <variable>    -- show variable content\n";
            cout << "- analyse <variable>  -- analyze variable of given name\n";
            cout << "- calc <expression>   -- calculate given expression\n";
            cout << "- result              -- show result of last operation\n";
            cout << "- saveres <variable>  -- Move result of calculation into variable\n";
            cout << "- save <from> <to>    -- save value of from into to\n";
            cout << "- exit                -- exit calculator\n\n";
        } else if (strcmp(input, "exit") == 0) {
            cout << "Exiting calculator\nBye\n\n";
            break;
        } else if (strncmp(input, "set ", 4) == 0) {
            try {
                parser_var *var_ptr = &calc.variable(input + 4);

                while (1) {
                    cout << "Set type of variable [number/matrix] or c to cancel: ";
                    take_input();
                    trim_input();

                    if (strcmp(input, "c") == 0) {
                        cout << "Leaving variable " << input + 4 << " as it is\n";
                        break;
                    } else if (strcmp(input, "number") == 0 || strcmp(input, "n") == 0) {
                        cout << "Enter number to store in the variable: ";
                        take_input();
                        trim_input();
                        var_ptr->num(input);
                        cout << "Number stored successfully\n\n";
                        break;
                    } else if (strcmp(input, "matrix") == 0 || strcmp(input, "m") == 0) {
                        cout << "Enter matrix to store in the variable: ";
                        take_input();
                        trim_input();
                        var_ptr->mat(input);
                        cout << "Matrix stored successfully\n\n";
                        break;
                    }
                }
            } catch (calculator_error err) {
                cout << "Error: " << err.info() << "\n\n";
            }
        } else if (strncmp(input, "print ", 6) == 0) {
            switch (calc.variable(input + 6).type()) {
                case PARSER_VAR_UNDEFINED:
                    cout << "Variable is undefined\n\n";
                    break;
                case PARSER_VAR_NUMBER:
                    cout << "Number stored in the variable is: " << calc.variable(input + 6).num() << "\n\n";
                    break;
                case PARSER_VAR_MATRIX:
                    cout << "Matrix stored in the variable is:\n" << calc.variable(input + 6).mat() << "\n";
                    break;
            }
        } else if (strncmp(input, "del ", 4) == 0) {
            calc.del_variable(input + 4);
            cout << "Deleting variable " << input + 4 << " if it exists\n\n";
        } else if (strcmp(input, "variables") == 0) {
            int i;

            cout << "Variables defined:\n\n";

            for (i = 0; i < calc.length(); i++) {
                switch(calc[i].type()) {
                    case PARSER_VAR_NUMBER:
                        cout << "- [Number] -- " << calc[i].name() << "\n";
                        break;
                    case PARSER_VAR_MATRIX:
                        cout << "- [Matrix] -- " << calc[i].name() << "\n";
                        break;
                }
            }
            cout << "\n";

        } else if (strcmp(input, "result") == 0) {
            switch(calc.result().type()) {
                case PARSER_VAR_UNDEFINED:
                    cout << "Result is undefined, no calculations have been performed yet\n\n";
                    break;
                case PARSER_VAR_NUMBER:
                    cout << "Result of last calculation [Number]: " << calc.result().num() << "\n\n";
                    break;
                case PARSER_VAR_MATRIX:
                    cout << "Rasult of last calculation [Matrix]:\n" << calc.result().mat() << "\n";
                    break;
            }
        } else if (strncmp(input, "calc ", 5) == 0) {
            try {
                calc.calculate(input + 5);
                switch(calc.result().type()) {
                    case PARSER_VAR_NUMBER:
                        cout << "Result [Number]: " << calc.result().num() << "\n\n";
                        break;
                    case PARSER_VAR_MATRIX:
                        cout << "Rasult [Matrix]:\n" << calc.result().mat() << "\n";
                        break;
                }
            } catch (calculator_error err) {
                cout << "Error: " << err.info() << "\n\n";
            }
        } else if (strncmp(input, "saveres ", 8) == 0) {
            try {
                calc.variable(input + 8) = calc.result();
                cout << "Saving result of last calculation into " << input + 8 << "\n\n";
            } catch (calculator_error err) {
                cout << "Error: " << err.info() << "\n\n";
            }
        } else if (strncmp(input, "save ", 5) == 0) {
            int i;
            char *arg1, *arg2;
            int space_cnt = 0;
            int length = strlen(input);

            for (i = 0; i < length; i++) {
                if (input[i] == ' ' && space_cnt == 0) {
                    ++space_cnt;
                    arg1 = input + i + 1;
                    input[i] = '\0';
                }
                if (input[i] == ' ' && space_cnt == 1) {
                    ++space_cnt;
                    arg2 = input + i + 1;
                    input[i] = '\0';
                }
                if (input[i] == ' ' && space_cnt == 2) {
                    input[i] = '\0';
                    break;
                } 
            }

            try {
                calc.variable(arg2) = calc.variable(arg1);
                cout << "Saving content of variable " << arg1 << " into " << arg2 << "\n\n";
            } catch (calculator_error err) {
                cout << "Error: " << err.info() << "\n\n";
            }
        } else if (strncmp(input, "analyse ", 8) == 0) {
            switch (calc.variable(input + 8).type()) {
                case PARSER_VAR_UNDEFINED:
                    cout << "Variable is undefined\n\n";
                    break;
                case PARSER_VAR_NUMBER:
                    cout << "Variable is a number, nothing to analyse\n\n";
                    break;
                case PARSER_VAR_MATRIX:
                    cout << "Analysing matrix:\n";
                    cout << "- Square matrix:     " << ((calc.variable(input + 8).mat().is_squared()) ? "True" : "False") << "\n";
                    cout << "- Null:              " << ((calc.variable(input + 8).mat().is_nul()) ? "True" : "False") << "\n";
                    cout << "- Identity:          " << ((calc.variable(input + 8).mat().is_identity()) ? "True" : "False") << "\n";
                    cout << "- Upper triangular:  " << ((calc.variable(input + 8).mat().is_upper_triangular()) ? "True" : "False") << "\n";
                    cout << "- Lower triangular:  " << ((calc.variable(input + 8).mat().is_lower_triangular()) ? "True" : "False") << "\n";
                    cout << "- Diagonal:          " << ((calc.variable(input + 8).mat().is_diagonal()) ? "True" : "False") << "\n";
                    cout << "- Simetrical:        " << ((calc.variable(input + 8).mat().is_simetrical()) ? "True" : "False") << "\n";
                    cout << "- Antisimetrical:    " << ((calc.variable(input + 8).mat().is_antisimetrical()) ? "True" : "False") << "\n";
                    cout << "- Ortogonal:         " << ((calc.variable(input + 8).mat().is_ortogonal()) ? "True" : "False") << "\n";
            }
        } else if (strlen(input) > 0) {
            cout << "Command not found\n\n";
        }
    }
    
    return 0;
}
