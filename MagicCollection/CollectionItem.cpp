#include "CollectionItem.h"



CollectionItem::CollectionItem(std::string aszName)
{
	m_szName = aszName;
}


CollectionItem::~CollectionItem()
{
}

std::string CollectionItem::GetName()
{
	return m_szName;
}

CopyObject* CollectionItem::AddCopyItem(std::string aszCollectionName,
	std::vector<std::pair<std::string, std::string>> alstAttrs,
	std::vector<std::pair<std::string, std::string>> alstMetaTags)
{

}

CopyObject* CollectionItem::FindCopyItem(std::string aszHash)
{
	std::vector<CopyObject>::iterator iter_Copies = m_lstCopies.begin();

	for (; iter_Copies != m_lstCopies.end(); ++iter_Copies)
	{

	}
}