#ifndef CSR_MATRIX_HPP
#define CSR_MATRIX_HPP

#include <vector>
#include <exception>

namespace sjtu {

class size_mismatch : public std::exception {
public:
    const char *what() const noexcept override {
        return "Size mismatch";
    }
};

class invalid_index : public std::exception {
public:
    const char *what() const noexcept override {
        return "Index out of range";
    }
};

template <typename T>
class CSRMatrix {

private:
    size_t rows;
    size_t cols;
    size_t non_zeros;
    std::vector<size_t> indptr;
    std::vector<size_t> indices;
    std::vector<T> data;
    
public:
    // Assignment operators are deleted
    CSRMatrix &operator=(const CSRMatrix &other) = delete;
    CSRMatrix &operator=(CSRMatrix &&other) = delete;

    // Constructor for empty matrix with dimensions
    CSRMatrix(size_t n, size_t m) : rows(n), cols(m), non_zeros(0), indptr(n + 1, 0) {}

    // Constructor with pre-built CSR components
    CSRMatrix(size_t n, size_t m, size_t count,
        const std::vector<size_t> &indptr, 
        const std::vector<size_t> &indices,
        const std::vector<T> &data) : rows(n), cols(m), non_zeros(count), indptr(indptr), indices(indices), data(data) {
        if (indptr.size() != n + 1) throw size_mismatch();
        if (indices.size() != count) throw size_mismatch();
        if (data.size() != count) throw size_mismatch();
    }

    // Copy constructor
    CSRMatrix(const CSRMatrix &other) = default;

    // Move constructor
    CSRMatrix(CSRMatrix &&other) = default;

    // Constructor from dense matrix format (given as vector of vectors)
    CSRMatrix(size_t n, size_t m, const std::vector<std::vector<T>> &dense_data) : rows(n), cols(m), non_zeros(0) {
        if (dense_data.size() != n) throw size_mismatch();
        indptr.push_back(0);
        for (size_t i = 0; i < n; ++i) {
            if (dense_data[i].size() != m) throw size_mismatch();
            for (size_t j = 0; j < m; ++j) {
                if (dense_data[i][j] != T()) {
                    indices.push_back(j);
                    data.push_back(dense_data[i][j]);
                    non_zeros++;
                }
            }
            indptr.push_back(non_zeros);
        }
    }

    // Destructor
    ~CSRMatrix() = default;

    // Get dimensions and non-zero count
    size_t getRowSize() const { return rows; }

    size_t getColSize() const { return cols; }

    size_t getNonZeroCount() const { return non_zeros; }

    // Element access
    T get(size_t i, size_t j) const {
        if (i >= rows || j >= cols) throw invalid_index();
        size_t start = indptr[i];
        size_t end = indptr[i + 1];
        for (size_t k = start; k < end; ++k) {
            if (indices[k] == j) return data[k];
        }
        return T();
    }

    // Set element at position (i,j), updating CSR structure as needed
    void set(size_t i, size_t j, const T &value) {
        if (i >= rows || j >= cols) throw invalid_index();
        size_t start = indptr[i];
        size_t end = indptr[i + 1];
        for (size_t k = start; k < end; ++k) {
            if (indices[k] == j) {
                if (value == T()) {
                    // The problem says: "set(i,j,value) 无需删除元素。"
                    // So we just update the value even if it's T().
                    data[k] = value;
                } else {
                    data[k] = value;
                }
                return;
            }
            if (indices[k] > j) {
                indices.insert(indices.begin() + k, j);
                data.insert(data.begin() + k, value);
                for (size_t r = i + 1; r <= rows; ++r) {
                    indptr[r]++;
                }
                non_zeros++;
                return;
            }
        }
        indices.insert(indices.begin() + end, j);
        data.insert(data.begin() + end, value);
        for (size_t r = i + 1; r <= rows; ++r) {
            indptr[r]++;
        }
        non_zeros++;
    }

    // Access CSR components
    const std::vector<size_t> &getIndptr() const { return indptr; }

    const std::vector<size_t> &getIndices() const { return indices; }

    const std::vector<T> &getData() const { return data; }

    // Convert to dense matrix format
    std::vector<std::vector<T>> getMatrix() const {
        std::vector<std::vector<T>> dense(rows, std::vector<T>(cols, T()));
        for (size_t i = 0; i < rows; ++i) {
            size_t start = indptr[i];
            size_t end = indptr[i + 1];
            for (size_t k = start; k < end; ++k) {
                dense[i][indices[k]] = data[k];
            }
        }
        return dense;
    }

    // Matrix-vector multiplication
    std::vector<T> operator*(const std::vector<T> &vec) const {
        if (vec.size() != cols) throw size_mismatch();
        std::vector<T> res(rows, T());
        for (size_t i = 0; i < rows; ++i) {
            size_t start = indptr[i];
            size_t end = indptr[i + 1];
            for (size_t k = start; k < end; ++k) {
                res[i] = res[i] + data[k] * vec[indices[k]];
            }
        }
        return res;
    }

    // Row slicing
    CSRMatrix getRowSlice(size_t l, size_t r) const {
        if (l > r || r > rows) throw invalid_index();
        size_t new_rows = r - l;
        size_t start_idx = indptr[l];
        size_t end_idx = indptr[r];
        size_t new_count = end_idx - start_idx;
        
        std::vector<size_t> new_indptr(new_rows + 1, 0);
        for (size_t i = 0; i <= new_rows; ++i) {
            new_indptr[i] = indptr[l + i] - start_idx;
        }
        
        std::vector<size_t> new_indices(indices.begin() + start_idx, indices.begin() + end_idx);
        std::vector<T> new_data(data.begin() + start_idx, data.begin() + end_idx);
        
        return CSRMatrix(new_rows, cols, new_count, new_indptr, new_indices, new_data);
    }
};

}

#endif // CSR_MATRIX_HPP