
std::optional<Command> parse_command(const std::string& command) {
    assert(!command.empty());

    if (command == ".exit") {
        return Command::EXIT;
    }

    return std::nullopt;
}

StatementResult parse_statement(
    const std::string& input, Statement& statement
) {
    assert(!input.empty());

    std::string_view view{input};

    size_t pos_l{0};
    size_t pos_r{view.find(' ')};

    if (pos_r == std::string::npos) {
        return StatementResult::ERROR_SYNTAX;
    }

    assert(pos_l < view.size() && pos_r < view.size() && pos_l <= pos_r);

    std::string_view type{view.substr(pos_l, pos_r)};

    if (type == "insert") {
        statement.type = StatementType::INSERT;

        assert(pos_l < pos_r);

        while (pos_l < view.size()) {
            pos_l = pos_r + 1;
            pos_r = view.find(' ', pos_l);

            if (pos_r == std::string::npos) {
                statement.values.push_back(view.substr(pos_l));
                break;
            }

            statement.values.push_back(view.substr(pos_l, pos_r - pos_l));
        }

        if (statement.values.size() < 3) {
            return StatementResult::ERROR_SYNTAX;
        }

        return StatementResult::SUCCESS;
    }

    if (type == "select") {
        statement.type = StatementType::SELECT;
        return StatementResult::SUCCESS;
    }

    return StatementResult::ERROR_UNRECOGNIZED;
}
