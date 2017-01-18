#pragma once
#pragma message ("Starting CollectionSource.h")
#include "SourceObject.h"
#include "CollectionObject.h"

#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"
#include "json.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>

class CollectionObject;
class Collection;

// CardAttributes is used to convert allsets json to xml.
class CardAttributes : public IName
{
   public:
      CardAttributes(std::string aszName) : IName(aszName){};
      ~CardAttributes(){};

      std::string Keys[35];
      std::string Vals[35];
      rapidxml::xml_node<>* XMLNode;
      rapidxml::xml_node<>* XMLChildNodes[35];
};

class CollectionSource
{
public:
   CollectionSource();
   ~CollectionSource();
   
   // Database Functions
   void ConvertJSONCollection(std::string aszFileName);
   void LoadLib(std::string aszFileName);

   // Service functions
   bool GetCard(std::string aszName, CollectionObject& roCard, CollectionObject*& roRetCard);
   void PrintAllWith(std::string aszMatch);
   void PrintAllWith(std::string aszMatch, bool caseSensitive);

private:
   std::vector<SourceObject*> m_lstCardBuffer;
   std::string str_trim(const std::string& srz, char removeChar);
   std::string str_clean(const std::string & src);
};

#pragma message ("Finish CollectionSource.h")