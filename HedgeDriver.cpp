//
// Created by patrick on 12/2/16.
//

#include "HedgeDriver.h"
HedgeDriver::HedgeDriver(string maturitydate, Option::Type optiontype, Real underlyingprice, Real strikeprice,
                         Spread dividend, Rate riskfreerate, Volatility vol): europeanExercise(new EuropeanExercise(maturity)),
                                                                              payoff(new PlainVanillaPayoff(type, strike)),
                                                                              europeanOption(payoff, europeanExercise)
{
    // set up dates
    calendar = TARGET();
    todaysDate = Date(2, December, 2016);
    settlementDate = Date(2, December, 2016);
    Settings::instance().evaluationDate() = todaysDate;

    // our options
    type = Option::Type(optiontype);
    underlying = underlyingprice;
    strike = strikeprice;
    dividendYield = dividend;
    riskFreeRate = riskfreerate;
    volatility = vol;
    maturity = Date(15, December, 2016);
    dayCounter = Actual365Fixed();

//    europeanExercise = boost::shared_ptr<Exercise>(new EuropeanExercise(maturity));
//    payoff = boost::shared_ptr<StrikedTypePayoff>(new PlainVanillaPayoff(type, strike));
//    europeanOption = VanillaOption(payoff, europeanExercise);

}
