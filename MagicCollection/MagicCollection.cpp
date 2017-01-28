// MagicCollection.cpp : Defines the entry point for the console application.
//
#define _ITERATOR_DEBUG_LEVEL 0  
#include <iostream>
#include <fstream>
#include <iterator>
#include <xmllite.h>

#include "json.hpp"
#include "json/json.h"
#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "Collection.h"
#include "JSONImporter.h"
#include "CollectionSource.h"
#include "CollectionFactory.h"

using json = nlohmann::json;

std::string escape(const std::string& str);

int main()
{
   CollectionSource CS;
   CS.LoadLib("AllSets.json.out");

   CollectionFactory CF(&CS);

   Collection* Col = CF.LoadCollection("Primary", "TestCollection.txt");
   Col->CreateBaselineHistory();
   Col->RemoveItem("Sylvan Advocate");
   
   

   Collection* ColM = CF.LoadCollection("Main", "TestCollectionTwo.txt");
   ColM->CreateBaselineHistory();
   Col->SaveCollection("ColOut.txt");
   ColM->SaveCollection("ColTwoOut.txt");
   Col->PrintList();
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


