#include "BTree.h"
#include <functional>

bool BTree::InsertRecord(DiskRecord diskRecord)
{
    std::size_t diskRecordId = diskRecord.GetId();

    // krok 1
    TreeRecord treeRecord = this->FindRecord(diskRecordId).first;

    if (treeRecord.GetId() != NULLPTR) {
        return false;
    }

    std::size_t currentPageNumber = this->GetLeaf(diskRecordId);

    // krok 2
    TreeRecord recordToInsert = TreeRecord(diskRecordId, this->diskPageManager.GetPagesCounter());

    bool insertToDisk = true;

    while (true) {
        TreePage* currentPage = treePageManager.ReadPageWithCache(currentPageNumber);

        // krok 3
        if (!currentPage->isOverflow()) {
            currentPage->InsertRecord(recordToInsert);
            if (insertToDisk) {
                diskPageManager.InsertRecordToBuffer(diskRecord);
            }
            break;
        }

        // krok 4
        // TODO: compensation has some exotic edge case
        if (this->TryCompensation(currentPage, recordToInsert)) {
            if (insertToDisk) {
                diskPageManager.InsertRecordToBuffer(diskRecord);
            }
            // krok 5 
            break;
        }

        // krok 6
        std::size_t rootNumberBeforeSplit = this->treePageManager.GetRootNumber();
        recordToInsert = this->SplitPage(currentPage, recordToInsert);

        if (insertToDisk) {
            diskPageManager.InsertRecordToBuffer(diskRecord);
        }
        insertToDisk = false;

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

std::pair<TreeRecord, std::size_t> BTree::FindRecord(std::size_t treeRecordId)
{
    std::size_t currentPageNumber = this->treePageManager.GetRootNumber();

    if (currentPageNumber == NULLPTR) {
        std::cout << "SEARCH R:" << this->treePageManager.GetPagesReadCounter() << " W:"
            << this->treePageManager.GetPagesWrittenCounter() << "\n";
        return { TreeRecord(), NULLPTR };
    }

    while (currentPageNumber != NULLPTR)
    {
        TreePage* currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);
        TreeRecord foundRecord = currentPage->FindRecordById(treeRecordId);

        if (foundRecord.GetId() != NULLPTR) {
            std::cout << "SEARCH R:" << this->treePageManager.GetPagesReadCounter() << " W:"
                << this->treePageManager.GetPagesWrittenCounter() << "\n";
            return { foundRecord, currentPageNumber };
        }

        std::size_t nextPageNumber = NULLPTR;
        std::size_t recordsSize = currentPage->GetRecordsSize();

        for (std::size_t i = 0; i < recordsSize; ++i)
        {
            if (treeRecordId < currentPage->GetRecordByIndex(i).GetId())
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

        if (nextPageNumber == NULLPTR)
        {
            nextPageNumber = currentPage->GetRightChildPageNumberById(recordsSize - 1);
        }

        if (nextPageNumber == NULLPTR) {
            // Rekord nie zosta³ znaleziony, a dotarliœmy do liœcia
            std::cout << "SEARCH R:" << this->treePageManager.GetPagesReadCounter() << " W:"
                << this->treePageManager.GetPagesWrittenCounter() << "\n";
            return { TreeRecord(), currentPageNumber };
        }

        currentPageNumber = nextPageNumber;
    }

    std::cout << "SEARCH R:" << this->treePageManager.GetPagesReadCounter() << " W:"
        << this->treePageManager.GetPagesWrittenCounter() << "\n";
    return { TreeRecord(), NULLPTR };
}

bool BTree::DeleteRecord(std::size_t treeRecordId)
{
    auto pair = this->FindRecord(treeRecordId);

    TreeRecord treeRecord = pair.first;
    std::size_t currentPageNumber = pair.second;

    if (treeRecord.GetId() == NULLPTR) {
        return false;
    }

    TreePage* currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);

    if (currentPage->GetHeadLeftChildPageNumber() == NULLPTR) {
        currentPage->RemoveRecordById(treeRecordId);
        this->diskPageManager.RemoveRecordById(treeRecord.GetDiskPageNumber(), treeRecordId);
    }
    else {
        std::size_t rightSubtreePageNumber = currentPage->GetRightChildPageNumberById(treeRecordId);
        auto minPair = FindMinInRightSubtree(rightSubtreePageNumber);
        TreeRecord minRecord = minPair.first;
        std::size_t minPageNumber = minPair.second;

        currentPage->ReplaceRecord(treeRecord, minRecord);

        currentPage = this->treePageManager.ReadPageWithCache(minPageNumber);
        currentPage->RemoveRecordById(minRecord.GetId());
        this->diskPageManager.RemoveRecordById(treeRecord.GetDiskPageNumber(), minRecord.GetId());
    }

    if (currentPage->GetRecordsSize() >= this->d) {
        this->treePageManager.FlushPageCache();
        return true;
    }

    while (true) {
        if (this->TryCompensation(currentPage)) {
            this->treePageManager.FlushPageCache();
            return true;
        }

        currentPageNumber = this->MergePage(currentPage);
        if (currentPageNumber == NULLPTR) {
            std::cout << "NULLPTR after MERGE!\n";
        }

        currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);
        if (currentPage->GetRecordsSize() >= this->d) {
            break;
        }
    }

    this->treePageManager.FlushPageCache();
    return true;
}

bool BTree::UpdateRecord(DiskRecord updatedRecord)
{
    std::size_t recordId = updatedRecord.GetId();
    TreeRecord treeRecord = this->FindRecord(recordId).first;

    if (treeRecord.GetId() == NULLPTR) {
        return false;
    }

    this->diskPageManager.UpdateRecordById(updatedRecord, treeRecord.GetDiskPageNumber(), recordId);
}

void BTree::PrintDiskFile()
{
    this->diskPageManager.PrintFile();
}

std::size_t BTree::GetLeaf(std::size_t treeRecordId) {
    std::size_t currentPageNumber = this->treePageManager.GetRootNumber();

    if (currentPageNumber == NULLPTR) {
        return NULLPTR;
    }

    while (currentPageNumber != NULLPTR) {
        TreePage* currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);


        std::size_t nextPageNumber = NULLPTR;
        std::size_t recordsSize = currentPage->GetRecordsSize();
        // TODO: cleanup powtarza siê kod
        for (std::size_t i = 0; i < recordsSize; ++i)
        {
            if (treeRecordId < currentPage->GetRecordByIndex(i).GetId())
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
            nextPageNumber = currentPage->GetRightChildPageNumberById(recordsSize - 1);
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
    std::size_t currentPageNumber = currentPage->GetPageNumber();
    std::size_t parentPageNumber = this->treePageManager.FindParentNumber(currentPageNumber);

    if (parentPageNumber == NULLPTR) {
        return false;
    }

    TreePage* parentPage = treePageManager.ReadPageWithCache(parentPageNumber);

    std::vector<SiblingInfo> siblings = DetermineSibling(parentPage, currentPageNumber);

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

bool BTree::TryCompensation(TreePage* currentPage)
{
    std::size_t currentPageNumber = currentPage->GetPageNumber();
    std::size_t parentPageNumber = this->treePageManager.FindParentNumber(currentPageNumber);

    if (parentPageNumber == NULLPTR) {
        return false;
    }

    TreePage* parentPage = treePageManager.ReadPageWithCache(parentPageNumber);

    std::vector<SiblingInfo> siblings = DetermineSibling(parentPage, currentPageNumber);

    if (siblings.empty()) {
        return false;
    }

    SiblingInfo sibling = SiblingInfo(TreeRecord(), RIGHT_SIBLING, NULLPTR);
    TreePage* siblingPage = nullptr;

    for (const auto& sibli : siblings) {
        siblingPage = treePageManager.ReadPageWithCache(sibli.siblingNumber);

        if (siblingPage->GetRecordsSize() > d) {
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
        TreeRecord tail = siblingPage->PopTail();
        std::size_t tailRightChildNumber = tail.GetTreeRightChildNumber();
        tail.SetTreeRightChildNumber(separatorRecord.GetTreeRightChildNumber());

        separatorRecord.SetTreeRightChildNumber(currentPage->GetHeadLeftChildPageNumber());
        currentPage->SetHeadLeftChildPageNumber(tailRightChildNumber);

        parentPage->ReplaceRecord(separatorRecord, tail);
        currentPage->InsertRecord(separatorRecord);
    }
    else {
        TreeRecord head = siblingPage->PopHead();
        std::size_t headRightChildNumber = head.GetTreeRightChildNumber();
        head.SetTreeRightChildNumber(separatorRecord.GetTreeRightChildNumber());

        separatorRecord.SetTreeRightChildNumber(siblingPage->GetHeadLeftChildPageNumber());
        siblingPage->SetHeadLeftChildPageNumber(headRightChildNumber);

        parentPage->ReplaceRecord(separatorRecord, head);
        currentPage->InsertRecord(separatorRecord);
    }

    return true;
}

std::vector<SiblingInfo> BTree::DetermineSibling(TreePage* parentPage, std::size_t currentPageNumber)
{
    std::vector<SiblingInfo> siblings;
    std::size_t recordsSize = parentPage->GetRecordsSize();

    // TODO: CLEANUP za du¿o wciêæ

    for (std::size_t i = 0; i < recordsSize; ++i) {

        std::size_t rightSiblingNumber = parentPage->GetRightChildPageNumberById(i);

        if (i == 0) {
            if (parentPage->GetHeadLeftChildPageNumber() == currentPageNumber) {
                siblings.push_back(SiblingInfo(parentPage->GetRecordByIndex(i), RIGHT_SIBLING, rightSiblingNumber));
                return siblings;
            }
            else if (rightSiblingNumber == currentPageNumber) {
                if (recordsSize > i + 1) {
                    rightSiblingNumber = parentPage->GetRightChildPageNumberById(i + 1);
                    siblings.push_back(SiblingInfo(parentPage->GetRecordByIndex(i + 1), RIGHT_SIBLING, rightSiblingNumber));
                }
                siblings.push_back(SiblingInfo(parentPage->GetRecordByIndex(i), LEFT_SIBLING, parentPage->GetHeadLeftChildPageNumber()));
                return siblings;
            }
        }
        else if (i == recordsSize - 1 && rightSiblingNumber == currentPageNumber) {
            std::size_t leftSiblingNumber;
            if (i == 0) {
                leftSiblingNumber = parentPage->GetHeadLeftChildPageNumber();
            }
            else {
                leftSiblingNumber = parentPage->GetRightChildPageNumberById(i - 1);
            }
            siblings.push_back(SiblingInfo(parentPage->GetRecordByIndex(i), LEFT_SIBLING, leftSiblingNumber));
            return siblings;
        }
        else if (rightSiblingNumber == currentPageNumber) {
            if (recordsSize > i + 1) {
                rightSiblingNumber = parentPage->GetRightChildPageNumberById(i + 1);
                siblings.push_back(SiblingInfo(parentPage->GetRecordByIndex(i + 1), RIGHT_SIBLING, rightSiblingNumber));
            }

            std::size_t leftSiblingNumber = parentPage->GetRightChildPageNumberById(i - 1);
            siblings.push_back(SiblingInfo(parentPage->GetRecordByIndex(i), LEFT_SIBLING, leftSiblingNumber));
            return siblings;
        }

    }

    return siblings;
}

TreeRecord BTree::SplitPage(TreePage* pageToSplit, TreeRecord recordToInsert)
{
    std::size_t recordsSize = pageToSplit->GetRecordsSize();
    pageToSplit->InsertRecord(recordToInsert);
    recordsSize++;

    std::size_t medianIndex = recordsSize / 2;
    TreeRecord medianRecord = pageToSplit->GetRecordByIndex(medianIndex);

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

    newPage.SetRecords(pageToSplit->GetRecordsByIndex(medianIndex, true));
    pageToSplit->SetRecords(pageToSplit->GetRecordsByIndex(medianIndex, false));

    if (parentPageNumber != NULLPTR) {
        medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());
    }

    treePageManager.WritePage(newPage);
    return medianRecord;
}

std::size_t BTree::MergePage(TreePage* pageToMerge)
{
    std::size_t currentPageNumber = pageToMerge->GetPageNumber();
    std::size_t parentPageNumber = this->treePageManager.FindParentNumber(currentPageNumber);

    if (parentPageNumber == NULLPTR) {
        // dobrze bo to jest root wiec moze miec mniej niz standardowo
        return NULLPTR;
    }

    TreePage* parentPage = treePageManager.ReadPageWithCache(parentPageNumber);

    std::vector<SiblingInfo> siblings = DetermineSibling(parentPage, currentPageNumber);

    if (siblings.empty()) {
       return NULLPTR;
    }

    SiblingInfo sibling = SiblingInfo(TreeRecord(), RIGHT_SIBLING, NULLPTR);
    TreePage* siblingPage = nullptr;

    for (const auto& sibli : siblings) {
        siblingPage = treePageManager.ReadPageWithCache(sibli.siblingNumber);

        if (siblingPage->GetRecordsSize() + pageToMerge->GetRecordsSize() < 2*d) {
            sibling = sibli;
            break;
        }
    }

    if (sibling.siblingNumber == NULLPTR) {
        return NULLPTR;
    }

    SiblingType siblingType = sibling.siblingType;
    TreeRecord separatorRecord = sibling.separatorRecord;


    if (siblingType == LEFT_SIBLING) {
        separatorRecord.SetTreeRightChildNumber(pageToMerge->GetHeadLeftChildPageNumber());
        siblingPage->InsertRecord(separatorRecord);

        for (std::size_t i = 0; i < pageToMerge->GetRecordsSize(); ++i) {
            siblingPage->InsertRecord(pageToMerge->GetRecordByIndex(i));
        }

        parentPage->RemoveRecordById(separatorRecord.GetId());
    }
    else {
        separatorRecord.SetTreeRightChildNumber(siblingPage->GetHeadLeftChildPageNumber());
        pageToMerge->InsertRecord(separatorRecord);

        for (std::size_t i = 0; i < siblingPage->GetRecordsSize(); ++i) {
            pageToMerge->InsertRecord(siblingPage->GetRecordByIndex(i));
        }

        parentPage->RemoveRecordById(separatorRecord.GetId());
    }

    return parentPageNumber;
}

std::pair<TreeRecord, std::size_t> BTree::FindMinInRightSubtree(std::size_t pageNumber)
{
    if (pageNumber == NULLPTR) {
        return { TreeRecord(), NULLPTR };
    }

    std::size_t currentPageNumber = pageNumber;
    TreePage* currentPage = nullptr;

    while (currentPageNumber != NULLPTR) {
        currentPage = this->treePageManager.ReadPageWithCache(currentPageNumber);

        if (currentPage->GetHeadLeftChildPageNumber() == NULLPTR) {
            return { currentPage->GetRecordByIndex(0), currentPageNumber };
        }

        currentPageNumber = currentPage->GetHeadLeftChildPageNumber();
    }

    return { TreeRecord(), NULLPTR };
}

void BTree::Print() {
    std::size_t rootNumber = this->treePageManager.GetRootNumber();

    if (rootNumber == NULLPTR) {
        std::cout << "Drzewo jest puste." << std::endl;
        return;
    }

    // Funkcja pomocnicza do rekursywnego drukowania drzewa
    std::function<void(std::size_t, int)> printNode = [&](std::size_t pageNumber, int level) {
        TreePage currentPage = this->treePageManager.ReadPage(pageNumber);

        // Budowanie wêz³a
        std::ostringstream nodeBuilder;
        nodeBuilder << std::string(level * 4, ' '); // Wciêcie na podstawie poziomu
        nodeBuilder << "Node " << currentPage.GetPageNumber();

        // Dane parenta i lewego dziecka
        nodeBuilder << " |" << (currentPage.GetHeadLeftChildPageNumber() != NULLPTR ? std::to_string(currentPage.GetHeadLeftChildPageNumber()) : "*");

        // Dane rekordów
        const auto& records = currentPage.GetRecords();
        for (const auto& record : records) {
            nodeBuilder << "|" << (record.GetId() != NULLPTR ? std::to_string(record.GetId()) : "*");
            nodeBuilder << "|" << (record.GetTreeRightChildNumber() != NULLPTR ? std::to_string(record.GetTreeRightChildNumber()) : "*");
        }

        std::cout << nodeBuilder.str() << "|\n";

        // Rekurencyjne drukowanie dzieci
        std::size_t leftChildPageNumber = currentPage.GetHeadLeftChildPageNumber();
        if (leftChildPageNumber != NULLPTR) {
            printNode(leftChildPageNumber, level + 1);
        }

        for (const auto& record : records) {
            std::size_t rightChildPageNumber = record.GetTreeRightChildNumber();
            if (rightChildPageNumber != NULLPTR) {
                printNode(rightChildPageNumber, level + 1);
            }
        }
        };

    // Start drukowania od korzenia
    printNode(rootNumber, 0);
}

