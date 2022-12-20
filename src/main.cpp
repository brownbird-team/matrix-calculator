#include <iostream>
#include <iomanip>

#include <matrix.hpp>
#include <parser.hpp>
#include <calculator_error.hpp>

using namespace std;

int main() {
    // Testiranje parsera

    parser parse;

    parse.variable("Test").num(4);
    //parse.calculate("Test * 2");
    parse.variable("Mirko") = parse.variable("Test") * 2;
    try {
    parse.variable("matA").mat("{{1, 2},{-3, 4}}");
    } catch (calculator_error err) {
        cout << "PARSER MATRIX ERROR: " << err.info() << "\n";
    }
    parse.variable("matB").mat("{{1, -4},{3, 4}}");
    parse.variable("matC").mat("{{-2, -4},{8, 6}}");
    parse.variable("matD").mat("{{3, -4},{2, 5},{6, -11}}");
    parse.variable("TSTS").num("2");
    
    try {
        parse.calculate("matA ^ T");
        
        if (parse.result().type() == PARSER_VAR_NUMBER) {
            cout << "REZULTAT: " << parse.result().num() << "\n";
        } else {
            cout << "REZULTAT:\n" << parse.result().mat() << "\n";
        }
    } catch (calculator_error err) {
        cout << "PARSER ERROR: " << err.info() << "\n";
    }

    return 0;
}