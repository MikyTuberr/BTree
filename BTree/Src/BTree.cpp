#include "BTree.h"

bool BTree::InsertRecord(DiskRecord diskRecord)
{
    std::size_t currentPageNumber = this->treePageManager.GetRoot().GetPageNumber();
    std::size_t diskRecordId = diskRecord.GetId();

    // krok 1
    std::pair<TreeRecord*, std::size_t> pair = this->FindRecord(diskRecordId);
    TreeRecord* treeRecord = pair.first;
    currentPageNumber = pair.second;

    if (treeRecord != nullptr) {
        return false;
    }

    while (true) {

        // krok 2
        TreePage treePage = treePageManager.ReadPage(currentPageNumber);
        if (!treePage.isOverflow()) {
            // TODO: treePageManager warstwa wyznaczenia prawego dziecka: treePageManager.InsertRecord(TreeRecord());
            diskPageManager.InsertRecordToBuffer(diskRecord);
        }


        // krok 3
        // TODO: compensation

        // krok 4 
        // je¿eli compensation mo¿liwe return true

        // krok 6 
        // TODO: split je¿eli overflown

        // krok 7
        // TODO: make the ancestor page the curr page go to step 3
    }
	return false;
}

std::pair<TreeRecord*, std::size_t> BTree::FindRecord(std::size_t treeRecordId)
{
    std::size_t currentPageNumber = this->treePageManager.GetRoot().GetPageNumber();

    if (currentPageNumber == UINT_MAX) {
        throw std::runtime_error("Record not found: tree is empty.");
    }

    while (currentPageNumber != UINT_MAX)
    {
        // krok 3
        TreePage currentPage = this->treePageManager.ReadPage(currentPageNumber);

        // krok 4: Szukamy rekordu na bie¿¹cej stronie
        TreeRecord* foundRecord = currentPage.FindRecordById(treeRecordId);

        if (foundRecord != nullptr) {
            // krok 5: Jeœli rekord zosta³ znaleziony, zwracamy wskaŸnik do rekordu 
            // MO¯E PARA DO ZWRÓCENIA (x, a)
            // MO¯E ZWRÓÆ TE¯ LICZBE STRONY
            return std::make_pair(foundRecord, currentPageNumber);
        }

        // Rekord nie znaleziony na bie¿¹cej stronie, wiêc musimy okreœliæ dalszy kierunek
        std::vector<TreeRecord> records = currentPage.GetRecords();

        std::size_t nextPageNumber = UINT_MAX;
        for (std::size_t i = 0; i < records.size(); ++i)
        {
            if (treeRecordId < records[i].GetId())
            {
                nextPageNumber = currentPage.GetRightChildPageNumberById(i);  // Pobierz wskaŸnik do strony dziecka (pi)
                break;
            }
        }

        // Jeœli nie znaleziono mniejszego klucza, przechodzimy do ostatniej strony dziecka
        if (nextPageNumber == UINT_MAX)
        {
            nextPageNumber = currentPage.GetRightChildPageNumberById(records.size());  // pm - ostatnie dziecko
        }

        currentPageNumber = nextPageNumber;
    }

    return std::make_pair(nullptr, UINT_MAX);
}

void BTree::Print()
{
}
