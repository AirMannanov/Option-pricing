#ifndef PRICING_CORE_OPTION_HPP
#define PRICING_CORE_OPTION_HPP

#include <stdexcept>

namespace pricing {
namespace core {

enum class OptionType {
    Call,
    Put
};

class Option {
public:
    Option(OptionType type, double strike, double timeToExpiration)
        : type_(type), strike_(strike), timeToExpiration_(timeToExpiration) {
        validate();
    }

    OptionType getType() const { return type_; }
    double getStrike() const { return strike_; }
    double getTimeToExpiration() const { return timeToExpiration_; }

    bool isCall() const { return type_ == OptionType::Call; }
    bool isPut() const { return type_ == OptionType::Put; }

private:
    void validate() const {
        if (strike_ <= 0.0) {
            throw std::invalid_argument("Strike price must be positive");
        }
        if (timeToExpiration_ < 0.0) {
            throw std::invalid_argument("Time to expiration cannot be negative");
        }
    }

    OptionType type_;
    double strike_;
    double timeToExpiration_;
};

} // namespace core
} // namespace pricing

#endif // PRICING_CORE_OPTION_HPP

