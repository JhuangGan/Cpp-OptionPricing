#ifndef OptionPricing_h
#define OptionPricing_h
#include <vector>
#include <string>

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
    virtual double assetPricing(Engine & engine)=0; //纯虚函数做统一接口，输入计算引擎，实现获取资产定价
};

//以下的期权的标的例子为股票

// 定义类Stock
class Stock : public Asset
{
public:
    Stock() {};
    Stock(double nowPrice, double sigma);
    ~Stock() {};
    Stock(Stock & rightstock);
    void Reset(double nowPrice, double sigma);
    double getnowPrice(){return this->nowPrice;};
    double getSigma(){return this->Sigma;};
    double assetPricing(Engine & engine);
private:
    double nowPrice=0; //股票现价
    double Sigma;  //市场波动率
};

// 定义类Option //不含到期日和行权日，仅作为欧式和美式期权的母类使用
class Option: public Asset
{
public:
    Option() {};  //构造函数
    Option(Asset & asset, Market & market, double StrikePrice, std::string PutCall, double ExpiredTime);
    ~Option(){};
    Option(Option & rightop);  //拷贝构造函数
    double getnowPrice(){return this->nowPrice;};
    double getS0(){return this->S0;};
    double getRisk(){return this->Risk;};
    double getSigma(){return this->Sigma;};
    double getStrikePrice(){return this->StrikePrice;};
    std::string getPutCall(){return this->PutCall;};
    double getExpiredTime(){return this->ExpiredTime;};
    virtual std::string getoptiontype()=0;

    double assetPricing(Engine & engine) override;
private:
    double nowPrice;   //期权当前价格，assetPricing后赋值
    double S0;      //标的现价
    double Risk;    // 市场无风险利率
    double Sigma;  //标的波动率
    double StrikePrice;   // 期权执行价格
    std::string PutCall;  //put or call
    double ExpiredTime;  // 到期日
    
    // std::string OptionType="None"; //期权类型
};

// 欧式期权

class EurpeanOption: public Option
{
public:
    EurpeanOption() {};
    EurpeanOption(Asset & asset, Market & market, double StrikePrice,double ExpiredTime, std::string PutCall);
    ~EurpeanOption() {};
    std::string getoptiontype(){return this->OptionType;};
private:
    std::string OptionType = "EurpeanOption";
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
private:
    std::string OptionType = "AmericanOption";
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

// 欧式期权的BS
class BSmodel: public Engine
{
public:
    BSmodel(){};
    BSmodel(double index);
    ~BSmodel(){};
    double CalValue(Option & op) override;
    friend double cdfnorm(double x);
private:
    double index;
};


// 定义计算方法蒙特卡洛
class MonteCarlo: public Engine
{
public:
    MonteCarlo() {};
    MonteCarlo(int iter);
    ~MonteCarlo() {};
    double CalValue(Option & op) override;
    //输入引擎

    int itert=100;  //迭代次数
};

// // 定义计算方法二叉树
// class BinaryTree: public Engine
// {
// public: 
//     BinaryTree() {};
//     ~BinaryTree() {};
//     double CalValue(Option * op){};   //输入需要的参数，输出计算的值
// };

// BS model


#endif