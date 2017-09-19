#include "TraitItem.h"

TraitItem::TraitItem( const std::string& aszKeyname,
                      const std::vector<std::string>& alstKeyVals, 
                      const std::vector<Tag>& alstPairedTraits )
{
   Config* config = Config::Instance();
   m_szKeyName = aszKeyname;
   m_lstPossibleValues = alstKeyVals;

   if (m_lstPossibleValues.size() == 0)
   {
      m_lstPossibleValues.push_back("");
   }

   std::string szKeyCode = config->GetKeyCode(aszKeyname);
   std::vector<Tag>::const_iterator iter_Tags = alstPairedTraits.cbegin();
   for (; iter_Tags != alstPairedTraits.cend(); ++iter_Tags)
   {
      if (szKeyCode == iter_Tags->first)
      {
         m_lstPairedValues.push_back(config->GetFullKey(iter_Tags->first));
      }
      else if (szKeyCode == iter_Tags->second)
      {
         m_lstPairedValues.push_back(config->GetFullKey(iter_Tags->second));
      }
   }
}

TraitItem::~TraitItem()
{
}

std::string TraitItem::GetKeyName() const
{
   return m_szKeyName;
}

std::string TraitItem::GetDefaultValue() const
{
   return m_lstPossibleValues[0];
}

std::vector<std::string> TraitItem::GetAllowedValues()
{
   return m_lstPossibleValues;
}

bool TraitItem::IsAllowedValue(std::string aszTestVal)
{
   return ListHelper::List_Find(aszTestVal, m_lstPossibleValues) != -1;
}