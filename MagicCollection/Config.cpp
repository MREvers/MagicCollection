#include "Config.h"

#include <sstream>
/*
const char * const CollectionObject::LstUniqueTraits[] = { "manaCost", "colors", "name", "power",
"toughness", "loyalty", "text" };

const char * const CollectionObject::LstNonUniqueTraits[] = { "set", "multiverseid" };
*/

Config* Config::ms_pConfig = nullptr;
Config* Config::ms_pTestConfig = nullptr;
char* Config::MetaFileExtension = "meta";
char* Config::HistoryFileExtension = "history";
char* Config::OverheadFileExtension = "data";
char* Config::TrackingTagId = "__";
char* Config::IgnoredTagId = "_";
char* Config::HashKey = "__hash";
char* Config::NotFoundString = "NF";
char* Config::CollectionDefinitionKey = ":";
bool Config::ms_bTestMode = false;

Config::Config()
{
   m_bIsFileConfig = false;
   m_fnKeyExtractor = [](Tag atag)->std::string { return atag.first; };
   m_fnValueExtractor = [](Tag atag)->std::string { return atag.second; };

   std::ifstream file(".\\Config\\Config.xml");
   if (!file.good())
   {
      initDefaultSettings();
   }
   else
   {
      initConfigSettings(file);
      m_bIsFileConfig = true;
   }
}


Config::~Config()
{
}

std::string Config::GetSourceFile()
{
   return ".\\Config\\" + m_szSourceFolder + "\\" + m_szSourceFile;
}

std::string 
Config::GetSourceFolder()
{
   return ".\\Config\\" + m_szSourceFolder;
}

std::string Config::GetImportSourceFile()
{
   return ".\\Config\\" + m_szSourceFolder + "\\" + m_szImportSource;
}

std::string 
Config::GetOverheadFolerName()
{
   return m_szOverheadFolder;
}

std::string 
Config::GetHistoryFolderName()
{
   return m_szHistoryFolder;
}

std::string Config::GetMetaFolderName()
{
   return m_szMetaFolder;
}

std::string Config::GetImagesFolder()
{
   return ".\\Config\\" + m_szImagesFolder;
}

std::string Config::GetCollectionsDirectory()
{
   return ".\\" + GetCollectionsFolderName();
}

std::string Config::GetCollectionsFolderName()
{
   return m_szCollectionsFolder;
}

// Returns a numerical key code
// -1 if not found.
char
Config::GetKeyCode(std::string aszFullKey)
{
   static std::map<std::string, char> mapFastKey;
   auto mapKey =  mapFastKey.find( aszFullKey );
   if( mapKey != mapFastKey.end() )
   {
      return mapKey->second;
   }

   auto iter_KeyMappings = m_lstKeyCodeMappings.begin();
   for (; iter_KeyMappings != m_lstKeyCodeMappings.end(); ++iter_KeyMappings)
   {
      if (iter_KeyMappings->first == aszFullKey)
      {
         char cCode = 0;
         size_t cNumChars;
         cCode = std::stoi(iter_KeyMappings->second, &cNumChars);
         if( cNumChars > 0 )
         {
            mapFastKey.insert(std::make_pair(aszFullKey, cCode));
            return cCode;
         }
         else
         {
            return -1;
         }
      }
   }

   return -1;
}

std::string 
Config::GetFullKey(char aiKeyCode)
{
   auto iter_KeyMappings = m_lstKeyCodeMappings.begin();
   for (; iter_KeyMappings != m_lstKeyCodeMappings.end(); ++iter_KeyMappings)
   {
      size_t iNumChars;
      char cCode = std::stoi(iter_KeyMappings->second, &iNumChars);
      if( (iNumChars > 0) && (cCode == aiKeyCode) )
      {
         return iter_KeyMappings->first;
      }
   }

   return "";
}

std::string Config::GetHash(std::string& aszHashingString)
{
   MD5* md5Hasher = new MD5(aszHashingString);
   std::string szResult = md5Hasher->hexdigest();
   delete md5Hasher;
   return szResult;
}

std::vector<std::string> 
Config::GetPairedKeys( const std::string& aszKey )
{
   std::vector<std::string> vecRetval;
   for( auto& tag : m_lstPairedKeys )
   {
      if( tag.first == aszKey )
      {
         vecRetval.push_back(tag.second);
      }
      else if( tag.second == aszKey )
      {
         vecRetval.push_back(tag.first);
      }
   }
   return vecRetval;
}

std::vector<std::pair<std::string, std::string>>& Config::GetPairedKeysList()
{
   return m_lstPairedKeys;
}
std::vector < std::string>& Config::GetIdentifyingAttributes()
{
   return m_lstIdentifyingAttributes;
}
std::vector<std::string>& Config::GetStaticAttributes()
{
   return m_lstStaticAttributes;
}

std::vector<std::string>& Config::GetPerCollectionMetaTags()
{
   return m_lstPerCollectionMetaTags;
}

const std::function<std::string(const Tag&)> Config::GetTagHelper(TagHelperType aiMode) const
{
   if (aiMode == Key)
   {
      return m_fnKeyExtractor;
   }
   else
   {
      return m_fnValueExtractor;
   }
}

std::string Config::GetHexID( unsigned long aulValue )
{
   std::stringstream stream;
   stream << std::setfill( '0' ) << std::setw( 3 ) << std::hex;
   
   stream << aulValue;

   return std::string( stream.str().substr(0, 3) );
}

bool Config::IsIdentifyingAttributes(const std::string& aszAttrs)
{
   return ListHelper::List_Find(aszAttrs, m_lstIdentifyingAttributes) != -1;
}

bool Config::IsPairedKey(const std::string& aszKey)
{
   return   ListHelper::List_Find(aszKey, m_lstPairedKeys, m_fnKeyExtractor) != -1 ||
          ListHelper::List_Find(aszKey, m_lstPairedKeys, m_fnValueExtractor) != -1;
}

bool Config::IsValidKey(const std::string& aszKey)
{
   // A valid key is either a static attr or identifying attr
   return ListHelper::List_Find(aszKey, m_lstStaticAttributes) != -1 ||
      ListHelper::List_Find(aszKey, m_lstIdentifyingAttributes) != -1;
}

bool Config::IsStaticAttribute(const std::string& aszAttr)
{
   return ListHelper::List_Find(aszAttr, m_lstStaticAttributes) != -1;
}

bool 
Config::IsLoaded()
{
   return m_bIsFileConfig;
}

Config* Config::Instance()
{
   if (ms_pConfig == nullptr)
   {
      ms_pConfig = new Config();
   }

   if( ms_pTestConfig == nullptr && ms_bTestMode )
   {
      ms_pTestConfig = new Config(TestInstance());
   }

   if( ms_bTestMode )
   {
      return ms_pTestConfig;
   }

   return ms_pConfig;
}

void
Config::SetTestMode( bool abMode )
{
   ms_bTestMode = abMode;
}

Config 
Config::TestInstance()
{
   Config cTest;
   cTest.m_lstIdentifyingAttributes.clear();
   cTest.m_lstKeyCodeMappings.clear();
   cTest.m_lstPairedKeys.clear();
   cTest.m_lstPerCollectionMetaTags.clear();
   cTest.m_lstStaticAttributes.clear();
   cTest.m_szSourceFolder = "Test";
   cTest.m_szSourceFile = "TestSource.xml";

   cTest.m_lstStaticAttributes.push_back("name");
   cTest.m_lstIdentifyingAttributes.push_back("set");
   cTest.m_lstIdentifyingAttributes.push_back("mud");
   cTest.m_lstIdentifyingAttributes.push_back("solo");

   cTest.m_lstPairedKeys.push_back(std::make_pair("set","mud"));

   cTest.m_lstKeyCodeMappings.push_back(std::make_pair("name", "1"));
   cTest.m_lstKeyCodeMappings.push_back(std::make_pair("set", "2"));
   cTest.m_lstKeyCodeMappings.push_back(std::make_pair("mud", "3"));
   cTest.m_lstKeyCodeMappings.push_back(std::make_pair("solo", "4"));

   return cTest;
}

void Config::initDefaultSettings()
{
   m_lstStaticAttributes.push_back("manaCost");
   m_lstStaticAttributes.push_back("colors");
   m_lstStaticAttributes.push_back("name");
   m_lstStaticAttributes.push_back("names");
   m_lstStaticAttributes.push_back("power");
   m_lstStaticAttributes.push_back("toughness");
   m_lstStaticAttributes.push_back("loyalty");
   m_lstStaticAttributes.push_back("text");
   m_lstStaticAttributes.push_back("cmc");
   m_lstStaticAttributes.push_back("colorIdentity");

   m_lstIdentifyingAttributes.push_back("set");
   m_lstIdentifyingAttributes.push_back("multiverseid");

   m_lstPairedKeys.push_back(std::make_pair("set", "multiverseid"));

   m_lstKeyCodeMappings.push_back(std::make_pair("set", "set"));
   m_lstKeyCodeMappings.push_back(std::make_pair("multiverseid", "mid"));
   m_lstKeyCodeMappings.push_back(std::make_pair("manaCost", "man"));
   m_lstKeyCodeMappings.push_back(std::make_pair("colors", "clr"));
   m_lstKeyCodeMappings.push_back(std::make_pair("name", "nam"));
   m_lstKeyCodeMappings.push_back(std::make_pair("toughness", "tuf"));
   m_lstKeyCodeMappings.push_back(std::make_pair("loyalty", "loy"));
   m_lstKeyCodeMappings.push_back(std::make_pair("text", "txt"));
   m_lstKeyCodeMappings.push_back(std::make_pair("cmc", "cmc"));
   m_lstKeyCodeMappings.push_back(std::make_pair("colorIdentity", "cid"));

   m_lstPerCollectionMetaTags.push_back("Generalization");

   m_szImportSource = "AllSets.json";
   m_szSourceFile = "MtGSource.xml";
   m_szSourceFolder = "Source";

   m_szCollectionsFolder = "Collections";
   m_szHistoryFolder = "Data";
   m_szMetaFolder = "Data";
   m_szOverheadFolder = "Data";
}

void Config::initConfigSettings(std::ifstream& asConfig)
{
   rapidxml::xml_document<> doc;
   std::stringstream buffer;
   buffer << asConfig.rdbuf();
   asConfig.close();

   std::cout << "Parsing Doc" << std::endl;
   std::string content(buffer.str());
   doc.parse<0>(&content[0]);
   std::cout << "Parse Done" << std::endl;

   rapidxml::xml_node<> *xmlNode_Config = doc.first_node();

   rapidxml::xml_node<> *xmlNode_Source = xmlNode_Config->first_node("Source");
   rapidxml::xml_node<> *xmlNode_ImportDefinitions = xmlNode_Source->first_node("ImportDefinitions");
   rapidxml::xml_node<> *xmlNode_ValidKeys = xmlNode_ImportDefinitions->first_node("ValidKeys");
   rapidxml::xml_node<> *xmlNode_ValidKey = xmlNode_ValidKeys->first_node("Key");
   while (xmlNode_ValidKey != 0)
   {
      std::string szKey = xmlNode_ValidKey->value();
      m_lstStaticAttributes.push_back(szKey);

      xmlNode_ValidKey = xmlNode_ValidKey->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_IdentifyingKeys = xmlNode_ImportDefinitions->first_node("IdentifyingKeys");
   xmlNode_ValidKey = xmlNode_IdentifyingKeys->first_node("Key");
   while (xmlNode_ValidKey != 0)
   {
      std::string szKey = xmlNode_ValidKey->value();
      m_lstIdentifyingAttributes.push_back(szKey);

      xmlNode_ValidKey = xmlNode_ValidKey->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_PairedKeys = xmlNode_ImportDefinitions->first_node("PairedKeys");
   rapidxml::xml_node<> *xmlNode_Pair = xmlNode_PairedKeys->first_node("Pair");
   while (xmlNode_Pair != 0)
   {
      rapidxml::xml_node<> *xmlNode_Key = xmlNode_Pair->first_node();

      std::string szKey = xmlNode_Key->value();

      xmlNode_Key = xmlNode_Key->next_sibling();

      std::string szValue = xmlNode_Key->value();

      m_lstPairedKeys.push_back(std::make_pair(szKey, szValue));

      xmlNode_Pair = xmlNode_Pair->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_CodeKeys = xmlNode_ImportDefinitions->first_node("CodeKeys");
   xmlNode_Pair = xmlNode_CodeKeys->first_node("Pair");
   while (xmlNode_Pair != 0)
   {
      rapidxml::xml_node<> *xmlNode_Key = xmlNode_Pair->first_node();

      std::string szKey = xmlNode_Key->value();

      xmlNode_Key = xmlNode_Key->next_sibling();

      std::string szValue = xmlNode_Key->value();

      m_lstKeyCodeMappings.push_back(std::make_pair(szKey, szValue));

      xmlNode_Pair = xmlNode_Pair->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_Collections = xmlNode_Config->first_node("Collection");
   rapidxml::xml_node<> *xmlNode_CollectionDefinitions = xmlNode_Collections->first_node("CollectionDefinitions");
   rapidxml::xml_node<> *xmlNode_CollectionMetaKeys = xmlNode_CollectionDefinitions->first_node("CollectionMetaKeys");

   rapidxml::xml_node<> *xmlNode_MetaKey = xmlNode_CollectionMetaKeys->first_node("Key");
   while (xmlNode_MetaKey != 0)
   {
      std::string szKey = xmlNode_MetaKey->value();
      m_lstPerCollectionMetaTags.push_back(szKey);

      xmlNode_MetaKey = xmlNode_MetaKey->next_sibling();
   }

   rapidxml::xml_node<> *xmlNode_Node = xmlNode_Source->first_node("SourceFolder");
   m_szSourceFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Source->first_node("ImportFile");
   m_szImportSource = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Source->first_node("SourceFile");
   m_szSourceFile = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("CollectionFolder");
   m_szCollectionsFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("HistoryFolder");
   m_szHistoryFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("OverheadFolder");
   m_szOverheadFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Collections->first_node("MetaFolder");
   m_szMetaFolder = xmlNode_Node->value();

   xmlNode_Node = xmlNode_Config->first_node("Images");
   xmlNode_Node = xmlNode_Node->first_node("ImagesFolder");
   m_szImagesFolder = xmlNode_Node->value();
}

