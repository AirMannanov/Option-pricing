#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "../../include/pricing/core/MarketData.hpp"
#include "../../include/pricing/core/Option.hpp"
#include "../../include/pricing/models/BlackScholesModel.hpp"

using namespace pricing;
using namespace pricing::core;
using namespace pricing::models;

struct BenchmarkResult {
    size_t count;
    double timeSeconds;
    double opsPerSecond;
};

BenchmarkResult runBenchmark(size_t numOptions, bool withGreeks) {
    BlackScholesModel model;
    std::vector<Option> options;
    std::vector<MarketData> marketDataVec;

    // Generate random test data
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> spotDist(50.0, 150.0);
    std::uniform_real_distribution<double> strikeDist(50.0, 150.0);
    std::uniform_real_distribution<double> rateDist(0.01, 0.1);
    std::uniform_real_distribution<double> volDist(0.1, 0.5);
    std::uniform_real_distribution<double> maturityDist(0.1, 2.0);
    std::uniform_int_distribution<int> typeDist(0, 1);

    for (size_t i = 0; i < numOptions; ++i) {
        OptionType type = (typeDist(gen) == 0) ? OptionType::Call : OptionType::Put;
        double strike = strikeDist(gen);
        double maturity = maturityDist(gen);
        options.emplace_back(type, strike, maturity);

        double spot = spotDist(gen);
        double rate = rateDist(gen);
        double vol = volDist(gen);
        marketDataVec.emplace_back(spot, rate, vol);
    }

    // Run benchmark
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numOptions; ++i) {
        if (withGreeks) {
            model.priceWithGreeks(options[i], marketDataVec[i]);
        } else {
            model.price(options[i], marketDataVec[i]);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double timeSeconds = duration.count() / 1'000'000.0;

    BenchmarkResult result;
    result.count = numOptions;
    result.timeSeconds = timeSeconds;
    result.opsPerSecond = numOptions / timeSeconds;

    return result;
}

int main() {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== Option Pricing Benchmark ===\n\n";

    const size_t testSizes[] = {100, 1000, 10000};
    const size_t numSizes = sizeof(testSizes) / sizeof(testSizes[0]);

    std::cout << "Testing pricing without Greeks:\n";
    std::cout << "--------------------------------\n";
    std::cout << std::setw(10) << "Options" 
              << std::setw(15) << "Time (s)" 
              << std::setw(20) << "Ops/sec" << "\n";
    std::cout << "--------------------------------\n";

    for (size_t i = 0; i < numSizes; ++i) {
        auto result = runBenchmark(testSizes[i], false);
        std::cout << std::setw(10) << result.count
                  << std::setw(15) << result.timeSeconds
                  << std::setw(20) << result.opsPerSecond << "\n";
    }

    std::cout << "\nTesting pricing with Greeks:\n";
    std::cout << "--------------------------------\n";
    std::cout << std::setw(10) << "Options" 
              << std::setw(15) << "Time (s)" 
              << std::setw(20) << "Ops/sec" << "\n";
    std::cout << "--------------------------------\n";

    for (size_t i = 0; i < numSizes; ++i) {
        auto result = runBenchmark(testSizes[i], true);
        std::cout << std::setw(10) << result.count
                  << std::setw(15) << result.timeSeconds
                  << std::setw(20) << result.opsPerSecond << "\n";
    }

    std::cout << "\n=== Benchmark Complete ===\n";
    return 0;
}
