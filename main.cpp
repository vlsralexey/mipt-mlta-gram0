#include "gram0.h"

#include <iostream>
#include <string>

const char* rule_file_name;
const char* input_file_name;
const char* output_file_name;
const char* trace_file_name = nullptr;

Options opts;
Files files;

void ExitWithError(const std::string& error_message) {
    std::cerr << "\033[1;31mError:\033[0m " << error_message << "\n";
    exit(1);
}

void ParseArguments(int argc, char* argv[]) {
    if (argc < 4) {
        ExitWithError(
            "Usage: gram0 <rule file> <input file> <output file> [-t <trace file>] [-m <max "
            "steps>] [-i] [-d <ms>]");
    }
    rule_file_name = argv[1];
    input_file_name = argv[2];
    output_file_name = argv[3];

    for (int i = 4; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-t") {
            if (i + 1 >= argc) {
                ExitWithError("-t requires a file argument");
            }

            trace_file_name = argv[++i];
        } else if (arg == "-i") {
            opts.interactive = true;
        } else if (arg == "-d") {
            if (i + 1 >= argc) {
                ExitWithError("-d requires a number argument");
            }

            try {
                opts.delay_ms = std::stoi(argv[++i]);
            } catch (...) {
                ExitWithError("-d requires a valid integer");
            }
        } else if (arg == "-m") {
            if (i + 1 >= argc) {
                ExitWithError("-m requires a number argument");
            }

            try {
                opts.max_steps = std::stoi(argv[++i]);
            } catch (...) {
                ExitWithError("-m requires a valid integer");
            }
        } else {
            ExitWithError("Unknown flag: " + arg);
        }
    }
}

void OpenFiles() {
    files.rules.open(rule_file_name);
    if (!files.rules) {
        ExitWithError("Cannot open rule file");
    }
    files.input.open(input_file_name);
    if (!files.input) {
        ExitWithError("Cannot open input file");
    }
    files.output.open(output_file_name);
    if (!files.output) {
        ExitWithError("Cannot open output file");
    }
    if (trace_file_name) {
        files.trace.open(trace_file_name);
        if (!files.trace) {
            ExitWithError("Cannot open trace file");
        }
    }
}

int main(int argc, char* argv[]) {
    ParseArguments(argc, argv);
    OpenFiles();

    std::vector<Rule> rules;
    try {
        rules = ParseRules(files.rules);
    } catch (const std::exception& e) {
        ExitWithError(e.what());
    }

    files.output << Run(GetInput(files), rules, opts, files) << "\n";
}
