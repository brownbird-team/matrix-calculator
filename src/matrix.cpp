#include <matrix.hpp>

using namespace std;

/*
 * Constructors
 */

matrix::matrix(int number_rows, int number_cols) {
    int i;

    rows = number_rows;
    cols = number_cols;

    mat_ptr = new int*[rows];
    for (i = 0; i < rows; i++)
        mat_ptr[i] = new int[cols];
}

matrix::matrix(const matrix &other_mat) {
    int i, j;

    cols = other_mat.cols;
    rows = other_mat.rows;

    mat_ptr = new int*[rows];
    for (i = 0; i < rows; i++)
        mat_ptr[i] = new int[cols];

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

inline void matrix::set(int row, int col, int value) {
    if (col <= cols && row <= rows) {
        mat_ptr[row - 1][col - 1] = value;
    } else {
        throw new matrix::element_not_found;
    }
}

inline int matrix::get(int row, int col) const {
    if (col <= cols && row <= rows) {
        return mat_ptr[row - 1][col - 1];
    } else {
        throw new matrix::element_not_found;
    }
}

/*
 * Operatori za računanje s matricama
 */

matrix operator + (const matrix &a, const matrix &b) {
    if (a.cols == b.cols && a.rows == b.rows) {
        int i, j;
        matrix result(a.cols, a.rows);

        for (i = 1; i <= a.rows; i++) {
            for (j = 1; j <= a.cols; j++) {
                result.set(i, j, a.get(i, j) + b.get(i, j));
            }
        }
        return result;
    } else {
        throw new matrix::calculation_error;
    }
}

matrix operator - (const matrix &a, const matrix &b) {
    if (a.cols == b.cols && a.rows == b.rows) {
        int i, j;
        matrix result(a.cols, a.rows);

        for (i = 1; i <= a.rows; i++) {
            for (j = 1; j <= a.cols; j++) {
                result.set(i, j, a.get(i, j) - b.get(i, j));
            }
        }
        return result;
    } else {
        throw new matrix::calculation_error;
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
        throw new matrix::calculation_error;
    }
}

matrix operator * (const int n, const matrix &b) {
    int i, j;
    matrix result(b.cols, b.rows);

    for (i = 1; i <= b.rows; i++) {
        for (j = 1; j <= b.cols; j++) {
            result.set(i, j, n * b.get(i, j));
        }
    }
    return result;
}

matrix operator * (const matrix &a, const int n) {
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
