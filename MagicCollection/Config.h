#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>

#include "rapidxml-1.13\rapidxml_print.hpp"
#include "rapidxml-1.13\rapidxml.hpp"
#include "rapidxml-1.13\rapidxml_utils.hpp"
#include "MD5.h"
#include "ListHelper.h"
#include "StringHelper.h"

typedef std::pair<std::string, std::string> Tag;

enum TagHelperType : int
{
   Key = 0x0,
   Value = 0x1
};

class Config
{
public:
   Config();
   ~Config();

   std::string GetImportSourceFile();
   std::string GetSourceFile();
   std::string GetSourceFolder();
   std::string GetOverheadFolerName();
   std::string GetHistoryFolderName();
   std::string GetMetaFolderName();
   std::string GetImagesFolder();
   
   std::string GetCollectionsDirectory();
   std::string GetCollectionsFolderName();

   char GetKeyCode(std::string aszFullKey);
   std::string GetFullKey(char aiKeyCode);

   std::string GetHash(std::string& aszHashingString);

   std::vector<std::string> GetPairedKeys(const std::string& aszKey);
   std::vector<Tag>& GetPairedKeysList();
   std::vector<std::string>& GetIdentifyingAttributes();
   std::vector<std::string>& GetStaticAttributes();
   std::vector<std::string>& GetPerCollectionMetaTags();

   const std::function<std::string(const Tag&)> GetTagHelper(TagHelperType aiMode = Key) const;
   std::string GetHexID( unsigned long aulValue );

   bool IsPairedKey(const std::string& aszKey);
   bool IsValidKey(const std::string& aszKey);
   bool IsStaticAttribute(const std::string& aszAttrs);
   bool IsIdentifyingAttributes(const std::string& aszAttrs);

   bool IsLoaded();

private:
   std::vector<Tag> m_lstKeyCodeMappings;
   std::vector<Tag> m_lstPairedKeys;
   std::vector<std::string> m_lstIdentifyingAttributes;
   std::vector<std::string> m_lstStaticAttributes;
   std::vector<std::string> m_lstPerCollectionMetaTags;

   std::string m_szImportSource;
   std::string m_szSourceFile;
   std::string m_szSourceFolder;

   std::string m_szCollectionsFolder;
   std::string m_szHistoryFolder;
   std::string m_szMetaFolder;
   std::string m_szImagesFolder;
   std::string m_szOverheadFolder;

   std::function<std::string(const Tag&)> m_fnKeyExtractor;
   std::function<std::string(const Tag&)> m_fnValueExtractor;

   void initDefaultSettings();
   void initConfigSettings(std::ifstream& asConfig);

   bool m_bIsFileConfig;

public:
   static Config* Instance();
   static void SetTestMode(bool abMode);
   static Config TestInstance();

   static char* Config::TrackingTagId;
   static char* Config::IgnoredTagId;
   static char* Config::MetaFileExtension;
   static char* Config::HistoryFileExtension;
   static char* Config::OverheadFileExtension;
   static char* Config::HashKey;
   static char* Config::NotFoundString;
   static char* Config::CollectionDefinitionKey;

private:
   static bool ms_bTestMode;
   static Config* Config::ms_pConfig;
   static Config* Config::ms_pTestConfig;
};