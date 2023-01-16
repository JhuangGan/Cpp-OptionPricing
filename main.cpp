#include<iostream>
#include "OptionPricing.hpp"

#include <iterator>
#include "Matrix.hpp"  //用于print矩阵

// example 部分
int main()
{
    Market market(0.1);
    Stock stock(50, 0.02, 0.4);

    //欧式期权的BS示例
    BSmodel bs;
    EuropeanOption Eop(stock, market, 50, 0.4167,"call");
    // 1 期权调用定价
    std::cout << "Eop:" << Eop.assetPricing(bs) << std::endl;
    // 2 用期权实例化引擎
    BSmodel bs2(Eop);
    std::cout << "Eop:" << bs2.CalValue() << std::endl;;
    // 查看bs引擎所关联的期权属性
    bs.getOptionAttribute();
    bs2.getOptionAttribute();
    
    // Monte Carlo 示例
    // 两种方式输出MonteCarlo的其他属性,第一种是,先实例化引擎,在assectPricing之后,自动将该期权实例化该mc引擎
    MonteCarlo mc = MonteCarlo(100);
    AmericanOption Aop(stock, market, 50 ,0.4167,"call");
    std::cout << "Aop1 by Monte Carlo:" << Aop.assetPricing(mc)<<std::endl;
    // 然后调用引擎输出对应的属性
    // 这里使用流迭代器输出vector
    std::vector<double> v=mc.getOptionList();
    std::cout << "OptionList: [";
    std::copy(std::begin(v), std::end(v), std::ostream_iterator<double>(std::cout, ","));
    std::cout << "]" << std::endl;

    // 第二种是主动用期权实例化引擎,然后通过引擎来输出
    MonteCarlo mc2(Aop, 10);  //10可不输入,默认100步迭代
    std::cout<< "Aop2 by Monte Carlo:" << mc2.CalValue() << std::endl;;  //先进行计算期权值,然后能够得到相应的list
    // 然后调用引擎输出对应的属性
    // 这里使用流迭代器输出vector
    std::vector<double> v2= mc2.getOptionList();
    std::cout << "OptionList: [";
    std::copy(std::begin(v2), std::end(v2), std::ostream_iterator<double>(std::cout, ","));
    std::cout << "]" << std::endl;
    
    // 查看所实例化的期权属性
    mc.getOptionAttribute();
    mc2.getOptionAttribute();

    // 这里需要说明的是，如果将已经实例化的引擎用于其他期权进行计算，其对应的引擎属性也将更新为最新使用的期权对应的属性。

    // Binary Tree example
    // 1 先实例化引擎，然后期权调用引擎计算定价
    BinaryTree bt(10);
    AmericanOption AopBT(stock, market, 50 ,0.4167,"put");
    std::cout<< "Aop by Binary Tree:"  << AopBT.assetPricing(bt) << std::endl;

    // 输出BT矩阵
    std::vector<std::vector<double>> v3= bt.getBT();
    
    std::cout << "BT: ";
    for (auto outer_it = v3.begin(); outer_it != v3.end(); ++outer_it) 
    {
        for (auto inner_it = outer_it->begin(); inner_it != outer_it->end(); ++inner_it) 
        {
            std::cout << *inner_it << ' ';
        }
        std::cout << '\n';
    }

    // Matrix y(v3);  // 也可以用Matrix里重载的输出符号print
    // std::cout << "BT: "<<y;

    // 2 用期权实例化引擎，然后引擎计算定价
    AmericanOption AopBT2(stock, market, 50 ,0.4167,"put");
    BinaryTree bt2(AopBT2,10);
    bt2.CalValue();
    // BT矩阵输入与上面一致
    
    // FD有限差分部分
    // 显式差分部分,隐式差分类似,将"Explicit"改为"Implicit"
    // 1 实例化引擎和期权,然后期权调用引擎
    AmericanOption AopFD(stock, market, 50 ,0.4167,"put");
    FD fdex("Explicit", 2, 20, 5.0);
    std::cout << "FD Explicit: " << AopFD.assetPricing(fdex) << std::endl;

    // 输出FDmatrix
    std::vector<std::vector<double>> v4= fdex.getFDMatrix();
    std::cout << "FDmatrix: ";
    for (auto outer_it = v4.begin(); outer_it != v4.end(); ++outer_it) 
    {
        for (auto inner_it = outer_it->begin(); inner_it != outer_it->end(); ++inner_it) 
        {
            std::cout << *inner_it << ' ';
        }
        std::cout << '\n';
    }

    // Matrix X(v4);  // 也可以用Matrix里重载的输出符号print
    // std::cout << "FDmatrix: "<<X;


    // 2 用期权实例化引擎,然后引擎调用计算函数
    FD fdex2(AopFD,"Explicit", 2, 20, 5.0);
    std::cout << "FD ex2: " << fdex2.CalValue() << std::endl;
    // 输出FDmatrix同上,省略

    // 隐式有限差分
    AmericanOption AopFDIm(stock, market, 50 ,0.4167,"put");
    FD fdim("Implicit", 2, 10, 5.0);
    std::cout << "FD Implicit: " << AopFDIm.assetPricing(fdim) << std::endl;
    
    // 将差分矩阵用Matrix的重载print
    // auto m = fdim.getFDMatrix();
    // Matrix M(m);
    // std::cout<<M;

    return 0;
}