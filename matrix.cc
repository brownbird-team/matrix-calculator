#include "matrix.hh"

using namespace std;

/*
 * Constructors
 */

matrix::matrix(int dim_x, int dim_y) {
    int i;

    x = dim_x;
    y = dim_y;

    mat_ptr = new int*[y];
    for (i = 0; i < y; i++)
        mat_ptr[i] = new int[x];
}

matrix::matrix(const matrix &other_mat) {
    int i, j;

    x = other_mat.x;
    y = other_mat.y;

    mat_ptr = new int*[y];
    for (i = 0; i < y; i++)
        mat_ptr[i] = new int[x];

    for (i = 1; i <= y && i <= other_mat.y; i++) {
        for (j = 1; j <= x && j <= other_mat.x; j++) {
            set(j, i, other_mat.get(j, i));
        }
    }
}

matrix::~matrix() {
    int i;

    for (i = 0; i < y; i++)
        delete mat_ptr[i];
    delete mat_ptr;
}

/*
 * Funkcije za postavljanje i dobivanje elementa matrice
 */

inline void matrix::set(int dim_x, int dim_y, int value) {
    if (dim_x <= x && dim_y <= y) {
        mat_ptr[dim_y - 1][dim_x - 1] = value;
    } else {
        throw new matrix::element_not_found;
    }
}

inline int matrix::get(int dim_x, int dim_y) const {
    if (dim_x <= x && dim_y <= y) {
        return mat_ptr[dim_y - 1][dim_x - 1];
    } else {
        throw new matrix::element_not_found;
    }
}

/*
 * Operatori za računanje s matricama
 */

inline matrix operator + (const matrix &a, const matrix &b) {
    if (a.x == b.x && a.y == b.y) {
        int i, j;
        matrix result(a.x, a.y);

        for (i = 1; i <= a.y; i++) {
            for (j = 1; j <= a.x; j++) {
                result.set(j, i, a.get(j, i) + b.get(j, i));
            }
        }
        return result;
    } else {
        throw new matrix::calculation_error;
    }
}

inline matrix operator - (const matrix &a, const matrix &b) {
    if (a.x == b.x && a.y == b.y) {
        int i, j;
        matrix result(a.x, a.y);

        for (i = 1; i <= a.y; i++) {
            for (j = 1; j <= a.x; j++) {
                result.set(j, i, a.get(j, i) - b.get(j, i));
            }
        }
        return result;
    } else {
        throw new matrix::calculation_error;
    }
}

inline matrix operator * (const matrix &a, const matrix &b) {
    if (a.x == b.y) {
        matrix res(a.y, b.x);
        int i, j, k;
        int n = a.x;

        for (i = 1; i <= res.y; i++) {
            for (j = 1; j <= res.x; j++) {
                int element = 0;
                for (k = 1; k <= n; k++) {
                    element += a.get(k, i) * b.get(j, k);
                }

                res.set(j, i, element);
            }
        }

        return res;
    } else {
        throw new matrix::calculation_error;
    }
}

inline matrix operator * (const int n, const matrix &b) {
    int i, j;
    matrix result(b.x, b.y);

    for (i = 1; i <= b.y; i++) {
        for (j = 1; j <= b.x; j++) {
            result.set(j, i, n * b.get(j, i));
        }
    }
    return result;
}

inline matrix operator * (const matrix &a, const int n) {
    try {
        return n * a;    
    } catch (matrix::calculation_error err) {
        throw err;
    }
    
}

/*
 * Operator za ispis matrice
 */

inline ostream &operator << (ostream &out_file, const matrix &mat) {
    int i, j;

    cout << "/ " << std::setw(mat.x * 7) << ' ' << " \\\n";

    for (i = 1; i <= mat.y; i++) {
        cout << "| ";
        for (j = 1; j <= mat.x; j++) {
            cout << std::setw(6) << mat.get(j, i) << ' ';
        }
        cout << " |\n";
    }

    cout << "\\ " << std::setw(mat.x * 7) << ' ' << " /\n";

    return cout;
}
