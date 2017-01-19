
#include "CollectionSource.h"
CollectionSource::CollectionSource()
{
}

CollectionSource::~CollectionSource()
{
}

void CollectionSource::LoadLib(std::string aszFileName)
{
   rapidxml::xml_document<> doc;
   std::ifstream file(aszFileName);
   std::stringstream buffer;
   buffer << file.rdbuf();
   file.close();

   std::cout << "Parsing Doc" << std::endl;
   std::string content(buffer.str());
   doc.parse<0>(&content[0]);
   std::cout << "Parse Done" << std::endl;

   // Mechanisms
   clock_t begin = clock();

   rapidxml::xml_node<> *xmlNode_CardDatabase = doc.first_node();
   // With the xml example above this is the <document/> node
   rapidxml::xml_node<> *xmlNode_Cards = xmlNode_CardDatabase->first_node("cards");
   rapidxml::xml_node<> *xmlNode_Card = xmlNode_Cards->first_node();

   while (xmlNode_Card != 0)
   {
      rapidxml::xml_node<> *xmlNode_CardAttribute = xmlNode_Card->first_node();

      rapidxml::xml_node<> *xmlNode_CardName = xmlNode_Card->first_node("name");

      SourceObject* sO = new SourceObject(xmlNode_CardName->value());

      while (xmlNode_CardAttribute != 0)
      {
         std::string szCardKey = xmlNode_CardAttribute->name();
         
         if (szCardKey != "name")
         {
            sO->AddAttribute(szCardKey, xmlNode_CardAttribute->value());
         }
         
         xmlNode_CardAttribute = xmlNode_CardAttribute->next_sibling();
      }

      xmlNode_Card = xmlNode_Card->next_sibling();
      m_lstptCardBuffer.push_back(*sO);
   }

   // Mechanisms
   clock_t end = clock();
   double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
   std::cout << "Load Time: " << elapsed_secs << std::endl;

   std::cout << "Load Done" << std::endl;
}

int CollectionSource::LoadCard(std::string aszCardName)
{
   int iCacheLocation = -1;
   int iFound = findInBuffer(aszCardName);
   if (iFound != -1)
   {
      SourceObject* oSource = &m_lstptCardBuffer.at(iFound);

      iCacheLocation = oSource->GetCacheIndex();
      if (iCacheLocation == -1)
      {
         // The card is already cached.
         CollectionObject oCard(aszCardName);

         std::map<std::string, std::string>::iterator att_iter = oSource->Attributes.begin();
         bool bHasAllAttributes = false;
         for (; att_iter != oSource->Attributes.end() && !bHasAllAttributes; ++att_iter)
         {
            bHasAllAttributes = oCard.MapAttributes(att_iter->first, att_iter->second);
         }

         // Store the location of the CollectionObject in the cache
         iCacheLocation = m_lstoCardCache.size();
         oSource->Cache(iCacheLocation);

         // Cache the collectionobject
         m_lstoCardCache.push_back(oCard);
      }
   }

   return iCacheLocation;
}

CollectionObject* CollectionSource::GetCardPrototype(int aiCacheIndex)
{
   if (aiCacheIndex < m_lstoCardCache.size())
   {
      return &m_lstoCardCache.at(aiCacheIndex);
   }
   else
   {
      return NULL;
   }
}

int CollectionSource::findInBuffer(std::string aszCardName)
{
   int iSize = m_lstptCardBuffer.size();
   int iLeft = 0;
   int iRight = iSize;
   if (iRight < 1)
   {
      return -1;
   }

   std::string szName;
   while (iLeft <= iRight)
   {
      int middle = (iLeft + iRight) / 2;

      if (middle < 0 || middle >= iSize)
      {
         return -1;
      }

      szName = m_lstptCardBuffer.at(middle).GetName();
      if (szName == aszCardName)
         return middle;
      else if (aszCardName.compare(szName) < 0)
         iRight = middle - 1;
      else
         iLeft = middle + 1;
   }

   return -1;
}
