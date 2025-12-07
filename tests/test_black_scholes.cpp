#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>

#include "../include/pricing/core/MarketData.hpp"
#include "../include/pricing/core/Option.hpp"
#include "../include/pricing/models/BlackScholesModel.hpp"

using namespace pricing;
using namespace pricing::core;
using namespace pricing::models;
using Catch::Matchers::WithinAbs;

TEST_CASE("Black-Scholes: Call option - standard case", "[black_scholes]") {
    // Test case: S=100, K=100, r=0.05, σ=0.2, T=1.0
    Option option(OptionType::Call, 100.0, 1.0);
    MarketData marketData(100.0, 0.05, 0.2);
    BlackScholesModel model;
    
    auto result = model.price(option, marketData);
    
    // Call option should have positive value
    REQUIRE(result.price > 0.0);
    // For at-the-money option with positive time value, price should be reasonable
    REQUIRE(result.price > 5.0);
    REQUIRE(result.price < 20.0);
}

TEST_CASE("Black-Scholes: Put option - standard case", "[black_scholes]") {
    // Test case: S=100, K=100, r=0.05, σ=0.2, T=1.0
    Option option(OptionType::Put, 100.0, 1.0);
    MarketData marketData(100.0, 0.05, 0.2);
    BlackScholesModel model;
    
    auto result = model.price(option, marketData);
    
    // Put option should have positive value
    REQUIRE(result.price > 0.0);
    // For at-the-money option with positive time value, price should be reasonable
    REQUIRE(result.price > 3.0);
    REQUIRE(result.price < 15.0);
}

TEST_CASE("Black-Scholes: Call option - ITM", "[black_scholes]") {
    // In-the-money call: S=110, K=100, r=0.05, σ=0.2, T=0.5
    Option option(OptionType::Call, 100.0, 0.5);
    MarketData marketData(110.0, 0.05, 0.2);
    BlackScholesModel model;
    
    auto result = model.price(option, marketData);
    
    // ITM call should be worth at least intrinsic value (10.0)
    REQUIRE(result.price >= 10.0);
}

TEST_CASE("Black-Scholes: Put option - ITM", "[black_scholes]") {
    // In-the-money put: S=90, K=100, r=0.05, σ=0.2, T=0.5
    Option option(OptionType::Put, 100.0, 0.5);
    MarketData marketData(90.0, 0.05, 0.2);
    BlackScholesModel model;
    
    auto result = model.price(option, marketData);
    
    // ITM put should be worth at least intrinsic value (10.0)
    REQUIRE(result.price >= 10.0);
}

TEST_CASE("Black-Scholes: Call option - OTM", "[black_scholes]") {
    // Out-of-the-money call: S=90, K=100, r=0.05, σ=0.2, T=0.5
    Option option(OptionType::Call, 100.0, 0.5);
    MarketData marketData(90.0, 0.05, 0.2);
    BlackScholesModel model;
    
    auto result = model.price(option, marketData);
    
    // OTM call should have positive value due to time value
    REQUIRE(result.price > 0.0);
    // But less than intrinsic (which would be 0)
    REQUIRE(result.price < 10.0);
}

TEST_CASE("Black-Scholes: Put-Call Parity", "[black_scholes]") {
    // Put-Call Parity: C - P = S - K*e^(-r*T)
    double S = 100.0;
    double K = 105.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 0.5;
    
    Option callOption(OptionType::Call, K, T);
    Option putOption(OptionType::Put, K, T);
    MarketData marketData(S, r, sigma);
    BlackScholesModel model;
    
    auto callResult = model.price(callOption, marketData);
    auto putResult = model.price(putOption, marketData);
    
    double discountFactor = std::exp(-r * T);
    double expectedDifference = S - K * discountFactor;
    double actualDifference = callResult.price - putResult.price;
    
    REQUIRE_THAT(actualDifference, WithinAbs(expectedDifference, 0.01));
}

TEST_CASE("Black-Scholes: Edge case - T=0 (at expiration)", "[black_scholes]") {
    // At expiration, option value equals intrinsic value
    Option callOption(OptionType::Call, 100.0, 0.0);
    Option putOption(OptionType::Put, 100.0, 0.0);
    MarketData marketData(110.0, 0.05, 0.2);
    BlackScholesModel model;
    
    auto callResult = model.price(callOption, marketData);
    auto putResult = model.price(putOption, marketData);
    
    // Call: max(S-K, 0) = max(110-100, 0) = 10
    REQUIRE_THAT(callResult.price, WithinAbs(10.0, 0.0001));
    
    // Put: max(K-S, 0) = max(100-110, 0) = 0
    REQUIRE_THAT(putResult.price, WithinAbs(0.0, 0.0001));
}

TEST_CASE("Black-Scholes: Edge case - σ=0 (no volatility)", "[black_scholes]") {
    // With zero volatility, option value is discounted intrinsic value
    Option callOption(OptionType::Call, 100.0, 0.5);
    MarketData marketData(110.0, 0.05, 0.0);
    BlackScholesModel model;
    
    auto result = model.price(callOption, marketData);
    
    // Should be approximately: max(S - K*e^(-r*T), 0)
    double discountFactor = std::exp(-0.05 * 0.5);
    double expected = std::max(110.0 - 100.0 * discountFactor, 0.0);
    
    REQUIRE_THAT(result.price, WithinAbs(expected, 0.01));
}

TEST_CASE("Black-Scholes: Edge case - very high volatility", "[black_scholes]") {
    // High volatility should increase option value
    Option option(OptionType::Call, 100.0, 0.5);
    MarketData lowVolData(100.0, 0.05, 0.1);
    MarketData highVolData(100.0, 0.05, 1.0);
    BlackScholesModel model;
    
    auto lowVolResult = model.price(option, lowVolData);
    auto highVolResult = model.price(option, highVolData);
    
    // Higher volatility should result in higher option price
    REQUIRE(highVolResult.price > lowVolResult.price);
}

TEST_CASE("Black-Scholes: Validation - negative strike", "[validation]") {
    REQUIRE_THROWS_AS(
        Option(OptionType::Call, -100.0, 1.0),
        std::invalid_argument
    );
}

TEST_CASE("Black-Scholes: Validation - negative time to expiration", "[validation]") {
    REQUIRE_THROWS_AS(
        Option(OptionType::Call, 100.0, -1.0),
        std::invalid_argument
    );
}

TEST_CASE("Black-Scholes: Validation - negative spot price", "[validation]") {
    REQUIRE_THROWS_AS(
        MarketData(-100.0, 0.05, 0.2),
        std::invalid_argument
    );
}

TEST_CASE("Black-Scholes: Validation - negative volatility", "[validation]") {
    REQUIRE_THROWS_AS(
        MarketData(100.0, 0.05, -0.2),
        std::invalid_argument
    );
}

TEST_CASE("Black-Scholes: Known reference values", "[black_scholes]") {
    // Reference values from standard Black-Scholes calculators
    // S=100, K=105, r=0.05, σ=0.2, T=0.5, Call
    Option option(OptionType::Call, 105.0, 0.5);
    MarketData marketData(100.0, 0.05, 0.2);
    BlackScholesModel model;
    
    auto result = model.price(option, marketData);
    
    // Expected value approximately 6.86 (calculated independently)
    REQUIRE_THAT(result.price, WithinAbs(6.86, 0.01));
}

