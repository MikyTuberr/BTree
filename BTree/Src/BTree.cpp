#include "BTree.h"

bool BTree::InsertRecord(DiskRecord diskRecord)
{
    std::size_t diskRecordId = diskRecord.GetId();

    // krok 1
    TreeRecord treeRecord = this->FindRecord(diskRecordId);

    if (treeRecord.GetId() != NULLPTR) {
        return false;
    }

    // TODO: WA¯NE!!!!!!!!!!!!!!!!!!! zrób vectora w którym przechowujesz œcie¿kê od roota do liœcia
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
        std::size_t rootNumberBeforeSplit = this->treePageManager.GetRootNumber();
        recordToInsert = this->SplitPage(currentPage, recordToInsert);
        diskPageManager.InsertRecordToBuffer(diskRecord);

        if (currentPageNumber == rootNumberBeforeSplit) {
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

bool BTree::TryCompensation(TreePage cureentPage, TreeRecord recordToInsert)
{
    std::size_t parentPageNumber = cureentPage.GetParentPageNumber();

    if (parentPageNumber == NULLPTR) {
        return false;
    }

    TreePage parentPage = treePageManager.ReadPage(parentPageNumber);

    auto data = DetermineSibling(cureentPage, parentPage);

    SiblingType siblingType = data.first;
    std::size_t siblingNumber = data.second.first;
    TreeRecord separatorRecord = data.second.second;

    if (siblingType == NO_SIBLING) {
        return false;
    }

    TreePage siblingPage = treePageManager.ReadPage(siblingNumber);

    if (siblingPage.isOverflow()) {
        return false;
    }

    if (siblingType == LEFT_SIBLING) {
        TreeRecord leafHead = cureentPage.PopHead();
        leafHead.SetTreeRightChildNumber(cureentPage.GetPageNumber());

        // TODO: recordToInsert wyszukanie nowych pointerów
        recordToInsert.SetTreeRightChildNumber(NULLPTR);
        cureentPage.InsertRecord(recordToInsert);

        parentPage.ReplaceRecord(separatorRecord, leafHead);

        // TODO: separatorRecord wyszukanie nowych pointerów
        separatorRecord.SetTreeRightChildNumber(NULLPTR);
        siblingPage.InsertRecord(separatorRecord);
    }
    else {
        TreeRecord leafTail = cureentPage.PopTail();
        leafTail.SetTreeRightChildNumber(cureentPage.GetPageNumber());

        // TODO: recordToInsert wyszukanie nowych pointerów
        recordToInsert.SetTreeRightChildNumber(NULLPTR);
        cureentPage.InsertRecord(recordToInsert);

        parentPage.ReplaceRecord(separatorRecord, leafTail);

        // TODO: separatorRecord wyszukanie nowych pointerów
        separatorRecord.SetTreeRightChildNumber(NULLPTR);
        siblingPage.InsertRecord(separatorRecord);
    }

    treePageManager.WritePage(cureentPage);
    treePageManager.WritePage(siblingPage);
    treePageManager.WritePage(parentPage);

    return true;
}

std::pair<SiblingType, std::pair<std::size_t, TreeRecord>> BTree::DetermineSibling(TreePage currentPage, TreePage parentPage)
{
    std::vector<TreeRecord> parentRecords = parentPage.GetRecords();
    std::size_t currentPageNumber = currentPage.GetPageNumber();

    for (std::size_t i = 0; i < parentRecords.size(); ++i) { 
        if ((i > 0 && parentPage.GetRightChildPageNumberById(i - 1) == currentPageNumber) ||
            (parentPage.GetHeadLeftChildPageNumber() == currentPageNumber)) 
        {
            std::size_t rightSiblingNumber = parentPage.GetRightChildPageNumberById(i);
            TreeRecord separatorRecord = parentRecords[i];
            return { RIGHT_SIBLING, { rightSiblingNumber, separatorRecord } };
        }

        if (parentPage.GetRightChildPageNumberById(i) == currentPageNumber) {
            if (i == 0) {
                std::size_t leftSiblingNumber = parentPage.GetHeadLeftChildPageNumber();
                TreeRecord separatorRecord = parentRecords[i];
                return { LEFT_SIBLING, { leftSiblingNumber, separatorRecord } };
            }
            else {
                std::size_t leftSiblingNumber = parentPage.GetRightChildPageNumberById(i - 1);
                TreeRecord separatorRecord = parentRecords[i - 1];
                return { LEFT_SIBLING, { leftSiblingNumber, separatorRecord } };
            }
        }
    }

    return { NO_SIBLING, { NULLPTR, TreeRecord() } };
}


TreeRecord BTree::SplitPage(TreePage pageToSplit, TreeRecord recordToInsert)
{
    // TODO: co gdy nie jest liœciem - pointery do ogarniecia ale tutaj chyba head lewy pointer to wiêkszy problem
    std::vector<TreeRecord> records = pageToSplit.GetRecords();

    // TODO: znajdŸ index który jest potrzebny chodzi o sytuacje gdy masz np: 1234 bierzesz rekord 2 splitujesz 1 34, wstawiasz 5 do 345 i masz 1 345
    std::size_t medianIndex = records.size() / 2;
    TreeRecord medianRecord = records[medianIndex];

    TreePage newPage = this->treePageManager.CreateNewPage();
    //newPage.SetHeadLeftChildPageNumber();
    std::size_t parentPageNumber = pageToSplit.GetParentPageNumber();
    if (parentPageNumber == NULLPTR) {

        TreePage newRoot = treePageManager.CreateNewPage();

        newRoot.SetHeadLeftChildPageNumber(pageToSplit.GetPageNumber());

        medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());
        newRoot.SetRecords({ medianRecord });

        std::size_t newRootPageNumber = newRoot.GetPageNumber();

        treePageManager.WritePage(newRoot);
        treePageManager.SetRootNumber(newRootPageNumber);
        pageToSplit.SetParentPageNumber(newRootPageNumber);
        newPage.SetParentPageNumber(newRootPageNumber);
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

    if (parentPageNumber != NULLPTR) {
        medianRecord.SetTreeRightChildNumber(newPage.GetPageNumber());
    }

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
