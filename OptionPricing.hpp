#ifndef OptionPricing_h
#define OptionPricing_h
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <chrono>

#include <Eigen/LU>
//用于隐式有限差分求解矩阵，使用逆矩阵求解矩阵方程

// 先声明
class Asset;
class Engine;
class MonteCarlo;

// 加一个市场类 Market,包含无风险利率一个属性
class Market
{
public:
    double risk;  //market的所有属性均public
    Market(){};
    ~Market(){};
    Market(double risk){this->risk=risk;};
};

//第一部分资产类定义

// 定义基础虚类Asset
class Asset
{
public:
	Asset() {};
	~Asset() {};
    virtual double getnowPrice()=0;  //得到市场上的现价
    virtual double getSigma()=0;  //sigma
    virtual double getReturnRate()=0;  //资产收益率
    virtual double assetPricing(Engine & engine)=0; //纯虚函数做统一接口，输入计算引擎，实现获取资产定价
};

//以下的期权的标的例子为股票
// 标的资产类
// 定义类Stock
class Stock : public Asset
{
public:
    Stock() {};
    Stock(double nowPrice, double returnRate, double sigma);
    ~Stock() {};
    Stock(Stock & rightstock);
    void Reset(double nowPrice, double returnRate, double sigma);
    double getnowPrice(){return this->nowPrice;};
    double getSigma(){return this->Sigma;};
    double getReturnRate(){return this->ReturnRate;};
    
private:
    double nowPrice=0; //股票现价
    double Sigma;  //市场波动率
    double ReturnRate;  //股息

    double assetPricing(Engine & engine){throw -1;  return -1;};  //但使用空的,不可避免的会有点问题
    // 股票等标的资产不使用资产定价所以不实现该接口
    // 采用私有的,空的方法,最终不实现该接口
};

// 非标的资产,期权基类
// 定义类Option，仅作为欧式和美式期权的母类使用, 新建立一个虚接口
class Option: public Asset
{
public:
    Option() {};  //构造函数
    Option(Asset & asset, Market & market, double StrikePrice, std::string PutCall, double ExpiredTime);
    ~Option(){};
    Option(Option & rightop);  //拷贝构造函数
    
    double getS0(){return this->S0;};
    double getRisk(){return this->Risk;};
    double getSigma(){return this->Sigma;};
    double getStrikePrice(){return this->StrikePrice;};
    std::string getPutCall(){return this->PutCall;};
    double getExpiredTime(){return this->ExpiredTime;};
    
    virtual std::string getoptiontype()=0;  //定义新的接口,期权类型(欧式或美式)
    double getnowPrice(){throw -1; return -1;};  //因为期权靠定价,所以也不实现该接口
    double getReturnRate(){return this->ReturnRate;};
    double assetPricing(Engine & engine){throw -1; return -1;}; 

    void whatItis();  //用于输出这个期权的所有属性
private:
    // double nowPrice;   //删去,期权当前价格，assetPricing后赋值
    double S0;      //标的现价
    double ReturnRate;  //注意这里是标的的收益率
    double Risk;    // 市场无风险利率
    double Sigma;  //标的波动率
    double StrikePrice;   // 期权执行价格
    std::string PutCall;  //put or call
    double ExpiredTime;  // 到期日
    // std::string OptionType="None"; //期权类型

    // 这里期权基类暂不实现,因为欧式只用BS定价,而美式有三种方法定价
    // 采用私有的,空的方法,最终不实现该接口,但似乎这样就不能访问,所以还是放在public
};

// 欧式期权

class EuropeanOption: public Option
{
public:
    EuropeanOption() {};
    EuropeanOption(Asset & asset, Market & market, double StrikePrice,double ExpiredTime, std::string PutCall);
    EuropeanOption(EuropeanOption & europeanOption);
    ~EuropeanOption() {};
    std::string getoptiontype(){return this->OptionType;};
    double assetPricing(Engine & engine) override;
private:
    std::string OptionType = "EuropeanOption";
    double getnowPrice(){throw -1;  return -1;};  //因为期权靠定价,所以也不实现该接口
    
};

// 定义美式期权,行权日期（与欧式不同的是行权日期<=到期时间）
class AmericanOption: public Option
{
public:
    AmericanOption() {};  //构造函数
    AmericanOption(Asset & asset, Market & market, double StrikePrice,double ExpiredTime, std::string PutCall);
    ~AmericanOption(){};
    AmericanOption(AmericanOption & righAtop);  //拷贝构造函数
    std::string getoptiontype(){return this->OptionType;};
    double assetPricing(Engine & engine) override;
private:
    std::string OptionType = "AmericanOption";
    double getnowPrice(){throw -1;  return -1;};  //因为期权靠定价,所以也不实现该接口
};

// 第二部分计算方法类定义，将计算方法与资产类分开，可拓展性和维护性大大提高

// 定义计算方法Engine基类，纯类，不是虚类，需要在Option中作为参数输入
class Engine
{
public:
    Engine() {};
    ~Engine() {};
    virtual double CalValue(Option & op)=0; //该引擎的定价计算
};

// 这里分出来两种期权,适用于欧式期权的引擎和美式期权的引擎,如果后续要定义两者都适用的引擎的话,
// 要么?算了,一般来讲也没人定义欧式期权来弄美式期权的计算方法吧

// 欧式期权的BS
class BSmodel: public Engine
{
public:
    BSmodel(){};
    BSmodel(Option & op);  //用期权实例化的引擎
    ~BSmodel(){};
    double CalValue(Option & op) override;
    double CalValue(){return this->CalValue(*(this->op));}
    void getOptionAttribute(){return (this->op)->whatItis();}; //返回所用实例化的期权属性展示
    friend double cdfnorm(double x);
private:
    Option * op;  // 将期权指针作为引擎属性, 实现引擎通过期权的实例化
};

// 定义计算方法蒙特卡洛
class MonteCarlo: public Engine 
{
public:
    MonteCarlo() {};
    MonteCarlo(int iter/*= 100*/);
    MonteCarlo(Option & op, int iter /*= 100*/);  //迭代次数可以自设,也可以默认100
    //用期权实例化的引擎,用于查看蒙特卡洛的其他属性
    double CalValue(){return this->CalValue(*(this->op));};  //与之对应的计算方法
    ~MonteCarlo() {};
    double CalValue(Option & op) override;  //计算后也会重置实例化的期权
    void getOptionAttribute(){return this->op->whatItis();}; //返回所用实例化的期权属性展示
    Option * getOptionPoint(){return this->op;};  //返回所用实例化的期权指针,现在没用之后可能有用
    
    double getIter(){return this->itert;};

    // 获取蒙特卡洛的东西的函数,比如模拟的股票数组和期权数组
    std::vector<double> getStockList(){return StockList;};
    std::vector<double> getOptionList(){return OptionList;};

private:
    int itert=100;  //迭代次数
    Option * op;  // 将期权指针作为引擎属性, 实现引擎通过期权的实例化,同时在计算时和构造函数时都进行初始化期权指针
    // 不得不感叹指针太强大了,能将指向虚类, 因为指针是实的,从而使得可以虚类可以实例化
    // 想起来之前看到个例子说让把不同类型的值放在一个vector里,但因为vector不能存放不同类型. 而最神奇的地方来了, 创建一个指向指针的指针,
    // 然后内嵌的指针指向不同类型的值,解两次引用,就能实现将不同类型的值存放在一个vector里
    std::vector<double> StockList;
    std::vector<double> OptionList;
};


// 定义计算方法二叉树
class BinaryTree: public Engine
{
public: 
    BinaryTree() {};
    BinaryTree(int iter /*=10*/){this->itert=iter;};
    BinaryTree(Option & op, int iter /*= 100*/);  //期权实例化二叉树引擎
    double CalValue(){return this->CalValue(*(this->op));};  //与之对应的计算方法
    void getOptionAttribute(){return this->op->whatItis();}; //返回所用实例化的期权属性展示
    Option * getOptionPoint(){return this->op;};  //返回所用实例化的期权指针,现在没用之后可能有用
    
    ~BinaryTree() {};
    double CalValue(Option & op);
    int getIter(){return this->itert;};

    std::vector<std::vector<double>> getBT(){return this->BT;};
private:
    int itert=100;
    // 这里使用vector实现简单的matrix
    std::vector<std::vector<double>> BT;
    Option * op;  // 将期权指针作为引擎属性, 实现引擎通过期权的实例化
};


// 有限差分计算引擎(显式和隐式放在一起)
class FD: public Engine
{
public:
    FD(){};
    ~FD(){};
    FD(std::string FDtype, int enlargefactor/*=2*/, int DN /*=20*/, double delta_s/*=0.1*/);
    FD(Option & op, std::string FDtype, int enlargefactor/*=2*/, int DN /*=20*/, double delta_s/*=0.1*/);
    double CalValue(){return this->CalValue(*(this->op));};  //与之对应的计算方法
    void getOptionAttribute(){return this->op->whatItis();}; //返回所用实例化的期权属性展示
    Option * getOptionPoint(){return this->op;};  //返回所用实例化的期权指针,现在没用之后可能有用
    
    std::vector<std::vector<double>> getFDMatrix(){return this->FDMatrix;};
    double CalValue(Option & op);

    int getenlargefactor(){return this->enlargefactor;};
    double getDN(){return this->DN;};
    double getdelta_s(){return this->delta_s;};
    std::string getFDtype(){return this->FDtype;};

private:
    int enlargefactor;
    int DN;  // 时间差分步数
    double delta_s;  //价格差分步长
    std::vector<std::vector<double>> FDMatrix;  //存储差分矩阵
    Option * op;   
    std::string FDtype;  //隐式差分或显式差分
};

#endif