#ifndef MATRIXCLASS_MATRIX_HPP
#define MATRIXCLASS_MATRIX_HPP

#include <iostream>
#include <vector>
using namespace std;

class Matrix {
private:
    vector<vector<double>> Mat;
    
public:
    Matrix(){};  //默认构造函数
    Matrix(vector<vector<double>> mat): Mat(mat){}  //带参的构造函数
    ~Matrix() {};  //析构函数
    Matrix(const Matrix &rhs);  //复制构造函数

    //mutators & accessors
    int getnum(int r, int c)const {return Mat[r][c];};   //返回第ij元素
    int getrow()const {return Mat.size();};
    int getcol()const {return Mat[0].size();};

    //overloaded operators
    friend ostream & operator<<(ostream &os, const Matrix &rhs);  //重载<<
    Matrix operator+(const Matrix &rhs);
    Matrix operator=(const Matrix &rhs);

    //fuctions to calculate the determinant and inverse of a Matrix
    double algebraic_cofactor(int i, int j) const;
    double det() const;
    Matrix inv() const;
};

#endif //MATRIXCLASS_MATRIX_HPP