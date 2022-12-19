#include <iostream>
#include <iomanip>

#include <matrix.hpp>
#include <parser.hpp>
#include <calculator_error.hpp>

using namespace std;

int main() {
    // Testiranje parsera (varijable)

    parser parse;

    parse.variable("Krizan").mat("{{1, 2},{3, 4}}");

    cout << "Matrica Krizan:\n" << parse.variable("Krizan").mat();

    parse.variable("Krizan").num(5);

    cout << "\nBroj Krizan: " << parse.variable("Krizan").num() << "\n\n";

    cout << "\nBroj Krizan (preko indexa): " << parse[0].num() << "\n\n";

    parse.variable("Nadarevic").mat("{{--++1, ---1}{-1, -1}}");

    parse.variable("Test") = parse.variable("Nadarevic") * parse.variable("Krizan");

    cout << "\n\nUmnozak:\n" << parse.variable("Test").mat();

    cout << "Broj varijabli: " << parse.length() << "\n";

    parse.del_variable("Nadarevic");

    cout << "Broj varijabli: " << parse.length() << "\n";

    cout << "\n----------------------------------------------------\n";
    parse.variable("TEST").num(222);

    cout << "Proba parsera: \n";

    try {
    parse.calculate("3 + 2 * -(11 * (33 + 22)) * -TEST");
    } catch (calculator_error err) {
        cout << "PARSER ERROR: " << err.info() << "\n";
    }

    return 0;
}