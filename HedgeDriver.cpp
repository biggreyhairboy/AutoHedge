//
// Created by patrick on 12/2/16.
//


#include "HedgeDriver.h"

HedgeDriver::HedgeDriver(string maturitydate, Option::Type optiontype, Real underlyingprice, Real strikeprice,
                         Spread dividend, Rate riskfreerate, Volatility vol)
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
    EE = boost::shared_ptr<Exercise>(new EuropeanExercise(maturity));
    Payoff = boost::shared_ptr<StrikedTypePayoff>(new PlainVanillaPayoff(Option::Type(type), strikeprice));
    europeanOption = new VanillaOption(Payoff, EE);
}
void HedgeDriver::SetupYieldDividendVolcurve()
{
    Handle<Quote> underlyingH(
            boost::shared_ptr<Quote>(new SimpleQuote(underlying)));
    Handle<YieldTermStructure> flatTermStructure(
            boost::shared_ptr<YieldTermStructure>(
                    new FlatForward(settlementDate, riskFreeRate, dayCounter)));
    Handle<YieldTermStructure> flatDividendTS(
            boost::shared_ptr<YieldTermStructure>(
                    new FlatForward(settlementDate, dividendYield, dayCounter)));
    Handle<BlackVolTermStructure> flatVolTS(
            boost::shared_ptr<BlackVolTermStructure>(
                    new BlackConstantVol(settlementDate, calendar, volatility,
                                         dayCounter)));
    boost::shared_ptr<StrikedTypePayoff> payoff(
            new PlainVanillaPayoff(type, strike));
    boost::shared_ptr<BlackScholesMertonProcess> bsmProcess(
            new BlackScholesMertonProcess(underlyingH, flatDividendTS,
                                          flatTermStructure, flatVolTS));
    europeanOption->setPricingEngine(boost::shared_ptr<PricingEngine>(
            new AnalyticEuropeanEngine(bsmProcess)));
}

double HedgeDriver::GetOptionValue() {
    SetupYieldDividendVolcurve();
    return europeanOption->NPV();
}


