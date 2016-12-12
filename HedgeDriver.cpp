//
// Created by patrick on 12/2/16.
//


#include "HedgeDriver.h"

HedgeDriver::HedgeDriver(string maturitydate, Option::Type optiontype, Real underlyingprice, Real strikeprice,
                         Spread dividend, Rate riskfreerate, Volatility vol)
{
    // set up dates
    calendar = TARGET();
    todaysDate = Date(10, December, 2016);
    settlementDate = Date(2, December, 2016);
    Settings::instance().evaluationDate() = todaysDate;

    // our options
    type = Option::Type(optiontype);
    underlying = underlyingprice;
    strike = strikeprice;
    dividendYield = dividend;
    riskFreeRate = riskfreerate;
    volatility = vol;
    maturity = Date(15, March, 2017);
    dayCounter = Actual365Fixed();
    EE = boost::shared_ptr<Exercise>(new EuropeanExercise(maturity));
    Payoff = boost::shared_ptr<StrikedTypePayoff>(new PlainVanillaPayoff(Option::Type(type), strikeprice));
    europeanOption = new VanillaOption(Payoff, EE);
}
void HedgeDriver::SetupYieldDividendVolcurve(double underlyingprice)
{
    Handle<Quote> underlyingH(
            boost::shared_ptr<Quote>(new SimpleQuote(underlyingprice)));
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
//    //underlyingH.currentLink()
//    underlying = 27000;
//    Handle<Quote> anotherunderlyingH(boost::shared_ptr<Quote>(new SimpleQuote(underlying)));
////    underlyingH = anotherunderlyingH;
////    bsmProcess(
////            new BlackScholesMertonProcess(underlyingH, flatDividendTS,
////                                          flatTermStructure, flatVolTS));


}

double HedgeDriver::GetOptionValue() {
    SetupYieldDividendVolcurve(underlying);
    //europeanOption->
    cout << europeanOption->NPV() << endl;
    cout << europeanOption->delta() << endl;
    cout << europeanOption->gamma() << endl;
    cout << europeanOption->theta() << endl;
    cout << europeanOption->vega() << endl;
    cout << europeanOption->rho() << endl;

    cout << "another process" << endl;
    SetupYieldDividendVolcurve(27000);
    //europeanOption->
    cout << europeanOption->NPV() << endl;
    cout << europeanOption->delta() << endl;
    cout << europeanOption->gamma() << endl;
    cout << europeanOption->theta() << endl;
    cout << europeanOption->vega() << endl;
    cout << europeanOption->rho() << endl;



    return europeanOption->NPV();
}


