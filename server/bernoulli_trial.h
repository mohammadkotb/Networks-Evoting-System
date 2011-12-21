#ifndef BERNOULLI_TRIAL_H_
#define BERNOULLI_TRIAL_H_

#include <time.h>
#include <stdlib.h>
class BernoulliTrial{
    public:
        BernoulliTrial(double success_probability);
        ~BernoulliTrial();
        bool shouldDoIt();
    private:
        double success_probability;
};

#endif
