#include "BTree.h"

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
        TreePage currentPage = treePageManager.ReadPage(currentPageNumber);

        // krok 3
        if (!currentPage.isOverflow()) {
            treePageManager.InsertRecord(recordToInsert, currentPage);
            diskPageManager.InsertRecordToBuffer(diskRecord);
            return true;
        }

        // krok 4
        if (this->TryCompensation(currentPage, recordToInsert)) {
            diskPageManager.InsertRecordToBuffer(diskRecord);
            // krok 5 
            return true;
        }

        // krok 6
        recordToInsert = this->SplitPage(currentPage, recordToInsert);
        diskPageManager.InsertRecordToBuffer(diskRecord);

        if (currentPageNumber == this->treePageManager.GetRootNumber()) {
            TreePage newRoot = treePageManager.CreateNewPage();

            newRoot.SetHeadLeftChildPageNumber(currentPage.GetPageNumber());

            newRoot.SetRecords({ recordToInsert });

            treePageManager.WritePage(newRoot);
            treePageManager.SetRootNumber(newRoot.GetPageNumber());
            return true;
        }

        // krok 7
        currentPageNumber = currentPage.GetParentPageNumber();
    }
   
	return true;
}

TreeRecord BTree::FindRecord(std::size_t treeRecordId)
{
    std::size_t currentPageNumber = this->treePageManager.GetRootNumber();

    if (currentPageNumber == NULLPTR) {
        return TreeRecord();
    }

    while (currentPageNumber != NULLPTR)
    {
        // krok 3
        TreePage currentPage = this->treePageManager.ReadPage(currentPageNumber);

        // krok 4: Szukamy rekordu na bie¿¹cej stronie
        TreeRecord foundRecord = currentPage.FindRecordById(treeRecordId);

        if (foundRecord.GetId() != NULLPTR) {
            // krok 5: Jeœli rekord zosta³ znaleziony, zwracamy wskaŸnik do rekordu 
            // MO¯E PARA DO ZWRÓCENIA (x, a)
            return foundRecord;
        }

        // Rekord nie znaleziony na bie¿¹cej stronie, wiêc musimy okreœliæ dalszy kierunek
        std::vector<TreeRecord> records = currentPage.GetRecords();

        std::size_t nextPageNumber = NULLPTR;
        for (std::size_t i = 0; i < records.size(); ++i)
        {
            if (treeRecordId < records[i].GetId())
            {
                nextPageNumber = currentPage.GetRightChildPageNumberById(i);  // Pobierz wskaŸnik do strony dziecka (pi)
                break;
            }
        }

        // Jeœli nie znaleziono mniejszego klucza, przechodzimy do ostatniej strony dziecka
        if (nextPageNumber == NULLPTR)
        {
            nextPageNumber = currentPage.GetRightChildPageNumberById(records.size() - 1);  // pm - ostatnie dziecko
        }

        currentPageNumber = nextPageNumber;
    }

    return TreeRecord();
}

std::size_t BTree::GetLeaf(std::size_t treeRecordId) {
    std::size_t currentPageNumber = this->treePageManager.GetRootNumber();

    if (currentPageNumber == NULLPTR) {
        return NULLPTR;
    }

    while (currentPageNumber != NULLPTR) {
        TreePage currentPage = this->treePageManager.ReadPage(currentPageNumber);

        std::vector<TreeRecord> records = currentPage.GetRecords();

        std::size_t nextPageNumber = NULLPTR;
        for (std::size_t i = 0; i < records.size(); ++i) {
            if (treeRecordId < records[i].GetId()) {
                nextPageNumber = currentPage.GetRightChildPageNumberById(i);
                break;
            }
        }

        if (nextPageNumber == NULLPTR) {
            nextPageNumber = currentPage.GetRightChildPageNumberById(records.size() - 1);
        }

        if (nextPageNumber == NULLPTR) {
            return currentPageNumber;
        }

        currentPageNumber = nextPageNumber;
    }

    return NULLPTR;
}

bool BTree::TryCompensation(TreePage leafPage, TreeRecord recordToInsert)
{
    std::size_t parentPageNumber = leafPage.GetParentPageNumber();

    if (parentPageNumber == NULLPTR) {
        return false;
    }

    TreePage parentPage = treePageManager.ReadPage(parentPageNumber);

    std::pair<std::size_t, TreeRecord> pair = parentPage.FindLeftSiblingNumberById(recordToInsert.GetId());

    std::size_t siblingNumber = pair.first;
    TreeRecord separatorRecord = pair.second;
    bool isLeftSibling = true;

    if (siblingNumber == NULLPTR) {
        pair = parentPage.FindRightSiblingNumberById(recordToInsert.GetId());
        siblingNumber = pair.first;
        separatorRecord = pair.second;
        bool isLeftSibling = false;
    }

    if (siblingNumber == NULLPTR) {
        return false;
    }

    TreePage siblingPage = treePageManager.ReadPage(siblingNumber);

    if (siblingPage.isOverflow()) {
        return false;
    }

    if (isLeftSibling) {
        TreeRecord leafHead = leafPage.PopHead();
        leafHead.SetTreeRightChildNumber(leafPage.GetPageNumber());

        // TODO: recordToInsert wyszukanie nowych pointerów
        recordToInsert.SetTreeRightChildNumber(NULLPTR);
        leafPage.InsertRecord(recordToInsert);

        parentPage.ReplaceRecord(separatorRecord, leafHead);

        // TODO: separatorRecord wyszukanie nowych pointerów
        separatorRecord.SetTreeRightChildNumber(NULLPTR);
        siblingPage.InsertRecord(separatorRecord);
    }
    else {
        TreeRecord leafTail = leafPage.PopTail();
        leafTail.SetTreeRightChildNumber(leafPage.GetPageNumber());

        // TODO: recordToInsert wyszukanie nowych pointerów
        recordToInsert.SetTreeRightChildNumber(NULLPTR);
        leafPage.InsertRecord(recordToInsert);

        parentPage.ReplaceRecord(separatorRecord, leafTail);

        // TODO: separatorRecord wyszukanie nowych pointerów
        separatorRecord.SetTreeRightChildNumber(NULLPTR);
        siblingPage.InsertRecord(separatorRecord);
    }

    treePageManager.WritePage(leafPage);
    treePageManager.WritePage(siblingPage);
    treePageManager.WritePage(parentPage);

    return true;
}

TreeRecord BTree::SplitPage(TreePage pageToSplit, TreeRecord recordToInsert)
{
    // TODO: co gdy nie jest liœciem - pointery do ogarniecia ale tutaj chyba head lewy pointer to wiêkszy problem
    std::vector<TreeRecord> records = pageToSplit.GetRecords();

    std::size_t medianIndex = records.size() / 2;
    TreeRecord medianRecord = records[medianIndex];

    TreePage newPage = this->treePageManager.CreateNewPage();
    //newPage.SetHeadLeftChildPageNumber();
    std::size_t parentPageNumber = pageToSplit.GetParentPageNumber();
    if (parentPageNumber == NULLPTR) {
        parentPageNumber = pageToSplit.GetPageNumber();
        pageToSplit.SetParentPageNumber(parentPageNumber);
        newPage.SetParentPageNumber(parentPageNumber);
    }
    else {
        newPage.SetParentPageNumber(parentPageNumber);
    }
    std::vector<TreeRecord> leftRecords(records.begin(), records.begin() + medianIndex);
    std::vector<TreeRecord> rightRecords(records.begin() + medianIndex + 1, records.end());

    if (recordToInsert.GetId() < medianRecord.GetId()) {
        leftRecords.push_back(recordToInsert);
    }
    else {
        rightRecords.push_back(recordToInsert);
    }

    medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());

    //pageToSplit.SetParentPageNumber()

    pageToSplit.SetRecords(leftRecords);
    newPage.SetRecords(rightRecords);

    treePageManager.WritePage(pageToSplit);
    treePageManager.WritePage(newPage);

    return medianRecord;
}

std::size_t BTree::GetRootNumber()
{
    return this->treePageManager.GetRootNumber();
}


void BTree::Print()
{
    std::size_t rootNumber = this->treePageManager.GetRootNumber();

    if (rootNumber == NULLPTR) {
        std::cout << "Drzewo jest puste." << std::endl;
        return;
    }

    std::queue<std::size_t> pageQueue;
    pageQueue.push(rootNumber);

    while (!pageQueue.empty()) {
        std::size_t currentPageNumber = pageQueue.front();
        pageQueue.pop();

        TreePage currentPage = this->treePageManager.ReadPage(currentPageNumber);

        std::cout << "Page " << currentPage.GetPageNumber() << ":\n";
        currentPage.Print();

        std::vector<TreeRecord> records = currentPage.GetRecords();
        if (!records.empty()) {
            std::size_t leftChildPageNumber = currentPage.GetHeadLeftChildPageNumber();
            if (leftChildPageNumber != NULLPTR) {
                pageQueue.push(leftChildPageNumber);
            }

            for (std::size_t i = 0; i < records.size(); ++i) {
                std::size_t rightChildPageNumber = currentPage.GetRightChildPageNumberById(i);
                if (rightChildPageNumber != NULLPTR) {
                    pageQueue.push(rightChildPageNumber);
                }
            }
        }
    }
}
