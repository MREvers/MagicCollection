
#include "CollectionSource.h"

CardAttributes::CardAttributes()
{
}

CardAttributes::~CardAttributes()
{
}

CollectionSource::CollectionSource()
{
}


CollectionSource::~CollectionSource()
{
}

bool CollectionSource::GetCard(std::string aszName, CollectionObject& roCard)
{
   bool bRetVal = false;

   // Search through all cards to find a match
   std::vector<SourceObject*>::iterator iter = m_lstCardBuffer.begin();
   for (; iter != m_lstCardBuffer.end(); ++iter)
   {
      std::string szName = (*iter)->GetName();
      if (szName == aszName)
      {
         bRetVal = true;
         // Check if this object is already cached
         if (!(*iter)->GetCollectionObject(roCard))
         {
            CollectionObject oRetVal(szName);

            // Iterate over all the attributes stored in the source object.
            std::map<std::string, std::string>::iterator att_iter = (*iter)->Attributes.begin();
            bool bHasAllAttributes = false;
            for (; att_iter != (*iter)->Attributes.end() && !bHasAllAttributes; ++att_iter)
            {
               bHasAllAttributes = oRetVal.MapAttributes(att_iter->first, att_iter->second);
            }

            // Return the CollectionObject
            roCard = oRetVal;
            (*iter)->Cache(&roCard);
         }
         
      }
   }

   return bRetVal;
}

void CollectionSource::PrintAllWith(std::string aszMatch)
{
   std::vector<SourceObject*>::iterator iter = m_lstCardBuffer.begin();
   for (; iter != m_lstCardBuffer.end(); ++iter)
   {
      std::string szName = (*iter)->GetName();
      if (szName.find(aszMatch) != std::string::npos)
      {
         std::cout << szName << std::endl;
      }
   }
}

void CollectionSource::PrintAllWith(std::string aszMatch, bool caseSensitive)
{
   std::vector<SourceObject*>::iterator iter = m_lstCardBuffer.begin();
   for (; iter != m_lstCardBuffer.end(); ++iter)
   {
      std::string szName = (*iter)->GetName();
      std::transform(szName.begin(), szName.end(), szName.begin(), ::toupper);
      std::string newMatch = aszMatch;
      std::transform(newMatch.begin(), newMatch.end(), newMatch.begin(), ::toupper);
      if (szName.find(newMatch) != std::string::npos)
      {
         std::cout << (*iter)->GetName() << std::endl;
      }
   }
}

int CollectionSource::load_Contains(std::string aszName)
{
   int iLeft = 0;
   int iRight = m_lstCardBuffer.size();
   if (iRight < 1)
   {
      return -1;
   }

   while (iLeft <= iRight)
   {
      int middle = (iLeft + iRight) / 2;

      if (middle < 0 || middle >= m_lstCardBuffer.size())
      {
         return -1;
      }

      if (m_lstCardBuffer[middle]->GetName() == aszName)
         return middle;
      else if (aszName.compare(m_lstCardBuffer[middle]->GetName()) < 0)
         iRight = middle - 1;
      else
         iLeft = middle + 1;
   }

   return -1;
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
            //std::cout << xmlNode_CardAttribute->value() << std::endl;
            sO->AddAttribute(szCardKey, xmlNode_CardAttribute->value());
         }
         
         xmlNode_CardAttribute = xmlNode_CardAttribute->next_sibling();

      }

      xmlNode_Card = xmlNode_Card->next_sibling();
      load_AddSorted(sO);
   }

   // Mechanisms
   clock_t end = clock();
   double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
   std::cout << "Load Time: " << elapsed_secs << std::endl;

   std::cout << "Load Done" << std::endl;
}

void CollectionSource::load_AddSorted(SourceObject* oSrcObj)
{
   m_lstCardBuffer.push_back(oSrcObj);
   /*
      int iLastSpot = 0;
      while (iLastSpot < lstCardBuffer.size())
      {
         int iComparisonResult = lstCardBuffer[iLastSpot]->GetName().compare(oSrcObj->GetName());
         if (iComparisonResult < 0)
         {
            iLastSpot++;
         }
         else
         {
            break;
         }
      }
      std::vector<SourceObject*>::iterator iter = lstCardBuffer.begin() + iLastSpot;
      lstCardBuffer.insert(iter, oSrcObj);
      */
}


std::string CollectionSource::ImportCollectionObject(std::string aszNewItem)
{




   std::ifstream i("JSONTest.txt");


   nlohmann::json j;
   i >> j;
   nlohmann::json cardsJ = j;
   for (nlohmann::json::iterator it = cardsJ.begin(); it != cardsJ.end(); ++it)
   {
      nlohmann::json js = it.value().at("cards");
      std::cout << it.key() << " : " << it.value() << "\n";
      for (nlohmann::json::iterator iter = js.begin(); iter != js.end(); ++iter)
      {
         std::cout << iter.value().find("name").value() << "\n";
      }

   }
   return "hi";
}

// ConvertJSONCollection
void CollectionSource::ConvertJSONCollection(std::string aszFileName)
{
   // Load the JSON object. This may take a while.
   std::cout << "Load the JSON object. This may take a while." << std::endl;

   std::ifstream i(aszFileName);
   nlohmann::json j;
   i >> j;

   std::cout << "JSON Loaded Into Memory: " << sizeof(j) << std::endl;

   // Allocate on Heap to avoid stack overflow.
   rapidxml::xml_document<>* xmlCardDoc = new rapidxml::xml_document<>;

   // Allocate on Heap to avoid stack overflow.
   std::vector<std::pair<std::string, CardAttributes*>>* lstCardNameNode
      = new std::vector<std::pair<std::string, CardAttributes*>>();

   // Since rapid xml takes char* pointers, we don't want those strings deleted (thereby created hanging pointers)
   //  before the strings are saved off. We also don't want the strings moved for the same reason.
   //  Thus, resize the vector now so it is not moved later.
   lstCardNameNode->reserve(17000); // 102,000 -- 17,000 cards * 9 fields each

   // Add the xml declaration.
   rapidxml::xml_node<>* decl = xmlCardDoc->allocate_node(rapidxml::node_declaration);
   decl->append_attribute(xmlCardDoc->allocate_attribute("version", "1.0"));
   decl->append_attribute(xmlCardDoc->allocate_attribute("encoding", "utf-8"));
   xmlCardDoc->append_node(decl);

   // Add the outermose element.
   rapidxml::xml_node<>* xmlNode_root = xmlCardDoc->allocate_node(rapidxml::node_element, "Card_Database");
   xmlCardDoc->append_node(xmlNode_root);

   // Create the 'Cards' element.
   rapidxml::xml_node<>* xmlNode_Cards = xmlCardDoc->allocate_node(rapidxml::node_element, "cards");
   xmlNode_root->append_node(xmlNode_Cards);

   // Begin JSON parsing.
   std::cout << "Begin JSON Parsing." << std::endl;

   // j is a complete json object.
   for (nlohmann::json::iterator iter_set = j.begin(); iter_set != j.end(); ++iter_set)
   {
      std::string szSetName = iter_set.key();
      std::cout << "Set: " << iter_set.key() << " Cards: " << lstCardNameNode->size() << std::endl;
      nlohmann::json jsonListCards = iter_set.value().at("cards");

      // jsonCardsList is a json-list of json objects... same as JSON, but no 'key'
      for (nlohmann::json::iterator iter_cards = jsonListCards.begin(); iter_cards != jsonListCards.end(); ++iter_cards)
      {
         // Get the first card json object.
         nlohmann::json jsonCard = iter_cards.value();
         std::stringstream ssCardName;
         ssCardName << jsonCard.at("name").begin().value();
         std::string szCardName = ssCardName.str();

         int iFound = import_Contains(*lstCardNameNode, str_trim(szCardName, '\"'));
         if (iFound == -1)
         {
            // Create the card node.
            rapidxml::xml_node<>* xmlNode_Card = xmlCardDoc->allocate_node(rapidxml::node_element, "card");

            // Dont add the node now, these get sorted first.
            //xmlNode_Cards->append_node(xmlNode_Card);

            // Create the data structure.
            CardAttributes* oCA = new CardAttributes();

            // Make the pair.
            std::pair<std::string, CardAttributes*>* pair_Name_CA =
               new std::pair<std::string, CardAttributes*>(str_trim(szCardName, '\"'), oCA);

            oCA->XMLNode = xmlNode_Card;

            // Add the set..
            oCA->Keys[0] = str_trim("set", '\"');
            oCA->Vals[0] = str_trim(szSetName, '\"');

            rapidxml::xml_node<>* xmlNode_CardAttribute = xmlCardDoc->allocate_node(rapidxml::node_element,
               oCA->Keys[0].c_str(), oCA->Vals[0].c_str());
            xmlNode_Card->append_node(xmlNode_CardAttribute);

            oCA->XMLChildNodes[0] = xmlNode_CardAttribute;

            // Add the other attributes.
            int iNumKeyValPair = 1;
            for (nlohmann::json::iterator iter_card = jsonCard.begin(); iter_card != jsonCard.end(); ++iter_card)
            {
               std::string szKey = iter_card.key();

               std::stringstream ss;
               ss << iter_card.value();
               std::string szValue = ss.str();

               oCA->Keys[iNumKeyValPair] = str_clean(szKey);
               oCA->Vals[iNumKeyValPair] = str_clean(szValue);

               xmlNode_CardAttribute = xmlCardDoc->allocate_node(rapidxml::node_element,
                  oCA->Keys[iNumKeyValPair].c_str(), oCA->Vals[iNumKeyValPair].c_str());
               xmlNode_Card->append_node(xmlNode_CardAttribute);

               oCA->XMLChildNodes[iNumKeyValPair] = xmlNode_CardAttribute;

               iNumKeyValPair++;

            } // End Card Attributes iterator

            // The pair is built up now add it.
            import_AddSorted(*lstCardNameNode, pair_Name_CA);
            delete pair_Name_CA;
         } // End card not yet added
         else
         {
            //std::cout << "Duplicate found: " << szCardName << std::endl;
            int iSetKeyIndex = 0;
            std::string szCurrentKey = lstCardNameNode->at(iFound).second->Keys[iSetKeyIndex];
            while (iSetKeyIndex < 31 && szCurrentKey != "set")
            {
               iSetKeyIndex++;
               szCurrentKey = lstCardNameNode->at(iFound).second->Keys[iSetKeyIndex];
            }

            if (szCurrentKey == "set")
            {
               // Update the string buffer
               lstCardNameNode->at(iFound).second->Vals[iSetKeyIndex] = lstCardNameNode->at(iFound).second->Vals[iSetKeyIndex] + ", " + szSetName;

               // Update the node
               rapidxml::xml_node<>* xmlNode_RemoveNode = lstCardNameNode->at(iFound).second->XMLChildNodes[iSetKeyIndex];
               rapidxml::xml_node<>* xmlNode_NewNode = xmlCardDoc->allocate_node(rapidxml::node_element,
                  lstCardNameNode->at(iFound).second->Keys[iSetKeyIndex].c_str(), lstCardNameNode->at(iFound).second->Vals[iSetKeyIndex].c_str());

               // Swap out the old node for the new.
               lstCardNameNode->at(iFound).second->XMLNode->remove_node(xmlNode_RemoveNode);
               lstCardNameNode->at(iFound).second->XMLNode->append_node(xmlNode_NewNode);

               // Update the stored reference.
               lstCardNameNode->at(iFound).second->XMLChildNodes[iSetKeyIndex] = xmlNode_NewNode;

            }
         } // Card already in database
      } // End Cards LIST

   } // End Sets

   std::cout << "Adding Sorted Nodes" << std::endl;
   // Iterate throught the buffer and add the nodes in the right order
   for (auto card : *lstCardNameNode)
   {
      xmlNode_Cards->append_node(card.second->XMLNode);
   }

   std::cout << "Parsing Complete" << std::endl;

   std::string xml_as_string;
   rapidxml::print(std::back_inserter(xml_as_string), *xmlCardDoc);

   // Save to file
   std::ofstream file_stored(aszFileName + ".out");
   file_stored << *xmlCardDoc;
   file_stored.close();
   xmlCardDoc->clear();

   std::cout << "XML Saved to Disk" << std::endl;

   for (auto card : *lstCardNameNode)
   {
      delete card.second;
   }

   lstCardNameNode->clear();

   std::cout << "Memory Cleared" << std::endl;
}

std::string CollectionSource::str_trim(const std::string& src, char removeChar)
{
   std::string rszOut = "";
   for (int i = 0; i < src.length(); i++)
   {
      if (src[i] != removeChar)
      {
         char c = src.c_str()[i];
         rszOut = rszOut + c;
      }

   }
   return rszOut;
}

std::string CollectionSource::str_clean(const std::string& src)
{
   std::string szRetVal = "";
   for (char c : src)
   {
      if (c == 145)
      {
         szRetVal += "ae";
      }
      else if (c == 146)
      {
         szRetVal += "AE";
      }
      else if (c == '\'' || c == '\"')
      {
         // leave out
      }
      else
      {
         szRetVal += c;
      }
   }

   return szRetVal;
}

int CollectionSource::import_Contains(std::vector<std::pair<std::string, CardAttributes*>>& buffer, std::string aszName)
{
   int iLeft = 0;
   int iRight = buffer.size();
   if (iRight < 1)
   {
      return -1;
   }

   while (iLeft <= iRight)
   {
      int middle = (iLeft + iRight) / 2;

      if (middle < 0 || middle >= buffer.size())
      {
         return -1;
      }

      if (buffer[middle].first == aszName)
         return middle;
      else if (aszName.compare(buffer[middle].first) < 0)
         iRight = middle - 1;
      else
         iLeft = middle + 1;
   }

   return -1;
}

void CollectionSource::import_AddSorted(std::vector<std::pair<std::string, CardAttributes*>>& buffer, std::pair<std::string, CardAttributes*>* element)
{
   std::string szName = element->first;

   int iLastSpot = 0;
   while (iLastSpot < buffer.size())
   {
      int iComparisonResult = buffer[iLastSpot].first.compare(szName);
      if (iComparisonResult < 0)
      {
         iLastSpot++;
      }
      else
      {
         break;
      }
   }

   std::vector<std::pair<std::string, CardAttributes*>>::iterator iter = buffer.begin() + iLastSpot;
   buffer.insert(iter, *element);
}

