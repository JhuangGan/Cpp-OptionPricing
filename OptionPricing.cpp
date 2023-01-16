#include "OptionPricing.hpp"
#include <cmath>

//Asset

// Stock
Stock::Stock(double nowPrice,  double returnRate, double sigma): Asset()
{
    this->ReturnRate = returnRate;
    this->Sigma = sigma;
    this->nowPrice = nowPrice;
}

Stock::Stock(Stock & rightstock):Asset()
{
    this->ReturnRate = rightstock.getReturnRate();
    this->Sigma =  rightstock.getSigma();
    this->nowPrice = rightstock.getnowPrice();
}

void Stock::Reset(double nowPrice, double returnRate, double sigma)
{
    this->ReturnRate = returnRate;
    this->Sigma = sigma;
    this->nowPrice = nowPrice;
}

// Option

Option::Option(Asset & asset, Market& market, double StrikePrice, std::string PutCall, double ExpiredTime):Asset()
{
    this->S0 = asset.getnowPrice();
    this->ReturnRate = asset.getReturnRate();
    this->Risk = market.risk;
    this->Sigma = asset.getSigma();
    this->ExpiredTime = ExpiredTime;
    this->StrikePrice = StrikePrice;
    this->PutCall = PutCall;
}

Option::Option(Option & rightop) 
{
    this->ReturnRate = rightop.getReturnRate();
    this->S0 = rightop.getS0();
    this->Risk = rightop.getRisk();
    this->Sigma = rightop.getSigma();
    this->ExpiredTime = rightop.getExpiredTime();
    this->StrikePrice = rightop.getStrikePrice();
    this->PutCall = rightop.getPutCall();
}

void Option::whatItis()
{
    std::cout<<"Market Risk-Free Rate: " << this->getRisk() << std::endl;
    std::cout<<"Object nowPrice: " << this->getS0() << std::endl;
    std::cout<<"Object Return Rate: " << this->getReturnRate() << std::endl;
    std::cout<<"Object Volatility: " << this->getSigma() << std::endl;
    std::cout<<"Object Strike Price: " << this->getStrikePrice() << std::endl;
    std::cout<<"Option Expired Time: " << this->getStrikePrice() << std::endl;
    std::cout<<"Option Type: " << this->getStrikePrice() << std::endl;
}


// European Option
EuropeanOption::EuropeanOption(Asset & asset, Market & market, double StrikePrice,double ExpiredTime, std::string PutCall):Option(asset, market, StrikePrice, PutCall,ExpiredTime)
{
    this->OptionType = "EuropeanOption";
}


EuropeanOption::EuropeanOption(EuropeanOption & europeanOption):Option(europeanOption)
{
    this->OptionType = europeanOption.getoptiontype();
}

double EuropeanOption::assetPricing(Engine & engine)
{
    return engine.CalValue(*this);
}


// American Option
AmericanOption::AmericanOption(Asset & asset, Market & market, double StrikePrice,double ExpiredTime, std::string PutCall):Option(asset,market,StrikePrice, PutCall, ExpiredTime)
{
    this->OptionType = "AmericanOption";
}

AmericanOption::AmericanOption(AmericanOption & rightAop):Option(rightAop)
{
    this->OptionType = rightAop.getoptiontype();
}

double AmericanOption::assetPricing(Engine & engine)
{
    return engine.CalValue(*this);
}

// double AmericanOption::assetPricing(Engine & engine)
// {
//     return engine.CalValue(*this);
// }

// // Engine 
// Engine::Engine(Option & op)
// {
//     this->op = &op;
// }

// BSmodel
BSmodel::BSmodel(Option & op):Engine()
{
    this->op = &op;
}

double cdfnorm(double x)
{
    return 0.5 * (1 + std::erf(x / std::sqrt(2)));
}

double BSmodel::CalValue(Option & op)
{
    double output = 0.0;
    double d1,d2;
    d1 = (std::log(op.getS0()/op.getStrikePrice())+((op.getRisk()-op.getReturnRate())+op.getSigma()*op.getSigma()/2)*op.getExpiredTime())/(op.getSigma()*std::sqrt(op.getExpiredTime()));
    d2 = d1 - op.getSigma()*std::sqrt(op.getExpiredTime());
    if (op.getPutCall() == "call")
    {
        output = op.getS0()*cdfnorm(d1)-op.getStrikePrice()*std::exp(-(op.getRisk()-op.getReturnRate())*op.getExpiredTime())*cdfnorm(d2);
    }
    else if (op.getPutCall() == "put") 
    {
        output = op.getStrikePrice()*std::exp(-(op.getRisk()-op.getReturnRate())*op.getExpiredTime())*cdfnorm(-d2)-op.getS0()*cdfnorm(-d1);
    }
    //期权实例化
    this->op = &op;
    return output;
}


// MonteCarlo
MonteCarlo::MonteCarlo(int iter)
{
    this->itert = iter;
}

MonteCarlo::MonteCarlo(Option & op, int iter /*= 100*/):Engine()
{
    this->itert = iter;
    this->op = &op;  //这里this->op应该不用再加解引用符号*吧,实验下来确实不用
}

double MonteCarlo:: CalValue(Option & op)
{
    if (op.getoptiontype() != "AmericanOption")
    {
        BSmodel bs;
        std::cout<<"it's not a AmericanOption"<<std::endl;
        std::cout<<"it's a "<<op.getoptiontype()<<"by BS model, price is"<<std::endl;
        return op.assetPricing(bs);
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    //也可以设置一个种子
    std::default_random_engine gen(seed);
    double StockTemp=0.0;
    double OptionTemp=0.0;
    double sumOp=0.0;
    std::vector<double> StockList;
    std::vector<double> OptionList;

    if (op.getPutCall() == "call")
    {
        for(int i(0); i<this->itert; ++i)
        {
            std::normal_distribution<double> dis(0,1);
            StockTemp = op.getS0()*exp(((op.getRisk()-op.getReturnRate())-op.getSigma()*op.getSigma()/2)*op.getExpiredTime() + op.getSigma()*dis(gen)*sqrt(op.getExpiredTime()));
            StockList.push_back(StockTemp);
            OptionTemp = exp(-(op.getRisk()-op.getReturnRate())*op.getExpiredTime())*std::max(op.getS0()*exp(((op.getRisk()-op.getReturnRate())-op.getSigma()*op.getSigma()/2)*op.getExpiredTime() + op.getSigma()*dis(gen)*sqrt(op.getExpiredTime()))-op.getStrikePrice(),0.0);
            OptionList.push_back(OptionTemp);
            sumOp += OptionTemp;
        }
    }
    else if (op.getPutCall() == "put") 
    {
        for(int i(0); i<itert; ++i)
        {
            std::normal_distribution<double> dis(0,1);
            StockTemp = op.getS0()*exp(((op.getRisk()-op.getReturnRate())-op.getSigma()*op.getSigma()/2)*op.getExpiredTime() + op.getSigma()*dis(gen)*sqrt(op.getExpiredTime()));
            StockList.push_back(StockTemp);
            OptionTemp = exp(-(op.getRisk()-op.getReturnRate())*op.getExpiredTime())*std::max(op.getStrikePrice()-op.getS0()*exp(((op.getRisk()-op.getReturnRate())-op.getSigma()*op.getSigma()/2)*op.getExpiredTime() + op.getSigma()*dis(gen)*sqrt(op.getExpiredTime())),0.0);
            OptionList.push_back(OptionTemp);
            sumOp += exp(-(op.getRisk()-op.getReturnRate())*op.getExpiredTime())*std::max(op.getStrikePrice()-op.getS0()*exp(((op.getRisk()-op.getReturnRate())-op.getSigma()*op.getSigma()/2)*op.getExpiredTime() + op.getSigma()*dis(gen)*sqrt(op.getExpiredTime())),0.0);
        }
    }
    // 属性赋值
    this->StockList = StockList;
    this->OptionList = OptionList;
    this->op = &op;  //这里this->op应该不用再加解引用符号*吧，不用

    return sumOp/itert;
}


// Binary Tree
BinaryTree::BinaryTree(Option & op, int iter /*= 100*/):Engine()
{
    this->itert=iter;
    this->op = &op;
}


double BinaryTree::CalValue(Option & op)
{
    std::vector<std::vector<double>> s(this->getIter()+1,std::vector<double>(this->getIter()+1,0.0));
    double u=std::exp(op.getSigma()*std::sqrt(op.getExpiredTime()/(this->getIter()-1)));
    double d=std::exp(-op.getSigma()*std::sqrt(op.getExpiredTime()/(this->getIter()-1)));
    double a=std::exp((op.getRisk()-op.getReturnRate())*(op.getExpiredTime()/(this->getIter()-1)));
    double p=(a-d)/(u-d);
    int i, j;
    for(i=1; i<=this->getIter(); i++)
    {
        for(j=0; j<=i-1; j++)
        {
            s[i][j]=op.getS0()*std::pow(u,j)*std::pow(d,i-j-1);
        }
    }
    if (op.getPutCall()=="call")
    {
        for(i=this->getIter()-1; i>=0; i--)
        {
            s[i][this->getIter()]=std::max(s[this->getIter()][i]-op.getStrikePrice(),0.0);
        }
        for(j=this->getIter()-1; j>=1; j--)
        {
            for(i=j-1; i>=0; i--)
            {
                s[i][j]=std::max(s[j][i]-op.getStrikePrice(),std::exp(-(op.getRisk()-op.getReturnRate())*(op.getExpiredTime()/(this->getIter()-1)))*(p*s[i+1][j+1]+(1-p)*s[i][j+1]));
            }
        }
    }
    else if (op.getPutCall()=="put")
    {
        for(i=this->getIter()-1; i>=0; i--)
        {
            s[i][this->getIter()]=std::max(-s[this->getIter()][i]+op.getStrikePrice(),0.0);
        }
        for(j=this->getIter()-1; j>=1; j--)
        {
            for(i=j-1; i>=0; i--)
            {
                s[i][j]=std::max(-s[j][i]+op.getStrikePrice(),std::exp(-(op.getRisk()-op.getReturnRate())*(op.getExpiredTime()/(this->getIter()-1)))*(p*s[i+1][j+1]+(1-p)*s[i][j+1]));
            }
        }
    }
    this->BT = s;  //将BT赋值s
    // 这里说明一下BT是以对角线为0，左下角三角存储股票价格二叉树数据，右上角存储以对角线对称对应的期权二叉树数据
    this->op = &op;
    return s[0][1];
}


// FD 有限差分--显式和隐式
FD::FD(std::string FDtype, int enlargefactor/*=2*/, int DN /*=20*/, double delta_s/*=0.1*/)
{
    this->FDtype = FDtype;
    this->enlargefactor = enlargefactor;
    this->DN = DN;
    this->delta_s = delta_s;
}

FD::FD(Option & op,std::string FDtype, int enlargefactor/*=2*/, int DN /*=20*/, double delta_s/*=0.1*/)
{
    this->FDtype = FDtype;
    this->enlargefactor = enlargefactor;
    this->DN = DN;
    this->delta_s = delta_s;
    this->op = &op;
}


// 需安装armadillo或eigen， 不用了，上课写的matrix可以求逆，从而可以solve方程
// 还是用eigen吧，上课写的那个求逆效率太低，一旦维数上去，时间复杂度太高
double FD::CalValue(Option & op)
{
    int Smax = std::floor(this->getenlargefactor()*op.getS0()); 
    double delta_S = this->getdelta_s();
    int M = std::floor(Smax/delta_S);
    std::vector<std::vector<double>> Option_result(M+1,std::vector<double>(this->getDN()+1,0.0));

    if (this->getFDtype()=="Implicit")
    {
/*
// 没安eigen的话，注释该部分：：：开始
        if (op.getPutCall()=="put")
        {
            for(int j = 0; j <= this->getDN(); j++)
            {
                Option_result[M][j] = op.getStrikePrice();
                Option_result[0][j] = 0.0;
            }
            for(int i = 0; i <= M; i++)
            {
                Option_result[i][this->getDN()] = std::max(op.getStrikePrice()-(M-i)*delta_S,0.0);
            }
        }
        else if(op.getPutCall()=="call")
        {
            for(int j = 0; j <= this->getDN(); j++)
            {
                Option_result[M][j] = 0.0;
                Option_result[0][j] = Smax-op.getStrikePrice();
            }
            for(int i = 0; i <= M; i++)
            {
                Option_result[i][this->getDN()] = std::max(-op.getStrikePrice()+(M-i)*delta_S,0.0);
            }
        }
        std::vector<double> a(M-1, 0.0);
        std::vector<double> b(M-1, 0.0);
        std::vector<double> c(M-1, 0.0);
        for(int k = 1; k <= M-1; k++)
        {
            a[k-1] = 0.5*(op.getRisk()-op.getReturnRate())*k*(op.getExpiredTime()/this->getDN())-0.5*op.getSigma()*op.getSigma()*k*k*(op.getExpiredTime()/this->getDN());
            b[k-1] = 1+op.getSigma()*op.getSigma()*k*k*(op.getExpiredTime()/this->getDN())+op.getRisk()*(op.getExpiredTime()/this->getDN());
            c[k-1] = -0.5*(op.getRisk()-op.getReturnRate())*k*(op.getExpiredTime()/this->getDN())-0.5*op.getSigma()*op.getSigma()*k*k*(op.getExpiredTime()/this->getDN());
        }
        std::vector<std::vector<double>> coef(M+1,std::vector<double>(M+1,0.0));
        coef[0][0]=1;
        coef[M][M]=1;
        for(int i = 1; i < M; i++)
        {
            coef[i][i-1] = a[i-1];
            coef[i][i] = b[i-1];
            coef[i][i+1] = c[i-1];
        }
        // 转为eigen向量格式：
        Eigen::MatrixXd coeff(M+1, M+1); 
        for(int i = 0 ; i < M+1 ; ++i) 
        { 
            for(int c = 0 ; c < M+1 ; ++c) 
            { 
                coeff(i, c) = coef[i][c]; 
            } 
        } 

        Eigen::VectorXd Y(M+1);

        double sum = 0;
        for(int k = this->getDN(); k > 0; k--)
        {
            for(int row(0); row < M+1; ++row)
            { 
                Y[row]=Option_result[row][k];
            }
            // y = Option_result.col(k);

            Eigen::VectorXd x = coeff.lu().solve(Y);
            // Option_result.col(k-1) = arma::solve(coef,y);
            // 将x的值赋给Option_result第k-1列
            for (int r(0); r < M+1; ++r)
            {
                Option_result[r][k-1] = x[r];
            }
        }
// 没安eigen的话，注释该部分：：：结束
*/
    }
    else if(getFDtype()=="Explicit")
    {
        if (op.getPutCall()=="put")
        {
            for(int j = 0; j <= this->getDN(); j++)
            {
                Option_result[M][j] = op.getStrikePrice();
                Option_result[0][j] = 0.0;
            }
            for(int i = 0; i <= M; i++)
            {
                Option_result[i][this->getDN()] = std::max(op.getStrikePrice()-(M-i)*delta_S,0.0);
            }
        }
        else if(op.getPutCall()=="call")
        {
            for(int j = 0; j <= this->getDN(); j++)
            {
                Option_result[M][j] = 0.0;
                Option_result[0][j] = Smax-op.getStrikePrice();
            }
            for(int i = 0; i <= M; i++)
            {
                Option_result[i][this->getDN()] = std::max(-op.getStrikePrice()+(M-i)*delta_S,0.0);
            }
        }
        std::vector<double> a(M-1, 0.0);
        std::vector<double> b(M-1, 0.0);
        std::vector<double> c(M-1, 0.0);
        for(int j = this->getDN()-1; j >=0 ; j--)
        {
            for(int i = 1; i <= M -1; i++)
            {
                double a = 1/(1+op.getRisk()*(op.getExpiredTime()/this->getDN()))*(-0.5*(op.getRisk()-op.getReturnRate())*(M-i)*(op.getExpiredTime()/this->getDN()) + 0.5*(op.getExpiredTime()/this->getDN())*op.getSigma()*op.getSigma()*(M-i)*(M-i));
                double b = 1/(1+op.getRisk()*(op.getExpiredTime()/this->getDN()))*(1-(op.getExpiredTime()/this->getDN())*op.getSigma()*op.getSigma()*(M-i)*(M-i));
                double c = 1/(1+op.getRisk()*(op.getExpiredTime()/this->getDN()))*((op.getExpiredTime()/this->getDN())*0.5*(op.getRisk()-op.getReturnRate())*(M-i) + 0.5*(op.getExpiredTime()/this->getDN())*op.getSigma()*op.getSigma()*(M-i)*(M-i));
                Option_result[i][j]= a*Option_result[i+1][j+1] + b*Option_result[i][j+1] + c*Option_result[i-1][j+1];
            }
        }
    }
    this->FDMatrix = Option_result;
    return Option_result[std::floor(M/2)][0];    
}