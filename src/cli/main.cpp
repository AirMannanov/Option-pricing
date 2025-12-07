#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>

#include "../../include/pricing/core/MarketData.hpp"
#include "../../include/pricing/core/Option.hpp"
#include "../../include/pricing/models/BlackScholesModel.hpp"

namespace {
    void printUsage(const char* programName) {
        std::cerr << "Usage: " << programName << " [OPTIONS]\n"
                  << "\nSingle calculation mode:\n"
                  << "  --model MODEL          Pricing model (black_scholes)\n"
                  << "  --type TYPE            Option type (call|put)\n"
                  << "  --spot S               Spot price of underlying asset\n"
                  << "  --strike K             Strike price\n"
                  << "  --rate r               Risk-free rate (annual)\n"
                  << "  --vol σ                Volatility (annual)\n"
                  << "  --maturity T           Time to expiration (years)\n"
                  << "  --with-greeks          Calculate and display Greeks\n"
                  << "\nBatch processing mode:\n"
                  << "  --batch-input FILE     Input CSV file\n"
                  << "  --batch-output FILE    Output CSV file\n"
                  << "  --with-greeks          Include Greeks in output\n"
                  << "\nOther:\n"
                  << "  --help                 Show this help message\n"
                  << "\nExample (single):\n"
                  << "  " << programName << " --model black_scholes --type call \\\n"
                  << "     --spot 100 --strike 105 --rate 0.05 --vol 0.2 --maturity 0.5 \\\n"
                  << "     --with-greeks\n"
                  << "\nExample (batch):\n"
                  << "  " << programName << " --batch-input options.csv --batch-output results.csv \\\n"
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
        std::string batchInputFile;
        std::string batchOutputFile;
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
            } else if (arg == "--batch-input" && i + 1 < argc) {
                args.batchInputFile = argv[++i];
            } else if (arg == "--batch-output" && i + 1 < argc) {
                args.batchOutputFile = argv[++i];
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

        // Batch mode validation
        if (!args.batchInputFile.empty() || !args.batchOutputFile.empty()) {
            if (args.batchInputFile.empty()) {
                throw std::invalid_argument("--batch-input is required when using batch mode");
            }
            if (args.batchOutputFile.empty()) {
                throw std::invalid_argument("--batch-output is required when using batch mode");
            }
            return; // Skip single mode validation in batch mode
        }

        // Single mode validation
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

    std::vector<std::string> splitCSVLine(const std::string& line) {
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;

        while (std::getline(ss, field, ',')) {
            // Trim whitespace
            field.erase(0, field.find_first_not_of(" \t"));
            field.erase(field.find_last_not_of(" \t") + 1);
            fields.push_back(field);
        }

        return fields;
    }

    struct OptionRow {
        std::string type;
        double spot;
        double strike;
        double rate;
        double vol;
        double maturity;
    };

    std::vector<OptionRow> readCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open input file: " + filename);
        }

        std::vector<OptionRow> rows;
        std::string line;
        bool isFirstLine = true;

        while (std::getline(file, line)) {
            // Skip empty lines
            if (line.empty() || (line.find_first_not_of(" \t") == std::string::npos)) {
                continue;
            }

            // Skip header line
            if (isFirstLine) {
                isFirstLine = false;
                continue;
            }

            auto fields = splitCSVLine(line);
            if (fields.size() < 6) {
                throw std::runtime_error("Invalid CSV line (expected 6 fields): " + line);
            }

            OptionRow row;
            row.type = fields[0];
            row.spot = parseDouble(fields[1], "spot");
            row.strike = parseDouble(fields[2], "strike");
            row.rate = parseDouble(fields[3], "rate");
            row.vol = parseDouble(fields[4], "vol");
            row.maturity = parseDouble(fields[5], "maturity");

            rows.push_back(row);
        }

        file.close();
        return rows;
    }

    void writeCSV(const std::string& filename, 
                  const std::vector<OptionRow>& inputRows,
                  const std::vector<pricing::core::PricingResult>& results,
                  bool withGreeks) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + filename);
        }

        file << std::fixed << std::setprecision(6);

        // Write header
        file << "type,spot,strike,rate,vol,maturity,price";
        if (withGreeks) {
            file << ",delta,gamma,vega,theta,rho";
        }
        file << "\n";

        // Write data rows
        for (size_t i = 0; i < inputRows.size() && i < results.size(); ++i) {
            const auto& row = inputRows[i];
            const auto& result = results[i];

            file << row.type << ","
                 << row.spot << ","
                 << row.strike << ","
                 << row.rate << ","
                 << row.vol << ","
                 << row.maturity << ","
                 << result.price;

            if (withGreeks) {
                file << "," << result.delta
                     << "," << result.gamma
                     << "," << result.vega
                     << "," << result.theta
                     << "," << result.rho;
            }
            file << "\n";
        }

        file.close();
    }

    void processBatch(const CliArguments& args) {
        // Read input CSV
        auto inputRows = readCSV(args.batchInputFile);

        if (inputRows.empty()) {
            throw std::runtime_error("Input file is empty or contains no data rows");
        }

        // Process each row
        pricing::models::BlackScholesModel model;
        std::vector<pricing::core::PricingResult> results;

        for (const auto& row : inputRows) {
            try {
                pricing::core::OptionType optionType = parseOptionType(row.type);
                pricing::core::Option option(optionType, row.strike, row.maturity);
                pricing::core::MarketData marketData(row.spot, row.rate, row.vol);

                pricing::core::PricingResult result;
                if (args.withGreeks) {
                    result = model.priceWithGreeks(option, marketData);
                } else {
                    result = model.price(option, marketData);
                }

                results.push_back(result);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Error processing row: " << e.what() << "\n";
                // Add empty result to maintain row alignment
                pricing::core::PricingResult emptyResult;
                results.push_back(emptyResult);
            }
        }

        // Write output CSV
        writeCSV(args.batchOutputFile, inputRows, results, args.withGreeks);

        std::cout << "Processed " << inputRows.size() << " options. Results written to " 
                  << args.batchOutputFile << "\n";
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

        // Check if batch mode
        if (!args.batchInputFile.empty()) {
            processBatch(args);
            return 0;
        }

        // Single calculation mode
        pricing::core::Option option(args.optionType, args.strike, args.maturity);
        pricing::core::MarketData marketData(args.spot, args.rate, args.vol);

        pricing::models::BlackScholesModel model;
        pricing::core::PricingResult result;
        if (args.withGreeks) {
            result = model.priceWithGreeks(option, marketData);
        } else {
            result = model.price(option, marketData);
        }

        printResult(result, args);

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        printUsage(argv[0]);
        return 1;
    }
}
