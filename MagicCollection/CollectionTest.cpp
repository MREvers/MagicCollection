#include "CollectionTest.h"

#include<cstdio>
#include<Windows.h>
#include <iostream>
#include <fstream>
#include "Collection.h"
#include "Config.h"
#include "CollectionSource.h"
#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"

using namespace std;

CollectionTest::CollectionTest()
{
   Config::SetTestMode(true);
   // This requires at least 3 identifying attributes and 1 static attr.
   writeTestColSourceFile();

   m_ptColSource = new CollectionSource();
   m_ptColSource->LoadLib(Config::Instance()->GetSourceFile());
}


CollectionTest::~CollectionTest()
{
   Config::SetTestMode(false);
   delete m_ptColSource;
}


bool 
CollectionTest::AddItem_Test()
{
   auto col = getTestCollection();

   col->AddItem(*cardName(0));

   auto str = col->GetCollectionList();

   return true;
}

bool 
CollectionTest::RemoveItem_Test()
{
   return false;
}

bool 
CollectionTest::ChangeItem_Test()
{
   return false;
}

bool 
CollectionTest::AddItemFrom_Test()
{
   return false;
}

bool 
CollectionTest::ReplaceItem_Test()
{
   return false;
}

bool 
CollectionTest::LoadCollection_Test()
{
   return false;
}

bool 
CollectionTest::SaveCollection_Test()
{
   return false;
}

void 
CollectionTest::writeTestColSourceFile()
{
   rapidxml::xml_document<>* xmlCardDoc = new rapidxml::xml_document<>;

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

   // Add a test card
   rapidxml::xml_node<>* xmlNode_Card = xmlCardDoc->allocate_node(rapidxml::node_element, "card");
   rapidxml::xml_node<>* xmlNode_attr;

   xmlNode_attr= xmlCardDoc->allocate_node(rapidxml::node_element,
                                            Config::Instance()->GetStaticAttributes()[0].c_str(), cardName(0)->c_str() );
   xmlNode_Card->append_node(xmlNode_attr);
   
   xmlNode_attr= xmlCardDoc->allocate_node(rapidxml::node_element, 
                                            Config::Instance()->GetIdentifyingAttributes()[0].c_str(), getAttr(0)->c_str());
   xmlNode_Card->append_node(xmlNode_attr);

   xmlNode_attr= xmlCardDoc->allocate_node(rapidxml::node_element, 
                                            Config::Instance()->GetIdentifyingAttributes()[1].c_str(), getAttr(1)->c_str() );
   xmlNode_Card->append_node(xmlNode_attr);

   xmlNode_attr= xmlCardDoc->allocate_node(rapidxml::node_element, 
                                            Config::Instance()->GetIdentifyingAttributes()[2].c_str(), getAttr(2)->c_str() );
   xmlNode_Card->append_node(xmlNode_attr);

   xmlNode_Cards->append_node(xmlNode_Card);

   // Save to file
   auto srcFile = Config::Instance()->GetSourceFolder();
   CreateDirectory(srcFile.c_str(), NULL);
   srcFile = Config::Instance()->GetSourceFile();
   std::fstream file_stored(srcFile.c_str(), ios::out);
   file_stored << *xmlCardDoc;
   file_stored.close();
   xmlCardDoc->clear();
   delete xmlCardDoc;
}

void
CollectionTest::deleteTestColSourceFile()
{
   std::remove( Config::Instance()->GetSourceFile().c_str() );
}

std::shared_ptr<Collection> 
CollectionTest::getTestCollection()
{
   return std::shared_ptr<Collection>(new Collection("TestCol", m_ptColSource, "TestColFile", "TestID"));
}

const std::string*
CollectionTest::cardName( int iName )
{
   static const std::vector<std::string> lstNames( { "Card One" } );
   return &lstNames[iName];
}

const std::string*
CollectionTest::getAttr( int iAttr )
{
   static const std::vector<std::string> lstIdAttrs( { "BFZ::ABC", "1::2", "cat::dog::mouse" } );
   return &lstIdAttrs[iAttr];
}