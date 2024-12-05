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
        currentPageNumber = currentPage->GetParentPageNumber();
    }

    // force zapis nowy
    this->treePageManager.FlushPageCache();
   
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
        TreePage* currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);
        TreeRecord foundRecord = currentPage->FindRecordById(treeRecordId);

        if (foundRecord.GetId() != NULLPTR) {
            // MO¯E PARA DO ZWRÓCENIA (x, a)
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
    std::size_t parentPageNumber = currentPage->GetParentPageNumber();

    if (parentPageNumber == NULLPTR) {
        return false;
    }

    TreePage* parentPage = treePageManager.ReadPageWithCache(parentPageNumber);

    auto data = DetermineSibling(currentPage, parentPage);

    SiblingType siblingType = data.first;
    std::size_t siblingNumber = data.second.first;
    TreeRecord separatorRecord = data.second.second;

    if (siblingType == NO_SIBLING) {
        return false;
    }

    TreePage* siblingPage = treePageManager.ReadPageWithCache(siblingNumber);

    if (siblingPage->isOverflow()) {
        return false;
    }

    if (siblingType == LEFT_SIBLING) {
        TreeRecord leafHead = currentPage->PopHead();
        leafHead.SetTreeRightChildNumber(currentPage->GetPageNumber());

        // TODO: recordToInsert wyszukanie nowych pointerów
        recordToInsert.SetTreeRightChildNumber(NULLPTR);
        currentPage->InsertRecord(recordToInsert);

        // TODO: leafTail.SetTreeRightChildNumber(separatorRecord.GetTreeRightChildNumber());
        parentPage->ReplaceRecord(separatorRecord, leafHead);

        // TODO: separatorRecord wyszukanie nowych pointerów
        separatorRecord.SetTreeRightChildNumber(NULLPTR);
        siblingPage->InsertRecord(separatorRecord);
    }
    else {
        TreeRecord leafTail = currentPage->PopTail();
        leafTail.SetTreeRightChildNumber(currentPage->GetPageNumber());

        // TODO: recordToInsert wyszukanie nowych pointerów
        recordToInsert.SetTreeRightChildNumber(NULLPTR);
        currentPage->InsertRecord(recordToInsert);

        leafTail.SetTreeRightChildNumber(separatorRecord.GetTreeRightChildNumber());
        parentPage->ReplaceRecord(separatorRecord, leafTail);

        // TODO: separatorRecord wyszukanie nowych pointerów
        separatorRecord.SetTreeRightChildNumber(NULLPTR);
        siblingPage->InsertRecord(separatorRecord);
    }

    //treePageManager.WritePage(currentPage);
   // treePageManager.WritePage(siblingPage);
   // treePageManager.WritePage(parentPage);

    return true;
}

std::pair<SiblingType, std::pair<std::size_t, TreeRecord>> BTree::DetermineSibling(TreePage* currentPage, TreePage* parentPage)
{
    std::vector<TreeRecord> parentRecords = parentPage->GetRecords();
    std::size_t currentPageNumber = currentPage->GetPageNumber();

    // TODO: Mo¿e chyba wyst¹piæ sytuacja, ¿e nie ma siblinga? Chyba po splicie lewy pointer tylko ale mo¿e byæ
    // TODO: za du¿e zag³êbienie jakiœ cleanup tu by siê przyda³
    for (std::size_t i = 0; i < parentRecords.size(); ++i) { 
        if ((i > 0 && parentPage->GetRightChildPageNumberById(i - 1) == currentPageNumber) ||
            (parentPage->GetHeadLeftChildPageNumber() == currentPageNumber))
        {
            std::size_t rightSiblingNumber = parentPage->GetRightChildPageNumberById(i);
            TreeRecord separatorRecord = parentRecords[i];
            return { RIGHT_SIBLING, { rightSiblingNumber, separatorRecord } };
        }

        if (parentPage->GetRightChildPageNumberById(i) == currentPageNumber) {
            if (i == 0) {
                std::size_t leftSiblingNumber = parentPage->GetHeadLeftChildPageNumber();
                if (leftSiblingNumber != NULLPTR) {
                    TreeRecord separatorRecord = parentRecords[i];
                    return { LEFT_SIBLING, { leftSiblingNumber, separatorRecord } };
                }
                return { NO_SIBLING, { NULLPTR, TreeRecord() } };
            }
            else {
                std::size_t leftSiblingNumber = parentPage->GetRightChildPageNumberById(i - 1);
                TreeRecord separatorRecord = parentRecords[i - 1];
                return { LEFT_SIBLING, { leftSiblingNumber, separatorRecord } };
            }
        }
    }

    return { NO_SIBLING, { NULLPTR, TreeRecord() } };
}


TreeRecord BTree::SplitPage(TreePage* pageToSplit, TreeRecord recordToInsert)
{
    // TODO: co gdy nie jest liœciem - pointery do ogarniecia ale tutaj chyba head lewy pointer to wiêkszy problem
    // UPDATE: chyba jednak jest okej?
    std::size_t recordsSize = pageToSplit->GetRecords().size();

    pageToSplit->InsertRecord(recordToInsert);

    std::vector<TreeRecord> records = pageToSplit->GetRecords();

    std::size_t medianIndex = recordsSize / 2;
    TreeRecord medianRecord = records[medianIndex];

    TreePage newPage = this->treePageManager.CreateNewPage();
    //newPage.SetHeadLeftChildPageNumber();
    std::size_t parentPageNumber = pageToSplit->GetParentPageNumber();

    if (parentPageNumber == NULLPTR) {
        TreePage newRoot = treePageManager.CreateNewPage();
        newRoot.SetHeadLeftChildPageNumber(pageToSplit->GetPageNumber());
        medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());
        newRoot.SetRecords({ medianRecord });

        std::size_t newRootPageNumber = newRoot.GetPageNumber();
        treePageManager.WritePage(newRoot);
        treePageManager.SetRootNumber(newRootPageNumber);

        pageToSplit->SetParentPageNumber(newRootPageNumber);
        newPage.SetParentPageNumber(newRootPageNumber);
    }
    else {
        newPage.SetParentPageNumber(parentPageNumber);
    }

    pageToSplit->SetRecords(std::vector<TreeRecord>(records.begin(), records.begin() + medianIndex));
    newPage.SetRecords(std::vector<TreeRecord>(records.begin() + medianIndex + 1, records.end()));

    if (parentPageNumber != NULLPTR) {
        medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());
    }

    treePageManager.WritePage(newPage);
    return medianRecord;
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
