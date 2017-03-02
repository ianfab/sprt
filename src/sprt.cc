#include <algorithm>
#include "sprt.h"

namespace {

int one(const double llrInc[3], PRNG& prng, size_t& stop)
{
    // LLR (Log Likelyhood Ratio)
    const double bound = std::log((1 - 0.05) / 0.05);
    double LLR = 0;
    int diff = 0;

    // Run an SPRT test (loop look infinite but terminates eventually with probability 1)
    for (stop = 0; std::abs(LLR) < bound; ++stop)
    {
        int result = prng.game_result();
        LLR += llrInc[result];
        diff += result == WIN ? 1 : result == LOSS ? -1 : 0;
    }

    return LLR >= bound ? GREEN : diff > 0 ? YELLOW : RED;
}

} // namespace

namespace SPRT {

Result average(size_t simulations, double bayesElo, double drawElo, double bayesElo0,
    double bayesElo1, const std::vector<double>& quantiles)
{
    // Calculate probability laws under H0 and H1
    Probability p0, p1;
    p0.set(bayesElo0, drawElo);
    p1.set(bayesElo1, drawElo);

    // Pre-calculate LLR increment for each game result
    const double llrInc[3] = {
        std::log(p1.loss / p0.loss),
        std::log(p1.draw() / p0.draw()),
        std::log(p1.win / p0.win)
    };

    // Prepare result
    Result r;
    r.p.set(bayesElo, drawElo);
    PRNG prng(r.p);

    // Run simulations
    size_t passCount = 0, yellowCount = 0, stopSum = 0, yellowSum = 0;
    std::vector<size_t> stop(simulations);
    for (size_t s = 0; s < simulations; ++s) {
        int result = one(llrInc, prng, stop[s]);
        if (result == GREEN)
            passCount++;
        else if (result == YELLOW)
        {
            yellowCount++;
            yellowSum += stop[s];
        }
        stopSum += stop[s];
    }

    // Collect quantiles
    if (!quantiles.empty()) {
        std::sort(stop.begin(), stop.end());
        for (auto& qp : quantiles)
            r.quantileValue.push_back(stop[(simulations - 1) * qp + 0.5]);
    }

    r.passRate   = double(passCount) / simulations;
    r.yellowRate = double(yellowCount) / simulations;
    r.stopAvg    = double(stopSum) / simulations;
    r.yellowAvg  = yellowCount ? double(yellowSum) / yellowCount : 0;
    return r;
}

} // namespace SPRT
