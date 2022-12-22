#include <matrix.hpp>

using namespace std;

/*
 * Constructors
 */

matrix::matrix(int number_rows, int number_cols) {
    int i;

    rows = number_rows;
    cols = number_cols;

    mat_ptr = new double*[rows];
    for (i = 0; i < rows; i++)
        mat_ptr[i] = new double[cols];
}

matrix::matrix(const matrix &other_mat) {
    int i, j;

    cols = other_mat.cols;
    rows = other_mat.rows;

    mat_ptr = new double*[rows];
    for (i = 0; i < rows; i++)
        mat_ptr[i] = new double[cols];

    for (i = 1; i <= rows && i <= other_mat.rows; i++) {
        for (j = 1; j <= cols && j <= other_mat.cols; j++) {
            set(i, j, other_mat.get(i, j));
        }
    }
}

matrix::~matrix() {
    int i;

    for (i = 0; i < rows; i++)
        delete mat_ptr[i];
    delete mat_ptr;
}

/*
 * Funkcije za postavljanje i dobivanje elementa matrice
 */

inline void matrix::set(int row, int col, double value) {
    if (col <= cols && row <= rows) {
        mat_ptr[row - 1][col - 1] = value;
    } else {
        throw matrix::element_not_found("Given element doesn't exist");
    }
}

inline void matrix::set(int row, int col, int value) {
    if (col <= cols && row <= rows) {
        mat_ptr[row - 1][col - 1] = (double)value;
    } else {
        throw matrix::element_not_found("Given element doesn't exist");
    }
}

inline double matrix::get(int row, int col) const {
    if (col <= cols && row <= rows) {
        return mat_ptr[row - 1][col - 1];
    } else {
        throw matrix::element_not_found("Given element doesn't exist");
    }
}

/*
 * Funkcije za analiziranje matrica
 */

matrix::data matrix::analize_matrix() {
    data matrix_data;
    matrix_data.square = is_squared();
    matrix_data.nul = is_nul();
    matrix_data.identity = is_identity();
    matrix_data.upper_triangular = is_upper_triangular();
    matrix_data.lower_triangular = is_lower_triangular();
    matrix_data.diagonal = is_diagonal();
    matrix_data.simetrical = is_simetrical();
    matrix_data.antisimetrical = is_antisimetrical();
    matrix_data.ortogonal = is_ortogonal();
    return matrix_data;
}
int matrix::is_squared() {
    return rows == cols;
}
int matrix::is_ortogonal() {
    return ((*this)*trans()).is_identity() && (trans()*(*this)).is_identity(); 
}
int matrix::is_simetrical() {
    int rez = 1;
    if (rows != cols)
        return 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(mat_ptr[i][j] != mat_ptr[j][i]) {
                rez = 0;
            }
        }
    }
    return rez;
}
int matrix::is_antisimetrical() {
    int rez = 1;
    if (rows != cols)
        return 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(mat_ptr[i][j] != -mat_ptr[j][i]) {
                rez = 0;
            }
        }
    }
    return rez;
}
int matrix::is_diagonal() {
    return is_upper_triangular() && is_lower_triangular();
}
int matrix::is_lower_triangular() {
    int rez = 1;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(mat_ptr[i][j] != 0 && i < j) {
                rez = 0;
            }
        }
    }
    return rez && is_squared();
}
int matrix::is_upper_triangular() {
    int rez = 1;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(mat_ptr[i][j] != 0 && i > j) {
                rez = 0;
            }
        }
    }
    return rez && is_squared();
}
int matrix::is_identity() {
    int rez = 1;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(mat_ptr[i][j] != 1 && i == j) {
                rez = 0;
            }
            if(mat_ptr[i][j] != 0 && i != j) {
                rez = 0;
            }
        }
    }
    return rez && is_squared();
}
int matrix::is_nul() {
    int rez = 1;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(mat_ptr[i][j] != 0) {
                rez = 0;
            }
        }
    }
    return rez;
}

/*
 * Funkcije za vršenje operacija nad matricom (rezultat je nova matrica)
 */

matrix matrix::trans() const {
    int i, j;
    matrix transp(cols, rows);
    for(i = 1; i <= rows; i++) {
        for(j = 1; j <= cols; j++) {
            transp.set(j, i, get(i, j));
        }
    }
    return transp;
}

/*
 * Operatori za računanje s matricama
 */

matrix operator + (const matrix &a, const matrix &b) {
    if (a.cols == b.cols && a.rows == b.rows) {
        int i, j;
        matrix result(a.rows, a.cols);

        for (i = 1; i <= a.rows; i++) {
            for (j = 1; j <= a.cols; j++) {
                result.set(i, j, a.get(i, j) + b.get(i, j));
            }
        }
        return result;
    } else {
        throw matrix::calculation_error("Matrices must have same number of rows and cols when added");
    }
}

matrix operator - (const matrix &a, const matrix &b) {
    if (a.cols == b.cols && a.rows == b.rows) {
        int i, j;
        matrix result(a.rows, a.cols);

        for (i = 1; i <= a.rows; i++) {
            for (j = 1; j <= a.cols; j++) {
                result.set(i, j, a.get(i, j) - b.get(i, j));
            }
        }
        return result;
    } else {
        throw matrix::calculation_error("Matrices must have same number of rows and cols when subtracted");
    }
}

matrix operator * (const matrix &a, const matrix &b) {
    if (a.cols == b.rows) {
        matrix res(a.rows, b.cols);
        int i, j, k;
        int n = a.cols;

        for (i = 1; i <= res.rows; i++) {
            for (j = 1; j <= res.cols; j++) {
                int element = 0;
                for (k = 1; k <= n; k++) {
                    element += a.get(i, k) * b.get(k, j);
                }

                res.set(i, j, element);
            }
        }

        return res;
    } else {
        throw matrix::calculation_error("Matrices must be chained in order to be multiplied");
    }
}

matrix operator * (const double n, const matrix &b) {
    int i, j;
    matrix result(b.rows, b.cols);

    for (i = 1; i <= b.rows; i++) {
        for (j = 1; j <= b.cols; j++) {
            result.set(i, j, n * b.get(i, j));
        }
    }
    return result;
}

matrix operator * (const matrix &a, const double n) {
    try {
        return n * a;
    } catch (matrix::calculation_error err) {
        throw err;
    }
}

/*
 * Operator za ispis matrice
 */

ostream &operator << (ostream &out_file, const matrix &mat) {
    int i, j;

    cout << "/ " << std::setw(mat.cols * 7) << ' ' << " \\\n";

    for (i = 1; i <= mat.rows; i++) {
        cout << "| ";
        for (j = 1; j <= mat.cols; j++) {
            cout << std::setw(6) << mat.get(i, j) << ' ';
        }
        cout << " |\n";
    }

    cout << "\\ " << std::setw(mat.cols * 7) << ' ' << " /\n";

    return cout;
}
