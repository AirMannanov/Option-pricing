#ifndef PRICING_CORE_MARKET_DATA_HPP
#define PRICING_CORE_MARKET_DATA_HPP

#include <stdexcept>

namespace pricing {
namespace core {

class MarketData {
public:
    MarketData(double spot, double riskFreeRate, double volatility)
        : spot_(spot), riskFreeRate_(riskFreeRate), volatility_(volatility) {
        validate();
    }

    double getSpot() const { return spot_; }
    double getRiskFreeRate() const { return riskFreeRate_; }
    double getVolatility() const { return volatility_; }

private:
    void validate() const {
        if (spot_ <= 0.0) {
            throw std::invalid_argument("Spot price must be positive");
        }
        if (volatility_ < 0.0) {
            throw std::invalid_argument("Volatility cannot be negative");
        }
        // Risk-free rate can be negative in some market conditions,
        // but we'll allow it for now
    }

    double spot_;
    double riskFreeRate_;
    double volatility_;
};

} // namespace core
} // namespace pricing

#endif // PRICING_CORE_MARKET_DATA_HPP
