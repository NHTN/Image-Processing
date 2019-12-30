#include "Matrix.hpp"

Matrix::Matrix(const std::vector<std::vector<double> > &parameter)
{
    row = parameter.size();
    assert(row > 0);
    col = parameter[0].size();
    for (int i = 0; i < row; ++i)
    {
        std::vector<double> temp;
        for (int j = 0; j < col; ++j)
        {
            temp.push_back(parameter[i][j]);
        }
        matrix.push_back(temp);
    }
}

Matrix::Matrix(int row, int col)
{
    assert(row > 0 && col > 0);
    this->row = row;
    this->col = col;
    for (int i = 0; i < row; ++i)
    {
        std::vector<double> temp;
        for (int j = 0; j < col; ++j)
        {
            temp.push_back(0);
        }
        matrix.push_back(temp);
    }
}

Matrix::Matrix(const Matrix &parameter)
{
    this->row = parameter.row;
    this->col = parameter.col;
    for (int i = 0; i < row; ++i)
    {
        std::vector<double> temp;
        for (int j = 0; j < col; ++j)
        {
            temp.push_back(parameter.matrix[i][j]);
        }
        matrix.push_back(temp);
    }
}

const Matrix & Matrix::operator= (const Matrix &parameter)
{
    for (int i = 0; i < row; ++i)
    {
        matrix[i].clear();
    }
    matrix.clear();
    this->row = parameter.row;
    this->col = parameter.col;
    for (int i = 0; i < row; ++i)
    {
        std::vector<double> temp;
        for (int j = 0; j < col; ++j)
        {
            temp.push_back(parameter.matrix[i][j]);
        }
        matrix.push_back(temp);
    }
    return *this;
}

Matrix::~Matrix()
{
    for (int i = 0; i < row; ++i)
        matrix[i].clear();
    matrix.clear();
}

int Matrix::getRow() const
{
    return row;
}

int Matrix::getCol() const
{
    return col;
}

double Matrix::operator() (int i, int j) const
{
    assert(i >= 0 && i < row);
    assert(j >= 0 && j < col);
    return matrix[i][j];
}

double & Matrix::operator() (int i, int j)
{
    assert(i >= 0 && i < row);
    assert(j >= 0 && j < col);
    return matrix[i][j];
}

Matrix Matrix::operator* (const Matrix &parameter) const
{
    Matrix result(row, parameter.col);
    assert(col == parameter.row);
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < parameter.col; ++j)
        {
            double s = 0;
            for (int k = 0; k < col; ++k)
            {
                s = s + matrix[i][k]*parameter.matrix[k][j];
            }
            result.matrix[i][j] = s;
        }
    }
    return result;
}

Matrix Matrix::operator* (double scale) const
{
    Matrix result(row, col);
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j)
        {
            result.matrix[i][j] = scale*matrix[i][j];
        }
    }
    return result;
}

Vector Matrix::operator* (const Vector &parameter) const
{
    assert(col == parameter.getLength());
    Vector result(row);
    for (int i = 0; i < row; ++i)
    {
        double s = 0;
        for (int j = 0; j < col; ++j)
        {
            s = s + matrix[i][j]*parameter[j];
        }
        result[i] = s;
    }
    return result;
}

Matrix Matrix::transpose() const
{
    Matrix result(col, row);
    for (int i = 0; i < col; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            result.matrix[i][j] = matrix[j][i];
        }
    }
    return result;
}

double Matrix::trace() const
{
    assert(row == col);
    double s = 0;
    for (int i = 0; i < row; ++i)
        s = s + matrix[i][i];
    return s;
}

std::vector<Matrix> Matrix::QR() const
{
    std::vector<Matrix> result;
    assert(row == col);
    int dimension = row;
    Matrix Q(dimension, dimension);
    Matrix R(dimension, dimension);
    std::vector<Vector> columns;
    for (int i = 0; i < dimension; ++i)
    {
        Vector xi(dimension);
        for (int j = 0; j < dimension; ++j)
        {
            xi[j] = matrix[i][j];
        }
        columns.push_back(xi);
    }
    std::vector<Vector> orthornormalized;
    for (int i = 0; i < dimension; ++i)
    {
        Vector sum(dimension);
        for (int k = 0; k < i; ++k)
        {
            sum = sum + (columns[i]*columns[k])*columns[k];
        }
        columns[i] = columns[i] - sum;
        columns[i].normalize();
    }
    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
        {
            Q(i, j) = columns[j][i];
        }
    }
    result.push_back(Q);
    R = Q.transpose()*(*this);
    result.push_back(R);
    return result;
}

std::vector<double> Matrix::eigenvalues(double eps) const
{
    std::vector<double> result;
    assert(row == col);
    int dimension = row;
    int iterationMax = 100;
    int count = 0;
    std::vector<double> prev;
    for (int i = 0; i < dimension; ++i)
    {
        result.push_back(matrix[i][i]);
    }
    Matrix updated = *this;
    while(count < iterationMax)
    {
        count++;
        prev = result;
        std::vector<Matrix> temp = updated.QR();
        Matrix Q = temp[0];
        Matrix R = temp[1];
        updated = R*Q;
        for (int i = 0; i < dimension; ++i)
        {
            result[i] = updated.matrix[i][i];
        }
        double s = 0;
        for (int i = 0; i < dimension; ++i)
        {
            s = s + (result[i] - prev[i])*(result[i] - prev[i]);
        }
        std::cout << "count = " << count << ", error = " << s << std::endl;
        if (sqrt(s) < eps) break;
    }
    return result;
}

Eigenstruct Matrix::eigensystem(double eps) const
{
    assert(row == col);
    int dimension = row;
    int iterationMax = 100;
    int count = 0;
    std::vector<double> prev;
    std::vector<double> result;
    for (int i = 0; i < dimension; ++i)
    {
        result.push_back(matrix[i][i]);
    }
    Matrix updated = *this;
    std::vector<Matrix> QMatrices;
    while(count < iterationMax)
    {
        count++;
        prev = result;
        std::vector<Matrix> temp = updated.QR();
        Matrix Q = temp[0];
        Matrix R = temp[1];
        QMatrices.push_back(Q);
        updated = R*Q;
        for (int i = 0; i < dimension; ++i)
        {
            result[i] = updated.matrix[i][i];
        }
        double s = 0;
        for (int i = 0; i < dimension; ++i)
        {
            s = s + (result[i] - prev[i])*(result[i] - prev[i]);
        }
        //std::cout << "count = " << count << ", error = " << s << std::endl;
        if (sqrt(s) < eps) break;
    }
    Matrix O = QMatrices[0];
    std::cout << 0 << "\n";
    std::cout << O << std::endl;
    for (int i = 1; i < QMatrices.size(); ++i)
    {
        O = O*QMatrices[i];
        std::cout << i << "\n";
        std::cout << O << std::endl;
    }
    return Eigenstruct(dimension, result, O);
}

void Matrix::print() const
{
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j)
        {
            std::cout << matrix[i][j] << "  ";
        }
        std::cout << std::endl;
    }
}

std::ostream & operator<< (std::ostream & os, const Matrix &parameter)
{
    for (int i = 0; i < parameter.row; ++i)
    {
        for (int j = 0; j < parameter.col; ++j)
        {
            os << parameter.matrix[i][j] << "  ";
        }
        os << "\n";
    }
    return os;
}
