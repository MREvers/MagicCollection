#pragma once

#include <vector>
#include <string>

#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"
#include "json.hpp"

// CardAttributes is used to convert allsets json to xml.
class CardAttributes
{
public:
   CardAttributes(std::string aszName)
   {
   };
   ~CardAttributes() {};

   std::string Keys[35];
   std::string Vals[35];
   rapidxml::xml_node<>* XMLNode;
   rapidxml::xml_node<>* XMLChildNodes[35];
};

class JSONImporter
{
public:
   JSONImporter();
   ~JSONImporter();

   void ImportJSON(std::string aszFileName);

private:
   int findInBuffer(std::vector<std::pair<std::string, CardAttributes*>>& buffer, std::string aszName);
   void addToBuffer(std::vector<std::pair<std::string, CardAttributes*>>& buffer, std::pair<std::string, CardAttributes*>* element);

   std::string str_trim(const std::string& srz, char removeChar);
   std::string str_clean(const std::string & src);
   int list_Find(std::string aszString, std::vector<std::string> alstSearch);
   std::vector<std::string> m_NonUniqueKeys; // These grab all values whenever found.
   std::vector<std::string> m_PairedKeys; // These will fill with a blank value if not found per card.
};

