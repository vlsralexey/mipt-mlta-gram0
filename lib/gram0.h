#pragma once
#include <fstream>
#include <istream>
#include <string>
#include <vector>

struct Rule {
    std::string a;
    std::string b;
    Rule(std::string a, std::string b);
};

struct Options {
    int max_steps = 1'000'000;
    bool interactive = false;
    int delay_ms = 1'000;
};

struct Files {
    std::ifstream rules;
    std::ifstream input;
    std::ofstream output;
    std::ofstream trace;
};

std::vector<Rule> ParseRules(std::istream& in);

std::string Run(const std::string& input, const std::vector<Rule>& rules,
                const Options& opts, Files& files);

std::string GetInput(Files& files);
