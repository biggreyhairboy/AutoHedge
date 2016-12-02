//
// Created by patrick on 12/2/16.
//

#ifndef AUTOHEDGE_HEDGEDRIVER_H
#define AUTOHEDGE_HEDGEDRIVER_H

#include <string>
#include <boost/timer.hpp>
#include <iomanip>
#include <iostream>
#include <ql/quantlib.hpp>
using namespace std;
using namespace QuantLib;

class HedgeDriver {
public:
    VanillaOption europeanOption;
    HedgeDriver(string maturity, Option::Type type, Real underlying, Real strike, Spread dividendYield, Rate riskfreerate, Volatility vol );


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

    boost::shared_ptr<Exercise> europeanExercise;
    boost::shared_ptr<StrikedTypePayoff> payoff;

};


#endif //AUTOHEDGE_HEDGEDRIVER_H
