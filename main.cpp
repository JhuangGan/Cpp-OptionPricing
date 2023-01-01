#include<iostream>
#include "OptionPricing.hpp"
// #include "AmericanOption.cpp"
// test 部分
int main()
{
    Market market(0.1);
    Stock stock(50, 0.4);
    MonteCarlo mc = MonteCarlo(100);
    EurpeanOption Eop(stock, market, 50,0.417,"call");
    AmericanOption Aop(stock, market, 50,0.417,"call");
    
    std::cout << Eop.assetPricing(mc)<<std::endl;

    std::cout << Aop.assetPricing(mc)<<std::endl;

    // std::cout << Aop.getExpiredTime()<<std::endl;
    // std::cout << Aop.getS0()<<std::endl;
    // std::cout << Aop.getRisk()<<std::endl;
    // std::cout << Aop.getSigma()<<std::endl;
    // std::cout << Aop.getStrikePrice()<<std::endl;
    // std::cout << Aop.getPutCall()<<std::endl;
    return 0;
}