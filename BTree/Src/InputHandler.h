#include <iostream>
#include <string>
#include <limits>
#include "./Managers/DiskPageManager.h"
#include "./Managers/TreePageManager.h"
#include "Config.h"
#include "BTree.h"

class InputHandler {
public:
    InputHandler(BTree& tree) : bTree(tree) {}

    void run();
private:
    DiskRecord inputRecordFromKeyboard();
    DiskRecord generateRandomRecord();

    void handleInsertOption();

    BTree& bTree;
};
