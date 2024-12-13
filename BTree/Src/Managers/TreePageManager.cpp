#include "TreePageManager.h"

TreePage* TreePageManager::ReadPageWithCache(std::size_t pageNumber)
{
    auto it = this->pageCache.find(pageNumber);
    if (it != this->pageCache.end()) {
        return &it->second;
    }

    TreePage page = this->ReadPage(pageNumber, true);

    this->pageCache[pageNumber] = page;
    this->pageCacheOrder.push_back(page.GetPageNumber());

    return &this->pageCache[pageNumber];
}

TreePage TreePageManager::ReadPage(std::size_t pageNumber, bool incrementReadCounter)
{
    std::streampos cursor = this->CalculateCursor(pageNumber);

    std::pair<std::vector<TreeRecord>, std::vector<std::size_t>> pair =
        this->randomAccessFile.ReadRecords<TreeRecord, std::size_t>(
            cursor, std::ios_base::beg, this->pageRecordsNumber, this->paramsNumber);

    std::vector<TreeRecord> records = pair.first;
    std::vector<std::size_t> params = pair.second;

    std::vector<TreeRecord> filteredRecords;

    for (const auto& record : records) {
        if (record.GetId() != NULLPTR) {
            filteredRecords.push_back(record);
        }
    }

    if (params.size() == 0) {
        params.push_back(NULLPTR);
    }

    TreePage page = TreePage(this->pageSize, this->pageRecordsNumber, pageNumber, params[0], filteredRecords);

    if (incrementReadCounter) {
        this->pagesReadCounter++;
    }

    return page;
}

bool TreePageManager::WritePage(const TreePage& treePage)
{
    std::streampos cursor = this->CalculateCursor(treePage.GetPageNumber());
    std::vector<TreeRecord> fixedRecords = treePage.GetFixedRecords();
    std::vector<std::size_t> params = { treePage.GetHeadLeftChildPageNumber() };
    if (this->randomAccessFile.WriteRecords(fixedRecords, cursor, std::ios_base::beg, params)) {
        this->pagesWrittenCounter++;
        return true;
    }
    return false;
}

bool TreePageManager::FlushPageCache()
{
    for (auto& [pageNumber, page] : pageCache) {
        if (!this->WritePage(page)) {
            return false;
        }
    }
    pageCache.clear();
    pageCacheOrder.clear();
    return true;
}

TreePage TreePageManager::CreateNewPage()
{
    this->pagesCounter++;
    return TreePage(this->pageSize, this->pageRecordsNumber, this->pagesCounter);
}

std::size_t TreePageManager::FindParentNumber(std::size_t childNumber)
{
    for (std::size_t i = 0; i < this->pageCacheOrder.size(); i++) {
        if (this->pageCacheOrder[i] == childNumber && i > 0) {
            return this->pageCacheOrder[i - 1];
        }
    }
    return NULLPTR;
}

const std::size_t TreePageManager::GetRootNumber() const
{
    return this->rootNumber;
}

const std::size_t TreePageManager::GetPagesReadCounter() const
{
    return this->pagesReadCounter;
}

const std::size_t TreePageManager::GetPagesWrittenCounter() const
{
    return this->pagesWrittenCounter;
}

const void TreePageManager::SetRootNumber(std::size_t rootNumber)
{
    this->rootNumber = rootNumber;
}

const std::streampos TreePageManager::CalculateCursor(const std::size_t& pageNumber) const
{
	return pageNumber * (this->pageSize + TREE_PAGE_PARAMS_SIZE) + TREE_FILE_OFFSET;
}
