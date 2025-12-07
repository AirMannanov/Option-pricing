#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>

#include "../include/pricing/core/Option.hpp"
#include "../include/pricing/core/MarketData.hpp"
#include "../include/pricing/models/BlackScholesModel.hpp"

using namespace pricing;
using namespace pricing::core;
using namespace pricing::models;
using namespace std::chrono;

int main() {
    BlackScholesModel model;
    
    // Test parameters
    const int numOptions = 10000;
    std::vector<Option> options;
    std::vector<MarketData> marketDataVec;
    
    // Generate test data
    for (int i = 0; i < numOptions; ++i) {
        double spot = 90.0 + (i % 20) * 1.0;  // 90-110
        double strike = 95.0 + (i % 15) * 1.0; // 95-110
        double rate = 0.02 + (i % 10) * 0.01;  // 0.02-0.11
        double vol = 0.1 + (i % 20) * 0.01;    // 0.1-0.3
        double maturity = 0.1 + (i % 50) * 0.02; // 0.1-1.0
        
        OptionType type = (i % 2 == 0) ? OptionType::Call : OptionType::Put;
        options.emplace_back(type, strike, maturity);
        marketDataVec.emplace_back(spot, rate, vol);
    }
    
    std::cout << "Benchmark: Pricing " << numOptions << " options\n";
    std::cout << "==========================================\n\n";
    
    // Benchmark: Price only
    auto start = high_resolution_clock::now();
    for (int i = 0; i < numOptions; ++i) {
        model.price(options[i], marketDataVec[i]);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    std::cout << "Price only:\n";
    std::cout << "  Time: " << duration.count() << " microseconds\n";
    std::cout << "  Time: " << duration.count() / 1000.0 << " milliseconds\n";
    std::cout << "  Throughput: " << std::fixed << std::setprecision(2)
              << (numOptions * 1000000.0 / duration.count()) << " options/second\n\n";
    
    // Benchmark: Price with Greeks
    start = high_resolution_clock::now();
    for (int i = 0; i < numOptions; ++i) {
        model.priceWithGreeks(options[i], marketDataVec[i]);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    
    std::cout << "Price with Greeks:\n";
    std::cout << "  Time: " << duration.count() << " microseconds\n";
    std::cout << "  Time: " << duration.count() / 1000.0 << " milliseconds\n";
    std::cout << "  Throughput: " << std::fixed << std::setprecision(2)
              << (numOptions * 1000000.0 / duration.count()) << " options/second\n\n";
    
    // Performance requirement check
    double timeSeconds = duration.count() / 1000000.0;
    std::cout << "Performance check:\n";
    std::cout << "  Requirement: 80% of requests (< 10,000 options) in < 1 second\n";
    std::cout << "  Actual: " << numOptions << " options in " << timeSeconds << " seconds\n";
    
    if (timeSeconds < 1.0) {
        std::cout << "  ✓ PASSED: Meets performance requirement\n";
    } else {
        std::cout << "  ✗ FAILED: Does not meet performance requirement\n";
    }
    
    return 0;
}

