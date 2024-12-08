#include "BTree.h"
//
// TODO: Zweryfikuj czy nie u¿ywasz za du¿o pamiêci
//

bool BTree::InsertRecord(DiskRecord diskRecord)
{
    std::size_t diskRecordId = diskRecord.GetId();

    // krok 1
    TreeRecord treeRecord = this->FindRecord(diskRecordId);

    if (treeRecord.GetId() != NULLPTR) {
        return false;
    }

    std::size_t currentPageNumber = this->GetLeaf(diskRecordId);

    // krok 2
    TreeRecord recordToInsert = TreeRecord(diskRecordId, this->diskPageManager.GetPagesCounter());

    while (true) {
        TreePage* currentPage = treePageManager.ReadPageWithCache(currentPageNumber);

        // krok 3
        if (!currentPage->isOverflow()) {
            currentPage->InsertRecord(recordToInsert);
            diskPageManager.InsertRecordToBuffer(diskRecord);
            break;
        }

        // krok 4
        // TODO: compensation has some exotic edge case
        if (this->TryCompensation(currentPage, recordToInsert)) {
            diskPageManager.InsertRecordToBuffer(diskRecord);
            // krok 5 
            break;
        }

        // krok 6
        std::size_t rootNumberBeforeSplit = this->treePageManager.GetRootNumber();
        recordToInsert = this->SplitPage(currentPage, recordToInsert);
        diskPageManager.InsertRecordToBuffer(diskRecord);

        if (currentPageNumber == rootNumberBeforeSplit) {
            break;
        }

        // krok 7
        currentPageNumber = this->treePageManager.FindParentNumber(currentPage->GetPageNumber());
    }

    this->treePageManager.FlushPageCache();
   
    std::cout << "INSERT R:" << this->treePageManager.GetPagesReadCounter() << " W:" 
        << this->treePageManager.GetPagesWrittenCounter() << "\n";
    //std::cout << "DISK      R:" << this->diskPageManager.GetPagesReadCounter() << "      W:" 
        //<< this->diskPageManager.GetPagesWrittenCounter() << "\n";

	return true;
}

TreeRecord BTree::FindRecord(std::size_t treeRecordId)
{
    std::size_t currentPageNumber = this->treePageManager.GetRootNumber();

    if (currentPageNumber == NULLPTR) {
        std::cout << "SEARCH R:" << this->treePageManager.GetPagesReadCounter() << " W:"
            << this->treePageManager.GetPagesWrittenCounter() << "\n";
        return TreeRecord();
    }

    while (currentPageNumber != NULLPTR)
    {
        TreePage* currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);
        TreeRecord foundRecord = currentPage->FindRecordById(treeRecordId);

        if (foundRecord.GetId() != NULLPTR) {
            std::cout << "SEARCH R:" << this->treePageManager.GetPagesReadCounter() << " W:"
                << this->treePageManager.GetPagesWrittenCounter() << "\n";
            return foundRecord;
        }

        std::vector<TreeRecord> records = currentPage->GetRecords();

        std::size_t nextPageNumber = NULLPTR;
        // TODO: cleanup powtarza siê kod
        for (std::size_t i = 0; i < records.size(); ++i)
        {
            if (treeRecordId < records[i].GetId())
            {
                if (i != 0) {
                    nextPageNumber = currentPage->GetRightChildPageNumberById(i - 1);
                }
                else {
                    nextPageNumber = currentPage->GetHeadLeftChildPageNumber();
                }
                break;
            }
        }

        // Jeœli nie znaleziono mniejszego klucza, przechodzimy do ostatniej strony dziecka
        if (nextPageNumber == NULLPTR)
        {
            nextPageNumber = currentPage->GetRightChildPageNumberById(records.size() - 1);
        }

        currentPageNumber = nextPageNumber;
    }

    std::cout << "SEARCH R:" << this->treePageManager.GetPagesReadCounter() << " W:"
        << this->treePageManager.GetPagesWrittenCounter() << "\n";
    return TreeRecord();
}

std::size_t BTree::GetLeaf(std::size_t treeRecordId) {
    std::size_t currentPageNumber = this->treePageManager.GetRootNumber();

    if (currentPageNumber == NULLPTR) {
        return NULLPTR;
    }

    while (currentPageNumber != NULLPTR) {
        TreePage* currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);

        std::vector<TreeRecord> records = currentPage->GetRecords();

        std::size_t nextPageNumber = NULLPTR;
        // TODO: cleanup powtarza siê kod
        for (std::size_t i = 0; i < records.size(); ++i) 
        {
            if (treeRecordId < records[i].GetId())
            {
                if (i != 0) {
                    nextPageNumber = currentPage->GetRightChildPageNumberById(i - 1);
                }
                else {
                    nextPageNumber = currentPage->GetHeadLeftChildPageNumber();
                }
                break;
            }
        }

        if (nextPageNumber == NULLPTR) {
            nextPageNumber = currentPage->GetRightChildPageNumberById(records.size() - 1);
        }

        if (nextPageNumber == NULLPTR) {
            return currentPageNumber;
        }

        currentPageNumber = nextPageNumber;
    }

    return NULLPTR;
}

bool BTree::TryCompensation(TreePage* currentPage, TreeRecord recordToInsert)
{
    std::size_t parentPageNumber = this->treePageManager.FindParentNumber(currentPage->GetPageNumber());

    if (parentPageNumber == NULLPTR) {
        return false;
    }

    TreePage* parentPage = treePageManager.ReadPageWithCache(parentPageNumber);

    std::vector<SiblingInfo> siblings = DetermineSibling(currentPage, parentPage);

    if (siblings.empty()) {
        return false;
    }

    SiblingInfo sibling = SiblingInfo(TreeRecord(), RIGHT_SIBLING, NULLPTR);
    TreePage* siblingPage = nullptr;

    for (const auto& sibli : siblings) {
        siblingPage = treePageManager.ReadPageWithCache(sibli.siblingNumber);

        if (!siblingPage->isOverflow()) {
            sibling = sibli;
            break;
        }
    }

    if (sibling.siblingNumber == NULLPTR) {
        return false;
    }

    SiblingType siblingType = sibling.siblingType;
    TreeRecord separatorRecord = sibling.separatorRecord;


    if (siblingType == LEFT_SIBLING) {
        currentPage->InsertRecord(recordToInsert);

        TreeRecord head = currentPage->PopHead();

        std::size_t separatorRecordRightChildNumber = separatorRecord.GetTreeRightChildNumber();

        separatorRecord.SetTreeRightChildNumber(currentPage->GetHeadLeftChildPageNumber());
        currentPage->SetHeadLeftChildPageNumber(head.GetTreeRightChildNumber());
        head.SetTreeRightChildNumber(separatorRecordRightChildNumber);

        parentPage->ReplaceRecord(separatorRecord, head);
        siblingPage->InsertRecord(separatorRecord);
    }
    else {
        currentPage->InsertRecord(recordToInsert);

        TreeRecord tail = currentPage->PopTail();

        std::size_t separatorRecordRightChildNumber = separatorRecord.GetTreeRightChildNumber();

        separatorRecord.SetTreeRightChildNumber(siblingPage->GetHeadLeftChildPageNumber());
        siblingPage->SetHeadLeftChildPageNumber(tail.GetTreeRightChildNumber());
        tail.SetTreeRightChildNumber(separatorRecordRightChildNumber);

        parentPage->ReplaceRecord(separatorRecord, tail);
        siblingPage->InsertRecord(separatorRecord);
    }

    return true;
}

std::vector<SiblingInfo> BTree::DetermineSibling(TreePage* currentPage, TreePage* parentPage)
{
    std::vector<TreeRecord> parentRecords = parentPage->GetRecords();
    std::size_t currentPageNumber = currentPage->GetPageNumber();
    std::vector<SiblingInfo> siblings;

    // TODO: za du¿e zag³êbienie jakiœ cleanup tu by siê przyda³

    for (std::size_t i = 0; i < parentRecords.size(); ++i) { 
        if ((i > 0 && parentPage->GetRightChildPageNumberById(i - 1) == currentPageNumber) ||
            (parentPage->GetHeadLeftChildPageNumber() == currentPageNumber))
        {
            std::size_t rightSiblingNumber = parentPage->GetRightChildPageNumberById(i);
            TreeRecord separatorRecord = parentRecords[i];
            siblings.push_back(SiblingInfo(separatorRecord, RIGHT_SIBLING, rightSiblingNumber));
        }

        if (parentPage->GetRightChildPageNumberById(i) == currentPageNumber) {
            if (i == 0) {
                std::size_t leftSiblingNumber = parentPage->GetHeadLeftChildPageNumber();
                if (leftSiblingNumber != NULLPTR) {
                    TreeRecord separatorRecord = parentRecords[i];
                    siblings.push_back(SiblingInfo(separatorRecord, LEFT_SIBLING, leftSiblingNumber));
                }
            }
            else {
                std::size_t leftSiblingNumber = parentPage->GetRightChildPageNumberById(i - 1);
                TreeRecord separatorRecord = parentRecords[i];
                siblings.push_back(SiblingInfo(separatorRecord, LEFT_SIBLING, leftSiblingNumber));
            }
        }
    }

    return siblings;
}

TreeRecord BTree::SplitPage(TreePage* pageToSplit, TreeRecord recordToInsert)
{
    std::size_t recordsSize = pageToSplit->GetRecords().size();

    pageToSplit->InsertRecord(recordToInsert);

    std::vector<TreeRecord> records = pageToSplit->GetRecords();

    std::size_t medianIndex = recordsSize / 2;
    TreeRecord medianRecord = records[medianIndex];

    TreePage newPage = this->treePageManager.CreateNewPage();
    newPage.SetHeadLeftChildPageNumber(medianRecord.GetTreeRightChildNumber());
    std::size_t parentPageNumber = this->treePageManager.FindParentNumber(pageToSplit->GetPageNumber());

    if (parentPageNumber == NULLPTR) {
        TreePage newRoot = treePageManager.CreateNewPage();
        newRoot.SetHeadLeftChildPageNumber(pageToSplit->GetPageNumber());
        medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());
        newRoot.SetRecords({ medianRecord });

        std::size_t newRootPageNumber = newRoot.GetPageNumber();
        treePageManager.WritePage(newRoot);
        treePageManager.SetRootNumber(newRootPageNumber);

    }

    pageToSplit->SetRecords(std::vector<TreeRecord>(records.begin(), records.begin() + medianIndex));
    newPage.SetRecords(std::vector<TreeRecord>(records.begin() + medianIndex + 1, records.end()));

    if (parentPageNumber != NULLPTR) {
        medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());
    }

    treePageManager.WritePage(newPage);
    return medianRecord;
}

void BTree::Print() {
    std::size_t rootNumber = this->treePageManager.GetRootNumber();

    if (rootNumber == NULLPTR) {
        std::cout << "Drzewo jest puste." << std::endl;
        return;
    }

    // BFS kolejka
    std::queue<std::pair<std::size_t, int>> pageQueue; // (Numer strony, Poziom drzewa)
    pageQueue.push({ rootNumber, 0 });

    int currentLevel = 0;
    std::cout << "LEVEL " << currentLevel << ":\n";

    while (!pageQueue.empty()) {
        auto [currentPageNumber, level] = pageQueue.front();
        pageQueue.pop();

        // Jeœli poziom siê zmienia, przechodzimy do nowej linii
        if (level > currentLevel) {
            currentLevel = level;
            std::cout << "\LEVEL " << currentLevel << ":\n";
        }

        // Pobranie strony i jej danych
        TreePage currentPage = this->treePageManager.ReadPage(currentPageNumber);

        // Budowanie wêz³a ASCII
        std::ostringstream nodeBuilder;

        // Nag³ówek wêz³a
        nodeBuilder << "Node " << currentPage.GetPageNumber();

        // Dane parenta i lewego dziecka
        nodeBuilder << " |" << (currentPage.GetHeadLeftChildPageNumber() != NULLPTR ? std::to_string(currentPage.GetHeadLeftChildPageNumber()) : "*");

        // Dane rekordów
        const auto& records = currentPage.GetRecords();
        for (const auto& record : records) {
            nodeBuilder << "|" << (record.GetId() != NULLPTR ? std::to_string(record.GetId()) : "*");
            //nodeBuilder << "|" << (record.GetDiskPageNumber() != NULLPTR ? std::to_string(record.GetDiskPageNumber()) : "*");
            nodeBuilder << "|" << (record.GetTreeRightChildNumber() != NULLPTR ? std::to_string(record.GetTreeRightChildNumber()) : "*");
        }

        // Drukowanie zbudowanego wêz³a
        std::cout << nodeBuilder.str() << "|\n";

        // Dodawanie dzieci do kolejki BFS
        std::size_t leftChildPageNumber = currentPage.GetHeadLeftChildPageNumber();
        if (leftChildPageNumber != NULLPTR) {
            pageQueue.push({ leftChildPageNumber, level + 1 });
        }

        for (std::size_t i = 0; i < records.size(); ++i) {
            std::size_t rightChildPageNumber = records[i].GetTreeRightChildNumber();
            if (rightChildPageNumber != NULLPTR) {
                pageQueue.push({ rightChildPageNumber, level + 1 });
            }
        }
    }
    std::cout << "\n";
}
