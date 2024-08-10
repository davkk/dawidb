#include <cassert>
#include <cstdlib>
#include <format>
#include <iostream>
#include <ostream>

enum class Command {
    EXIT,
};

enum class StatementType {
    INSERT,
    SELECT,
};

struct Statement {
    StatementType type;
};

std::ostream& prompt() {
    return std::cout << "\ndawidb> ";
}

std::optional<Command> parse_command(const std::string& command) {
    assert(!command.empty());

    if (command == ".exit") {
        return Command::EXIT;
    }

    return std::nullopt;
}

std::optional<Statement> parse_statement(const std::string& statement) {
    assert(!statement.empty());

    auto type = statement.substr(0, statement.find(' '));

    if (type == "select") {
        return Statement{.type = StatementType::SELECT};
    }

    if (type == "insert") {
        return Statement{.type = StatementType::INSERT};
    }

    return std::nullopt;
}

void exec(const Command& command) {
    switch (command) {
    case Command::EXIT:
        std::cerr << "successful exit\n";
        std::exit(0);
        break;
    }
}

void exec(const Statement& statement) {
    switch (statement.type) {
    case StatementType::SELECT:
        std::cout << "select statement\n";
        break;
    case StatementType::INSERT:
        std::cout << "insert statement\n";
        break;
    }
}

int main() {
    std::string line{};

    while (true) {
        prompt();
        std::getline(std::cin, line);

        if (line.empty()) {
            continue;
        }

        auto command{parse_command(line)};
        if (line.front() == '.' && command) {
            exec(*command);
        } else if (auto statement{parse_statement(line)}) {
            exec(*statement);
        } else {
            std::cerr << std::format("unrecognized: `{}`\n", line);
            continue;
        }
    }

    return 0;
}
