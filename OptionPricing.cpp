#include "OptionPricing.hpp"
#include <cmath>
#include <random>
#include <chrono>
#include <vector>
#include <iostream>
//Asset

// Stock
Stock::Stock(double nowPrice, double sigma): Asset()
{
    this->Sigma = sigma;
    this->nowPrice = nowPrice;
}

Stock::Stock(Stock & rightstock):Asset()
{
    this->Sigma =  rightstock.Sigma;
    this->nowPrice = rightstock.nowPrice;
}

void Stock::Reset(double nowPrice, double sigma)
{
    this->Sigma = sigma;
    this->nowPrice = nowPrice;
}


double Stock::assetPricing(Engine & engine)
{
    std::cout<< "stock are not used for asset pricing, and return stock nowPrice"<< std::endl;
    return this->nowPrice;
}

// Option

Option::Option(Asset & asset, Market& market, double StrikePrice, std::string PutCall, double ExpiredTime):Asset()
{
    this->S0 = asset.getnowPrice();
    this->Risk = market.risk;
    this->Sigma = asset.getSigma();
    this->ExpiredTime = ExpiredTime;
    this->StrikePrice = StrikePrice;
    this->PutCall = PutCall;
}

Option::Option(Option & rightop) 
{
    this->nowPrice = rightop.getnowPrice();
    this->S0 = rightop.S0;
    this->Risk = rightop.Risk;
    this->Sigma = rightop.Sigma;
    this->ExpiredTime = rightop.ExpiredTime;
    this->StrikePrice = rightop.StrikePrice;
    this->PutCall = rightop.PutCall;
}

double Option::assetPricing(Engine & engine)
{
    return engine.CalValue(*this);
}


// European Option
EurpeanOption::EurpeanOption(Asset & asset, Market & market, double StrikePrice,double ExpiredTime, std::string PutCall):Option(asset,market,StrikePrice, PutCall,ExpiredTime)
{
    this->OptionType = "EurpeanOption";
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



// double AmericanOption::assetPricing(Engine & engine)
// {
//     return engine.CalValue(*this);
// }

// Engine 
// double Engine::CalValue(Option & op)
// {
//     return op.S0;
// }

// BSmodel
BSmodel::BSmodel(double index):Engine()
{
    this->index = index;
}

double cdfnorm(double x)
{
    return 0.5 * (1 + std::erf(x / std::sqrt(2)));
}

double BSmodel::CalValue(Option & op)
{
    double output = 0.0;
    if (op.getPutCall() == "call")
    {
        double d1 = (std::log(op.getS0()/op.getStrikePrice())+((op.getRisk())+op.getSigma()*op.getSigma()/2)*op.getExpiredTime())/(op.getSigma()*std::sqrt(op.getExpiredTime()));
        double d2 = d1 - op.getSigma()*std::sqrt(op.getExpiredTime());
        output = op.getS0()*cdfnorm(d1)-op.getStrikePrice()*std::exp(-(op.getRisk())*op.getExpiredTime())*cdfnorm(d2);
    }
    else if (op.getPutCall() == "put") 
    {
        double d1 = (std::log(op.getS0()/op.getStrikePrice())+((op.getRisk())+op.getSigma()*op.getSigma()/2)*op.getExpiredTime())/(op.getSigma()*std::sqrt(op.getExpiredTime()));
        double d2 = d1 - op.getSigma()*std::sqrt(op.getExpiredTime());
        output =  op.getStrikePrice()*std::exp(-(op.getRisk())*op.getExpiredTime())*cdfnorm(-d2)-op.getS0()*cdfnorm(-d1);
    }
    return output;
}




// Binary Tree




// mat BinaryTree::bitree(double S0, double K, double r, double q, double T, int N, double sigma) 







// MonteCarlo

MonteCarlo::MonteCarlo(int iter)
{
    this->itert = iter;
}

double MonteCarlo:: CalValue(Option & op)
{
    if (op.getoptiontype() != "AmericanOption")
    {
        BSmodel bs(0.1);
        std::cout<<"it's not a AmericanOption"<<std::endl;
        std::cout<<"it's a "<<op.getoptiontype()<<"by BS model, price is"<<std::endl;
        return op.assetPricing(bs);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    //也可以设置一个种子
    std::default_random_engine gen(seed);
    
    double sumOp=0.0;
    if (op.getPutCall() == "call")
    {
        for(int i(0); i<this->itert; ++i)
        {
            std::normal_distribution<double> dis(0,1);
            sumOp += exp(-op.getRisk()*op.getExpiredTime())*std::max(op.getS0()*exp((op.getRisk()-op.getSigma()*op.getSigma()/2)*op.getExpiredTime() + op.getSigma()*dis(gen)*sqrt(op.getExpiredTime()))-op.getStrikePrice(),0.0);
        }
    }
    else if (op.getPutCall() == "put") 
    {
        for(int i(0); i<itert; ++i)
        {
            std::normal_distribution<double> dis(0,1);
            sumOp += exp(-op.getRisk()*op.getExpiredTime())*std::max(op.getStrikePrice()-op.getS0()*exp((op.getRisk()-op.getSigma()*op.getSigma()/2)*op.getExpiredTime() + op.getSigma()*dis(gen)*sqrt(op.getExpiredTime())),0.0);
        }
    }
    return sumOp/itert;
}


