#include "bernoulli_trial.h"
BernoulliTrial::BernoulliTrial(double success_probability){
    this->success_probability = success_probability;
    /* initialize random seed: */
    srand (time(NULL));
}

bool BernoulliTrial::shouldDoIt(){
    //generate a random number
    if (rand() % 100 < 100 * success_probability)
        return true;
    return false;
}

BernoulliTrial::~BernoulliTrial(){}
