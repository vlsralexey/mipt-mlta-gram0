#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#define DEBUG true
#define MAX_STEPS 100

struct Rule {
    std::string a;
    std::string b;
};

const char* rule_file_name;
const char* input_file_name;
const char* output_file_name;
const char* trace_file_name = "";

std::regex rule_regex(
    R"(((:?[0-9a-zA-Z_.']+)(,:?[0-9a-zA-Z_.']+)*)\s*->\s*((:?[0-9a-zA-Z_.']+)(,:?[0-9a-zA-Z_.']+)*))");
size_t left_part_group_index = 1;
size_t right_part_group_index = 4;

std::regex stop_regex(R"((,:B)?[^:]*(:E,)?)");

std::string state;
std::vector<Rule> rules;

void ExitWithError(const std::string& error_message) {
    std::cerr << "\033[1;31mError:\033[0m " << error_message << "\n";
    exit(1);
}

void ParseArguments(int argc, char* argv[]) {
    if (argc != 4 && argc != 5) {
        ExitWithError("Usage: <rule file> <input file> <output file> [<trace file>]");
    }
    rule_file_name = argv[1];
    input_file_name = argv[2];
    output_file_name = argv[3];
    if (argc == 5) {
        trace_file_name = argv[4];
    }
}

void ParseRules() {
    std::ifstream rule_file(rule_file_name);
    if (!rule_file) {
        ExitWithError("Cannot open rule file");
    }

    std::string line;
    int line_num = 0;
    while (std::getline(rule_file, line)) {
        ++line_num;
        if (line.empty()) {
            continue;
        }

        std::smatch matches;
        if (!std::regex_match(line, matches, rule_regex)) {
            ExitWithError("Syntax error on line number " + std::to_string(line_num));
        }
        rules.emplace_back(',' + std::string(matches[left_part_group_index]) + ',',
                           ',' + std::string(matches[right_part_group_index]) + ',');
    }
}

void GetInput() {
    std::ifstream input_file(input_file_name);
    if (!input_file) {
        ExitWithError("Cannot open input file");
    }
    std::getline(input_file, state);
    state = ',' + state + ',';
}

bool TryToApplyRule(const Rule& rule) {
    size_t pos = state.find(rule.a);
    if (pos != std::string::npos) {
        state.replace(pos, rule.a.size(), rule.b);
        return true;
    }
    return false;
}

void Run() {
    int step = 1;
    do {
        bool replaced = false;
        for (const auto& rule : rules) {
            if (TryToApplyRule(rule)) {
                if constexpr (DEBUG) {
                    std::cout << rule.a << " -> " << rule.b << "\n";
                }
                replaced = true;
                break;
            }
        }
        if constexpr (DEBUG) {
            std::cout << "Log: " << state << "\n\n";
        }
        if (!replaced) {
            break;
        }
        ++step;
    } while (step <= MAX_STEPS && !std::regex_match(state, stop_regex));
}

void WriteOutput() {
    std::ofstream output_file(output_file_name);
    if (!output_file) {
        ExitWithError("Cannot open output file");
    }
    output_file << state << "\n";
}

int main(int argc, char* argv[]) {
    ParseArguments(argc, argv);
    ParseRules();
    GetInput();

    if constexpr (DEBUG) {
        std::cout << "Run:" << "\n\n";
    }

    Run();
    WriteOutput();
}
