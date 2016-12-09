//
// Created by patrick on 12/8/16.
//

#ifndef AUTOHEDGE_OPTIONCALCULATOR_H
#define AUTOHEDGE_OPTIONCALCULATOR_H


class OptionCalculator {
public:
    OptionCalculator();
    double CalculateDelta(double lastprice);
private:
    double OptionValue;

};


#endif //AUTOHEDGE_OPTIONCALCULATOR_H
