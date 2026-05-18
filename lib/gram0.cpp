#include "gram0.h"

#include <chrono>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <thread>

Rule::Rule(std::string a, std::string b)
    : a(std::move(a)),
      b(std::move(b)) {
}

static std::regex rule_regex(
    R"(((?::?[0-9a-zA-Z_.']+)(?:,:?[0-9a-zA-Z_.']+)*)\s*->\s*((?::?[0-9a-zA-Z_.']+)(?:,:?[0-9a-zA-Z_.']+)*))");

static std::regex stop_regex(R"((,:B)?[^:]*(:E,)?)");

std::vector<Rule> ParseRules(std::istream& in) {
    std::vector<Rule> rules;
    std::string line;
    int line_num = 0;
    while (std::getline(in, line)) {
        ++line_num;
        if (line.empty()) {
            continue;
        }

        std::smatch matches;
        if (!std::regex_match(line, matches, rule_regex)) {
            throw std::runtime_error("Syntax error on line " + std::to_string(line_num));
        }
        rules.emplace_back(',' + std::string(matches[1]) + ',',
                           ',' + std::string(matches[2]) + ',');
    }
    return rules;
}

static std::string Crop(const std::string& s) {
    return s.substr(1, s.size() - 2);
}

static bool TryToApplyRule(std::string& state, const Rule& rule) {
    size_t pos = state.find(rule.a);
    if (pos != std::string::npos) {
        state.replace(pos, rule.a.size(), rule.b);
        return true;
    }
    return false;
}

std::string Run(const std::string& input, const std::vector<Rule>& rules, const Options& opts,
                Files& files) {
    std::string state = ',' + input + ',';
    int step = 1;
    do {
        bool replaced = false;
        for (const auto& rule : rules) {
            if (TryToApplyRule(state, rule)) {
                if (opts.interactive) {
                    std::cout << Crop(state) << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(opts.delay_ms));
                }
                if (files.trace.is_open()) {
                    files.trace << "Step " << step << "\n";
                    files.trace << "Rule: " << Crop(rule.a) << " -> " << Crop(rule.b) << "\n";
                    files.trace << "State: " << Crop(state) << "\n\n";
                }

                replaced = true;
                break;
            }
        }
        if (!replaced) {
            break;
        }
        ++step;
    } while (step <= opts.max_steps && !std::regex_match(state, stop_regex));

    return Crop(state);
}

std::string GetInput(Files& files) {
    std::string input;
    std::getline(files.input, input);
    return input;
}
