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
        std::cout << "3. Go back\n";
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
        std::cout << "4. Exit\n";
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
