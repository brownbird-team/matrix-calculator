#ifndef CALCULATOR_ERROR_HPP
#define CALCULATOR_ERROR_HPP

// Maksimalna duljina opisa greške
#define MAX_ERROR_DESC_LEN 70

// Klasa koja se koristi kao parent za klase greške klasa parser i parser_var
class calculator_error {
    private:
        char description[MAX_ERROR_DESC_LEN];
    public:
        calculator_error(const char desc[]) {
            int i;
            for (i = 0; desc[i] != '\0' && i < MAX_ERROR_DESC_LEN - 1; i++)
                description[i] = desc[i];
            description[i] = '\0';
        }
        inline const char * info() const {
            return description;
        }
};

#endif