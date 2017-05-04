#pragma once

#include <vector>
#include <string>

#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"
#include "json.hpp"

#include "Config.h"
#include "StringHelper.h"

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
};

