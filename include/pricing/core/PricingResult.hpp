#ifndef PRICING_CORE_PRICING_RESULT_HPP
#define PRICING_CORE_PRICING_RESULT_HPP

namespace pricing {
namespace core {

struct PricingResult {
    double price = 0.0;

    // Greeks (will be populated later)
    double delta = 0.0;
    double gamma = 0.0;
    double vega = 0.0;
    double theta = 0.0;
    double rho = 0.0;

    bool hasGreeks() const {
        return delta != 0.0 || gamma != 0.0 || vega != 0.0 || 
               theta != 0.0 || rho != 0.0;
    }
};

} // namespace core
} // namespace pricing

#endif // PRICING_CORE_PRICING_RESULT_HPP
