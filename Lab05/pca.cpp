#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include "Matrix.hpp"
using namespace std;

vector<double> readFile(string fileName)
{
    ifstream ifile;
    ifile.open(fileName.c_str());
    if (!ifile.is_open())
    {
        cout << fileName + " does not exist. " << endl;
        exit(-1);
    }
    vector<double> data;
    double temp;
    while(!ifile.eof())
    {
        string line;
        getline(ifile, line);
        if (line != "")
        {
            data.push_back(stof(line));
        }
    }
    ifile.close();
    return data;
}

void printVector(const vector<double> &myVector)
{
    for (int i = 0; i < myVector.size(); ++i)
    {
        cout << myVector[i] << "  ";
    }
    cout << endl;
}

double sum(const vector<double> &myVector)
{
    double s = 0;
    for (int i = 0; i < myVector.size(); ++i)
    {
        s = s + myVector[i];
    }
    return s;
}

double mean(const vector<double> &A)
{
    int size = A.size();
    assert(size > 0);
    return sum(A)/float(size);
}

double getCovariance(const vector<double> &A, const vector<double> &B)
{
    assert(A.size() == B.size());
    assert(A.size() > 1);
    int size = A.size();
    double s = 0;
    double meanA = mean(A);
    double meanB = mean(B);
    for (int k = 0; k < size; ++k)
    {
        s = s + (A[k] - meanA)*(B[k] - meanB);
    }
    return s/float(size-1);
}

void printMatrix(const vector<vector<double> > &matrix)
{
    int row = matrix.size();
    assert(row > 0);
    int col = matrix[0].size();
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < col; ++j)
        {
            cout << matrix[i][j] << "  ";
        }
        cout << endl;
    }
}

Matrix getCovarianceMatrix(const vector<vector<double> > &Data)
{
    int row = Data.size();
    assert(row > 0);
    int col = Data[0].size();
    assert(col > 0);
    vector<vector<double> > cov(row, vector<double>(row, 0));
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            cov[i][j] = getCovariance(Data[i], Data[j]);
        }
    }
    return Matrix(cov);
}


int main()
{
    vector<vector<double> > Data;
    vector<string> fileNames;
    fileNames.push_back("X.txt");
    fileNames.push_back("Y.txt");
    for (int i = 0; i < fileNames.size(); ++i)
    {
        vector<double> data = readFile(fileNames[i]);
        Data.push_back(data);
    }
    Matrix covariance = getCovarianceMatrix(Data);
    cout << "Covariance matrix : " << endl;
    cout << covariance << endl; 
    vector<Matrix> QRDecomposition = covariance.QR();
    Matrix Q = QRDecomposition[0];
    Matrix R = QRDecomposition[1];
    cout << "Q: " << endl;
    cout << Q << endl;
    cout << "R: " << endl;
    cout << R << endl;
    cout << "Q*R = " << endl;
    cout << Q*R << endl;
    cout << "Q * Q^T = " << endl;
    cout << Q*Q.transpose() << endl;
    vector<double> eigenvalues = covariance.eigenvalues();
    cout << "eigenvalues of covaraince matrix : " << endl;
    printVector(eigenvalues);
    cout << "trace of covariance matrix = " << covariance.trace() << endl;
    cout << "sum of eigenvalues = " << sum(eigenvalues) << endl;
    Eigenstruct eigenstruct = covariance.eigensystem();
    cout << "Orthogonal transformation matrix is : " << endl;
    cout << eigenstruct.O << endl;
    cout << "O^T * COV * O : " << endl;
    cout << eigenstruct.O.transpose()*covariance*eigenstruct.O << endl;
    return 0;
}