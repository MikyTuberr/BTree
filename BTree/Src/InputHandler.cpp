#include "InputHandler.h"

DiskRecord InputHandler::inputRecordFromKeyboard()
{
    int id, a, b, alpha;
    std::cout << "Enter record details:\n";
    std::cout << "ID: ";
    std::cin >> id;
    std::cout << "A side: ";
    std::cin >> a;
    std::cout << "B side: ";
    std::cin >> b;
    std::cout << "Alpha: ";
    std::cin >> alpha;
    return DiskRecord(a, b, alpha, id);
}

DiskRecord InputHandler::generateRandomRecord()
{
    static int counter = 1;
    return DiskRecord(counter, rand() % 100, rand() % 100);
}

void InputHandler::handleInsertOption()
{
    while (true) {
        std::cout << "\n--- Insert Menu ---\n";
        std::cout << "1. Insert random record\n";
        std::cout << "2. Insert record from keyboard\n";
        std::cout << "3. Print tree\n";
        std::cout << "4. Go back\n";
        std::cout << "Choose an option: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            DiskRecord randomRecord = generateRandomRecord();
            if (bTree.InsertRecord(randomRecord)) {
                std::cout << "Inserted random record:\n";
            }
            else {
                std::cout << "Record already exists:\n";
            }
            randomRecord.Print();
            std::cout << "\n";
        }
        else if (choice == 2) {
            DiskRecord keyboardRecord = inputRecordFromKeyboard();
            if (bTree.InsertRecord(keyboardRecord)) {
                std::cout << "Inserted record from keyboard:\n";
            }
            else {
                std::cout << "Record already exists:\n";
            }
            keyboardRecord.Print();
            std::cout << "\n";
        }
        else if (choice == 3) {
            bTree.Print();
        }
        else if (choice == 4) {
            break;
        }
        else {
            std::cout << "Invalid option. Try again.\n";
        }
    }
}

void InputHandler::run()
{
    while (true) {
        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Search record\n";
        std::cout << "2. Insert record\n";
        std::cout << "3. Print tree\n";
        std::cout << "4. Print disk file\n";
        std::cout << "5. Process commands from file\n";
        std::cout << "6. Exit\n";
        std::cout << "Choose an option: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            int id;
            std::cout << "Enter the ID of the record to search: ";
            std::cin >> id;
            TreeRecord result = bTree.FindRecord(id);
            if (result.GetId() != NULLPTR) {
                std::cout << "Record found: ";
                result.Print();
                std::cout << "\n";
            }
            else {
                std::cout << "Record not found.\n";
            }
        }
        else if (choice == 2) {
            handleInsertOption();
        }
        else if (choice == 3) {
            bTree.Print();
        }
        else if (choice == 4) {
            bTree.PrintDiskFile();
        }
        else if (choice == 5) {
            std::string filename;
            std::cout << "Enter the filename: ";
            std::cin >> filename;
            this->processFileCommands(filename);
        }
        else if (choice == 6) {
            std::cout << "Exiting program.\n";
            break;
        }
        else {
            std::cout << "Invalid option. Try again.\n";
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void InputHandler::processFileCommands(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string command;
        stream >> command;

        if (command == "insert") {
            int id, a, b, alpha;
            stream >> id >> a >> b >> alpha;
            if (!stream.fail()) {
                DiskRecord record(id, a, b, alpha);
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
                std::cerr << "Error: Invalid insert command format in line: " << line << "\n";
            }
        }
        else if (command == "search") {
            int id;
            stream >> id;
            if (!stream.fail()) {
                TreeRecord result = bTree.FindRecord(id);
                if (result.GetId() != NULLPTR) {
                    std::cout << "Record found: ";
                    result.Print();
                    std::cout << "\n";
                }
                else {
                    std::cout << "Record not found for ID: " << id << "\n";
                }
            }
            else {
                std::cerr << "Error: Invalid search command format in line: " << line << "\n";
            }
        }
        else {
            std::cerr << "Error: Unknown command in line: " << line << "\n";
        }
    }

    file.close();
}