#include "Matrix.hpp"
#include <math.h>
Matrix::Matrix(const Matrix &rhs)
{
    Mat = rhs.Mat;
    cout << "Copy Constructor" << endl;
}

ostream &operator<<(ostream &os, const Matrix &rhs)    //cout matrix的每个元素
{
    for(int i(0); i < rhs.getrow(); ++i)
    {
        for(int j(0); j < rhs.getcol(); ++j)
            os << rhs.Mat[i][j] << '\t';   
        os << endl;
    }
    return os;
}

Matrix Matrix::operator+(const Matrix &rhs)
{
    vector<vector<double>> temp(getrow(),vector<double>(getrow(),0));  //每个值初始化为0
    Matrix Temp(temp);
    if((getrow() != rhs.getrow()) || (getcol() != rhs.getcol()))
    {
        cout << "Addition cannot be done on these matrices (dims not equal)" << endl;
        exit(1);
    }
    for(unsigned int i(0); i < Temp.getrow(); i++)
        for(unsigned int j(0); j < Temp.getcol(); j++)
            Temp.Mat[i][j] = this->Mat[i][j] + rhs.Mat[i][j];
            cout << "Addition" << endl;
    return Temp;
}

Matrix Matrix::operator=(const Matrix &rhs)
{
    this->Mat.resize(rhs.getrow());

    for(int i(0); i < this->getrow(); ++i)
        this->Mat[i].resize(rhs.getcol());
    for(int i(0); i < this->getrow(); ++i)
    {
        for(int j(0); j < this->getcol(); ++j)
        {
            this->Mat[i][j] = rhs.Mat[i][j];
        }
    }
    return *this;
}

double Matrix::det() const
{
    double det = 0.0;
    if (this->getrow() == 1 && this->getcol() == 1)
    {
        det = this->Mat[0][0];
    }
    else
    {
        for (int j(0); j < this->getcol(); j++)
        {   
            det += this->Mat[0][j] * (this->algebraic_cofactor(0, j));   //使用代数余子式求解矩阵的行列式
        }
    }
    // std::cout<<det<<std::endl;
    return det;
}

double Matrix::algebraic_cofactor(int i, int j) const  // 代数余子式函数编写
{
    vector<vector<double>> temp(this->getrow()-1,vector<double>(this->getrow()-1,0.0));  //每个值初始化为0
    Matrix Temp(temp);  //用于存储余子式对应的子式
    int r_orginal = 0;  //用于将余子式的子式元素位置与原矩阵的位置对应的位置参数
    int c_orginal = 0;
    for(unsigned int r(0); r < Temp.getrow(); r++)
    {
        c_orginal = 0;
        for(unsigned int c(0); c < Temp.getcol(); c++)
        {
            if (r == i && r_orginal == i)   //若遇到余子式对应的行和列将跳过
            {
                r_orginal ++;
            }
            if (c == j && c_orginal == j)
            {
                c_orginal ++;

            }
            Temp.Mat[r][c] = this->Mat[r_orginal][c_orginal];
            c_orginal ++;
        }
        r_orginal ++;
    }
    return pow(-1, i+j) * Temp.det();  //代数余子式用行列式求解，进行迭代求解
}

Matrix Matrix::inv() const
{
    vector<vector<double>> temp(this->getrow(),vector<double>(this->getrow(),0.0));  //每个值初始化为0
    Matrix Temp(temp);  //用于存储逆矩阵的临时矩阵
    if (this->det() == 0.0)  //可逆与否的判断
    {
        cout << "can't transfer to inverse matrix" << endl;
        cout << "therefore this is a empty matrix" << endl;
    }
    else
    {
        for(unsigned int r(0); r < Temp.getrow(); r++)
        {
            for(unsigned int c(0); c < Temp.getcol(); ++c)
            {
                Temp.Mat[r][c] = this->algebraic_cofactor(c,r)/(this->det());  //代数余子式求逆矩阵
            }
        }
    }
    return Temp;
}