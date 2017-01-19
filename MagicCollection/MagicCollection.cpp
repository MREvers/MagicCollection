// MagicCollection.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <iterator>
#include <xmllite.h>

#include "json.hpp"
#include "json/json.h"
#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "Collection.h"
#include "CollectionSource.h"

using json = nlohmann::json;

std::string escape(const std::string& str);

int main()
{
   CollectionSource CS;
   CS.LoadLib("AllCards.xml");

   Collection Col("Primary", &CS);
   Col.AddItem("Thing in the Ice");
   Col.AddItem("Thing in the Ice");
   Col.PrintList();
   
   //CS.PrintAllWith("Ice", true);
   //CS.ConvertJSONCollection("AllSets.json");

   /*
   
   // read a JSON file
   std::ifstream i("JSONTest.txt");
   std::ofstream outputStream("TestOut.xml");

   json j;
   i >> j;
   json cardsJ = j;

   rapidxml::xml_document<> doc;
   rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
   decl->append_attribute(doc.allocate_attribute("version", "1.0"));
   decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
   doc.append_node(decl);

   rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_element, "Card_Database");
   //root->append_attribute(doc.allocate_attribute("version", "1.0"));
   //root->append_attribute(doc.allocate_attribute("type", "example"));
   doc.append_node(root);

   rapidxml::xml_node<>* child = doc.allocate_node(rapidxml::node_element, "cards");
   root->append_node(child);
   std::vector<std::string>* lstCards = new std::vector<std::string>();
   lstCards->resize(102000);
   for (json::iterator it = cardsJ.begin(); it != cardsJ.end(); ++it)
   {
      json js = it.value().at("cards");

      for (json::iterator iter = js.begin(); iter != js.end(); ++iter)
      {
         rapidxml::xml_node<>* card = doc.allocate_node(rapidxml::node_element, "card");
         child->append_node(card);
         json cardjs = iter.value();
         
         for (json::iterator cardIter = cardjs.begin(); cardIter != cardjs.end(); ++cardIter)
         {
            
            std::stringstream ss;
            ss << cardIter.value();
            
            lstCards->push_back(trim(ss.str(), '\"'));
            lstCards->push_back(trim(cardIter.key(), '\"'));

            rapidxml::xml_node<>* cardKey = doc.allocate_node(rapidxml::node_element,
               (*lstCards)[lstCards->size()-1].c_str(), (*lstCards)[lstCards->size() - 2].c_str());
            card->append_node(cardKey);
         }
         
      }

   }

   // Convert doc to string if needed
   std::string xml_as_string;
   rapidxml::print(std::back_inserter(xml_as_string), doc);

   // Save to file
   std::ofstream file_stored("file_stored.xml");
   file_stored << doc;
   file_stored.close();
   doc.clear();

   lstCards->clear();
   delete lstCards;
   */
  //std::cout << j.at("cards").cbegin().value() << std::endl;
  //std::cout << j.at("cards").cbegin().value().find("name").value() << std::endl;
  /*
  json cardsJ = j.at("cards").cbegin().value();
   for (json::iterator it = cardsJ.begin(); it != cardsJ.end(); ++it)
   {
      std::cout << it.key() << " : " << it.value() << "\n";
   }
   */
   /*
   json cardsJ = j.at("cards");
   for (json::iterator it = cardsJ.begin(); it != cardsJ.end(); ++it)
   {
      std::cout <<  it.value() << "\n";
   }
   */
   /*
   json cardsJ = j.at("cards");
   for (json::iterator it = cardsJ.begin(); it != cardsJ.end(); ++it)
   {
      std::cout << it.value().find("name").value() << "\n";
   }
   */


   std::cin.get();
    return 0;
}

std::string escape(const std::string& src)
{
   std::stringstream dst;
   for (char ch : src)
   {
      switch (ch)
      {
      case '&': dst << "&amp;"; break;
      case '\'': dst << "&apos;"; break;
      case '"': dst << "&quot;"; break;
      case '<': dst << "&lt;"; break;
      case '>': dst << "&gt;"; break;
      default: dst << ch; break;
      }
   }
   return dst.str();
}


