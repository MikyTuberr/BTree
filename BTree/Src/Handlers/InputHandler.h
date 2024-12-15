#include <iostream>
#include <string>
#include <limits>
#include "../Managers/DiskPageManager.h"
#include "../Managers/TreePageManager.h"
#include "../Config.h"
#include "../BTree.h"

class InputHandler {
public:
    InputHandler(BTree& tree) : bTree(tree) {}

    void runShell();
private:
    DiskRecord generateRandomRecord();

    void handleInsertCommand(std::istringstream& stream);
    void handleSearchCommand(std::istringstream& stream);
    void handleDeleteCommand(std::istringstream& stream);
    void handleUpdateCommand(std::istringstream& stream);
    void handlePrintCommand(std::istringstream& stream);
    void handleHelpCommand();
    void handleLoadCommand(std::istringstream& stream);

    BTree& bTree;
};
