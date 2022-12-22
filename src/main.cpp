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
            cout << "- analyze <variable>  -- analyze variable of given name\n";
            cout << "- calc <expression>   -- calculate given expression\n";
            cout << "- result              -- show result of last operation\n";
            cout << "- save <variable>     -- Move result of calculation into variable\n";
            cout << "- save <to> <from>    -- save value of from into to\n";
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

        } else {
            cout << "Command not found\n\n";
        }
    }
    
    return 0;
}
