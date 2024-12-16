#include "InputHandler.h"

DiskRecord InputHandler::generateRandomRecord()
{
    return DiskRecord(rand() % 100, rand() % 100, rand() % 100);
}

void InputHandler::handleInsertCommand(std::istringstream& stream) {
    std::string subcommand;
    stream >> subcommand;

    if (subcommand == "-r") {
        int count = 1;
        stream >> count;
        for (int i = 0; i < count; ++i) {
            DiskRecord record = generateRandomRecord();
            if (bTree.InsertRecord(record)) {
                std::cout << "Inserted random record: ";
            }
            else {
                std::cout << "Record already exists: ";
            }
            record.Print();
            std::cout << "\n";
        }
    }
    else if (subcommand == "-s") {
        int id, a, b, alpha;
        stream >> id >> a >> b >> alpha;
        if (!stream.fail()) {
            DiskRecord record(a, b, alpha, id);
            if (bTree.InsertRecord(record)) {
                std::cout << "Inserted record: ";
            }
            else {
                std::cout << "Record already exists: ";
            }
            record.Print();
            std::cout << "\n";
        }
        else {
            std::cerr << "Invalid insert command format.\n";
        }
    }
    else {
        std::cerr << "Invalid insert command. Use 'insert -s' or 'insert -r'.\n";
    }
}

void InputHandler::handleSearchCommand(std::istringstream& stream) {
    int id;
    stream >> id;
    if (!stream.fail()) {
        DiskRecord result = bTree.SearchRecord(id);
        if (result.GetId() != NULLPTR) {
            std::cout << "Record found: ";
            result.Print();
            std::cout << "\n";
        }
        else {
            std::cout << "Record not found.\n";
        }
    }
    else {
        std::cerr << "Invalid search command format.\n";
    }
}

void InputHandler::handleDeleteCommand(std::istringstream& stream) {
    int id;
    stream >> id;
    if (!stream.fail()) {
        if (bTree.DeleteRecord(id)) {
            std::cout << "Record " << id << " deleted.\n";
        }
        else {
            std::cout << "Record not found for ID: " << id << "\n";
        }
    }
    else {
        std::cerr << "Invalid delete command format.\n";
    }
}

void InputHandler::handleUpdateCommand(std::istringstream& stream) {
    int id, a, b, alpha;
    stream >> id >> a >> b >> alpha;
    if (!stream.fail()) {
        DiskRecord updatedRecord(a, b, alpha, id);
        if (bTree.UpdateRecord(updatedRecord)) {
            std::cout << "Updated record: ";
        }
        else {
            std::cout << "Record not found for update: ";
        }
        updatedRecord.Print();
        std::cout << "\n";
    }
    else {
        std::cerr << "Invalid update command format.\n";
    }
}

void InputHandler::handlePrintCommand(std::istringstream& stream) {
    std::string subcommand;
    stream >> subcommand;

    if (subcommand == "-t") {
        bTree.Print();
    }
    else if (subcommand == "-d") {
        bTree.PrintDiskFile();
    }
    else if (subcommand == "-c") {
        bTree.PrintIOCounters("");
    }
    else {
        std::cerr << "Invalid print command. Use 'print -t', 'print -d', or 'print -c'.\n";
    }
}

void InputHandler::handleHelpCommand() {
    std::cout << "Available commands:\n";
    std::cout << "  insert -s <id> <a> <b> <alpha>   - Insert a specific record with given values\n";
    std::cout << "  insert -r [count]                - Insert random records (default count: 1)\n";
    std::cout << "  search <id>                      - Search for a record by its ID\n";
    std::cout << "  delete <id>                      - Delete a record by its ID\n";
    std::cout << "  update <id> <a> <b> <alpha>      - Update a record with new values\n";
    std::cout << "  print -t                         - Print the B-tree structure\n";
    std::cout << "  print -d                         - Print the disk file contents\n";
    std::cout << "  print -c                         - Print I/O operation counters\n";
    std::cout << "  load <filename>                  - Load and execute commands from a file\n";
    std::cout << "  help                             - Show this help message\n";
    std::cout << "  exit                             - Exit the shell\n";
}


void InputHandler::handleLoadCommand(std::istringstream& stream) {
    std::string filename;
    stream >> filename;

    filename = "./Src/Data/" + filename;

    if (filename.empty()) {
        std::cerr << "Error: Filename not provided.\n";
        return;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream commandStream(line);
        std::string command;
        commandStream >> command;

        if (command == "insert") {
            handleInsertCommand(commandStream);
        }
        else if (command == "search") {
            handleSearchCommand(commandStream);
        }
        else if (command == "delete") {
            handleDeleteCommand(commandStream);
        }
        else if (command == "update") {
            handleUpdateCommand(commandStream);
        }
        else if (command == "print") {
            handlePrintCommand(commandStream);
        }
        else {
            std::cerr << "Unknown command in file: " << line << "\n";
        }
    }

    file.close();
}

void InputHandler::runShell() {
    std::cout << "\nWelcome to the B-Tree Shell Interface!\n";
    std::cout << "Type 'help' to see a list of available commands.\n";
    std::cout << "------------------------------------------------\n";

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        std::istringstream stream(input);
        std::string command;
        stream >> command;

        if (command == "insert") {
            handleInsertCommand(stream);
        }
        else if (command == "search") {
            handleSearchCommand(stream);
        }
        else if (command == "delete") {
            handleDeleteCommand(stream);
        }
        else if (command == "update") {
            handleUpdateCommand(stream);
        }
        else if (command == "print") {
            handlePrintCommand(stream);
        }
        else if (command == "help") {
            handleHelpCommand();
        }
        else if (command == "load") {
            handleLoadCommand(stream);
        }
        else if (command == "exit") {
            std::cout << "Goodbye!\n";
            break;
        }
        else {
            std::cerr << "Unknown command: " << command << "\n";
            std::cerr << "Type 'help' for a list of available commands.\n";
        }
    }
}
