#include <vector>
#include "stat.h"

enum {GREEN, YELLOW, RED};

namespace SPRT {

struct Result {
    Probability p;
    double passRate, yellowRate, stopAvg, yellowAvg;
    std::vector<size_t> quantileValue;
};

Result average(size_t simulations, double bayesElo, double drawElo, double bayesElo0,
    double bayesElo1, const std::vector<double>& quantiles);

} // namespace SPRT
