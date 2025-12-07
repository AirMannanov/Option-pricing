#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../include/pricing/core/MarketData.hpp"
#include "../../include/pricing/core/Option.hpp"
#include "../../include/pricing/models/BlackScholesModel.hpp"

namespace {
    void printUsage(const char* programName) {
        std::cerr << "Usage: " << programName << " [OPTIONS]\n"
                  << "Options:\n"
                  << "  --model MODEL          Pricing model (black_scholes)\n"
                  << "  --type TYPE            Option type (call|put)\n"
                  << "  --spot S               Spot price of underlying asset\n"
                  << "  --strike K             Strike price\n"
                  << "  --rate r               Risk-free rate (annual)\n"
                  << "  --vol σ                Volatility (annual)\n"
                  << "  --maturity T           Time to expiration (years)\n"
                  << "  --with-greeks          Calculate and display Greeks\n"
                  << "  --help                 Show this help message\n"
                  << "\nExample:\n"
                  << "  " << programName << " --model black_scholes --type call \\\n"
                  << "     --spot 100 --strike 105 --rate 0.05 --vol 0.2 --maturity 0.5 \\\n"
                  << "     --with-greeks\n";
    }

    double parseDouble(const std::string& arg, const std::string& paramName) {
        try {
            return std::stod(arg);
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid value for " + paramName + ": " + arg);
        }
    }

    pricing::core::OptionType parseOptionType(const std::string& typeStr) {
        if (typeStr == "call") {
            return pricing::core::OptionType::Call;
        } else if (typeStr == "put") {
            return pricing::core::OptionType::Put;
        } else {
            throw std::invalid_argument("Invalid option type: " + typeStr + " (must be 'call' or 'put')");
        }
    }

    struct CliArguments {
        std::string model = "black_scholes";
        pricing::core::OptionType optionType = pricing::core::OptionType::Call;
        double spot = 0.0;
        double strike = 0.0;
        double rate = 0.0;
        double vol = 0.0;
        double maturity = 0.0;
        bool withGreeks = false;
        bool help = false;
    };

    CliArguments parseArguments(int argc, char* argv[]) {
        CliArguments args;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                args.help = true;
                return args;
            } else if (arg == "--model" && i + 1 < argc) {
                args.model = argv[++i];
            } else if (arg == "--type" && i + 1 < argc) {
                args.optionType = parseOptionType(argv[++i]);
            } else if (arg == "--spot" && i + 1 < argc) {
                args.spot = parseDouble(argv[++i], "--spot");
            } else if (arg == "--strike" && i + 1 < argc) {
                args.strike = parseDouble(argv[++i], "--strike");
            } else if (arg == "--rate" && i + 1 < argc) {
                args.rate = parseDouble(argv[++i], "--rate");
            } else if (arg == "--vol" && i + 1 < argc) {
                args.vol = parseDouble(argv[++i], "--vol");
            } else if (arg == "--maturity" && i + 1 < argc) {
                args.maturity = parseDouble(argv[++i], "--maturity");
            } else if (arg == "--with-greeks") {
                args.withGreeks = true;
            } else {
                throw std::invalid_argument("Unknown argument: " + arg);
            }
        }

        return args;
    }

    void validateArguments(const CliArguments& args) {
        if (args.model != "black_scholes") {
            throw std::invalid_argument("Unsupported model: " + args.model + " (only 'black_scholes' is supported)");
        }

        if (args.spot <= 0.0) {
            throw std::invalid_argument("--spot must be specified and positive");
        }
        if (args.strike <= 0.0) {
            throw std::invalid_argument("--strike must be specified and positive");
        }
        if (args.vol < 0.0) {
            throw std::invalid_argument("--vol must be non-negative");
        }
        if (args.maturity < 0.0) {
            throw std::invalid_argument("--maturity must be non-negative");
        }
    }

    void printResult(const pricing::core::PricingResult& result, const CliArguments& args) {
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "\n=== Option Pricing Result ===\n";
        std::cout << "Option Type: " << (args.optionType == pricing::core::OptionType::Call ? "Call" : "Put") << "\n";
        std::cout << "Spot Price: " << args.spot << "\n";
        std::cout << "Strike Price: " << args.strike << "\n";
        std::cout << "Risk-Free Rate: " << args.rate << "\n";
        std::cout << "Volatility: " << args.vol << "\n";
        std::cout << "Time to Expiration: " << args.maturity << " years\n";
        std::cout << "--------------------------------\n";
        std::cout << "Option Price: " << result.price << "\n";
        
        if (result.hasGreeks()) {
            std::cout << "\n--- Greeks ---\n";
            std::cout << "Delta: " << result.delta << "\n";
            std::cout << "Gamma: " << result.gamma << "\n";
            std::cout << "Vega:  " << result.vega << "\n";
            std::cout << "Theta: " << result.theta << "\n";
            std::cout << "Rho:   " << result.rho << "\n";
        }
        
        std::cout << "==============================\n\n";
    }
}

int main(int argc, char* argv[]) {
    try {
        CliArguments args = parseArguments(argc, argv);

        if (args.help) {
            printUsage(argv[0]);
            return 0;
        }

        validateArguments(args);

        // Create option and market data
        pricing::core::Option option(args.optionType, args.strike, args.maturity);
        pricing::core::MarketData marketData(args.spot, args.rate, args.vol);

        // Create model and calculate price
        pricing::models::BlackScholesModel model;
        pricing::core::PricingResult result;
        if (args.withGreeks) {
            result = model.priceWithGreeks(option, marketData);
        } else {
            result = model.price(option, marketData);
        }

        // Print result
        printResult(result, args);

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        printUsage(argv[0]);
        return 1;
    }
}
