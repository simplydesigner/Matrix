#ifndef matrix_hpp
#define matrix_hpp

#include <iostream>
#include <exception>
#include <sstream>
#include <fstream>

template <typename T>
class matrix;

template <typename T>
std::ostream & operator<<(std::ostream & output, const matrix<T> &);

template <typename T>
std::istream & operator>>(std::istream & input, matrix<T> &);

template <typename T>
class matrix
{
public:
    matrix(const matrix & _matrix);
    matrix(unsigned int rows, unsigned int columns);
    virtual ~matrix() noexcept;
    auto rows() const noexcept -> unsigned int;
    auto columns() const noexcept -> unsigned int;
    auto operator[](unsigned int index) const -> const int *;
    auto operator*(const matrix &_matrix) const -> matrix;
    auto operator+(const matrix &_matrix) const -> matrix;
    auto operator==(const matrix &_matrix) const noexcept -> bool;
    auto operator=(const matrix &_matrix) -> matrix &;

    friend std::ostream & operator<< <>(std::ostream & output, const matrix<T> & _matrix);
    friend std::istream & operator>> <>(std::istream & input, matrix<T> & _matrix);
private:
    unsigned int rows_, columns_;
    T **elements_;

    matrix(unsigned int rows, unsigned int columns, T **elements);
    auto swap(matrix & _matrix) -> void;
    auto fill(T **elements) -> void;

    auto throw_invalid_rows(unsigned int rows) const -> void;
    auto throw_invalid_size(unsigned int rows, unsigned int columns) const -> void;
    auto throw_out_of_range(unsigned index) const -> void;
    static auto throw_unknown(const char * _message) -> void;
    static auto throw_invalid_input_stream() -> void;
};

template <typename T>
auto matrix<T>::throw_invalid_rows(unsigned int rows) const -> void
{
    std::ostringstream stringStream;
    stringStream << "matrix has " << rows << " rows but expected " << rows_;
    throw std::invalid_argument(stringStream.str());
}

template <typename T>
auto matrix<T>::throw_invalid_size(unsigned int rows, unsigned int columns) const -> void
{
    std::ostringstream stringStream;
    stringStream << "matrix has " << rows << "x" << columns << " size but expected " << rows_ << " x " << columns_;
    throw std::invalid_argument(stringStream.str());
}

template <typename T>
auto matrix<T>::throw_out_of_range(unsigned index) const -> void
{
    std::ostringstream stringStream;
    stringStream << "index " << index << " beyond bounds [0.." << rows_ - 1 << "]";

    throw std::invalid_argument(stringStream.str());
}

template <typename T>
auto matrix<T>::throw_invalid_input_stream() -> void
{
    throw std::invalid_argument("invalid input stream");
}

template <typename T>
auto matrix<T>::throw_unknown(const char * _message) -> void
{
    throw std::logic_error(_message);
}

template <typename T>
matrix<T>::matrix(const matrix<T> &_matrix) : rows_(_matrix.rows_), columns_(_matrix.columns_)
{
    fill(_matrix.elements_);
}

template <typename T>
matrix<T>::matrix(unsigned int rows, unsigned int columns) : rows_(rows), columns_(columns)
{
    fill(nullptr);
}

template <typename T>
matrix<T>::matrix(unsigned int rows, unsigned int columns, T **elements) : rows_(rows), columns_(columns)
{
    fill(elements);
}

template <typename T>
auto matrix<T>::fill(T **elements) -> void
{
    elements_ = new T *[rows_];
    for (unsigned int i = 0; i < rows_; ++i) {
        elements_[i] = new T[columns_];
        for (unsigned int j = 0; j < columns_; ++j) {
            elements_[i][j] = elements ? elements[i][j] : 0;
        }
    }
}

template <typename T>
auto matrix<T>::operator=(const matrix & _matrix) -> matrix &
{
    if ( this != &_matrix ) {
        matrix(_matrix).swap(*this);
    }

    return *this;
}

template <typename T>
auto matrix<T>::swap(matrix & _matrix) -> void
{
    std::swap(rows_, _matrix.rows_);
    std::swap(columns_, _matrix.columns_);
    std::swap(elements_, _matrix.elements_);
}

template <typename T>
matrix<T>::~matrix() noexcept
{
    for (unsigned int i = 0; i < rows_; ++i) {
        delete [] elements_[i];
    }

    delete [] elements_;
}

template <typename T>
auto matrix<T>::rows() const noexcept -> unsigned int
{
    return rows_;
}

template <typename T>
auto matrix<T>::columns() const noexcept -> unsigned int
{
    return columns_;
}

template <typename T>
auto matrix<T>::operator[](unsigned int index) const -> const int *
{
    if ( index >= rows_ ) {
        throw_out_of_range(index);
    }

    return elements_[index];
}

template <typename T>
auto matrix<T>::operator*(const matrix & _matrix) const -> matrix
{
    if ( columns_ != _matrix.rows_ ) {
        throw_invalid_rows(_matrix.rows_);
    }

    unsigned int n = rows_;
    unsigned int m = _matrix.columns_;
    unsigned int s = columns_;

    T **elements = new T *[n];
    for (unsigned int i = 0; i < n; ++i) {
        elements[i] = new T[m];
        for (unsigned int j = 0; j < m; ++j) {
            T value = 0;
            for (unsigned int k = 0; k < s; ++k) {
                value += elements_[i][k] * _matrix.elements_[k][j];
            }
            elements[i][j] = value;
        }
    }

    return matrix(n, m, elements);
}

template <typename T>
auto matrix<T>::operator+(const matrix & _matrix) const -> matrix
{
    if ( rows_ != _matrix.rows_ || columns_ != _matrix.columns_ ) {
        throw_invalid_size(_matrix.rows_, _matrix.columns_);
    }

    unsigned int n = rows_;
    unsigned int m = columns_;

    T **data = new T *[n];
    for (unsigned int i = 0; i < n; ++i) {
        data[i] = new T[m];
        for (unsigned int j = 0; j < m; ++j) {
            data[i][j] = elements_[i][j] + _matrix.elements_[i][j];
        }
    }

    return matrix(n, m, data);
}

template <typename T>
auto matrix<T>::operator==(const matrix & _matrix) const noexcept -> bool
{
    if ( rows_ != _matrix.rows_ || columns_ != _matrix.columns_ ) {
        return false;
    }

    for (unsigned int i = 0; i < rows_; ++i) {
        for (unsigned int j = 0; j < columns_; ++j) {
            if ( elements_[i][j] != _matrix.elements_[i][j] ) {
                return false;
            }
        }
    }

    return true;
}

template <typename T>
std::ostream & operator<<(std::ostream & output, const matrix<T> & _matrix)
{
    for (unsigned int i = 0; i < _matrix.rows_; ++i) {
        for (unsigned int j = 0; j < _matrix.columns_; ++j) {
            if ( !(output << _matrix.elements_[i][j] << "\t") ) {
                matrix<T>::throw_unknown(__PRETTY_FUNCTION__);
            }
        }
        output << std::endl;
    }

    return output;
}

template <typename T>
std::istream & operator>>(std::istream & input, matrix<T> & _matrix)
{
    for (unsigned int i = 0; i < _matrix.rows_; ++i) {
        for (unsigned int j = 0; j < _matrix.columns_; ++j) {
            if ( !(input >> _matrix.elements_[i][j]) ) {
                matrix<T>::throw_invalid_input_stream();
            }
        }
    }

    return input;
}

#endif
