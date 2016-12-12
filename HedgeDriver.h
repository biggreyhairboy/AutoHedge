//
// Created by patrick on 12/2/16.
//

#ifndef AUTOHEDGE_HEDGEDRIVER_H
#define AUTOHEDGE_HEDGEDRIVER_H

#include <string>
#include <iomanip>
#include <iostream>
#include <ql/quantlib.hpp>
using namespace QuantLib;
using namespace std;


class HedgeDriver {
public:
    HedgeDriver(string maturity, Option::Type type, Real underlying, Real strike, Spread dividendYield,
                    Rate riskfreerate, Volatility vol);
    void SetupYieldDividendVolcurve(double underlyingprice);
    double GetOptionValue();
private:
    // set up dates
    Calendar calendar;
    Date todaysDate;
    Date settlementDate;

    // our options
    Option::Type type;
    Real underlying;
    Real strike;
    Spread dividendYield;
    Rate riskFreeRate;
    Volatility volatility;
    Date maturity;

    DayCounter dayCounter;
    boost::shared_ptr<Exercise> EE;
    boost::shared_ptr<StrikedTypePayoff> Payoff;
    VanillaOption* europeanOption;
};

#endif //AUTOHEDGE_HEDGEDRIVER_H
