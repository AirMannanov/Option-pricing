#ifndef PRICING_MODELS_PRICING_MODEL_HPP
#define PRICING_MODELS_PRICING_MODEL_HPP

#include "../core/Option.hpp"
#include "../core/MarketData.hpp"
#include "../core/PricingResult.hpp"

namespace pricing {
namespace models {

class PricingModel {
public:
    virtual ~PricingModel() = default;
    
    virtual core::PricingResult price(
        const core::Option& option,
        const core::MarketData& marketData) const = 0;
};

} // namespace models
} // namespace pricing

#endif // PRICING_MODELS_PRICING_MODEL_HPP
