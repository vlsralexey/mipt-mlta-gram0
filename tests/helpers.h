#pragma once
#include "gram0.h"

#include <fstream>
#include <sstream>

static std::vector<Rule> ParseRulesFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open " + path);
    }
    return ParseRules(file);
}

static std::vector<Rule> ParseRulesStr(const std::string& s) {
    std::istringstream in(s);
    return ParseRules(in);
}

static std::string RunSimple(const std::string& input, const std::vector<Rule>& rules,
                             int max_steps = 1'000'000) {
    Files files;
    Options opts;
    opts.max_steps = max_steps;
    return Run(input, rules, opts, files);
}
