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
   std::string GetHistoryFolderName();
   std::string GetMetaFolderName();
   std::string GetImagesFolder();
   
   std::string GetCollectionsDirectory();
   std::string GetCollectionsFolderName();

   std::string GetKeyCode(std::string aszFullKey);
   std::string GetFullKey(std::string aszKeyCode);

   std::string GetHash(std::string& aszHashingString);

   std::vector<Tag>& GetPairedKeysList();
   std::vector<std::string>& GetIdentifyingAttributes();
   std::vector<std::string>& GetStaticAttributes();
   std::vector<std::string>& GetPerCollectionMetaTags();

   const std::function<std::string(const Tag&)> GetTagHelper(TagHelperType aiMode = Key) const;
   std::string GetHexID( unsigned long aulValue );

   bool IsPairedKey(std::string aszKey);
   bool IsValidKey(std::string aszKey);
   bool IsStaticAttribute(std::string aszAttrs);
   bool IsIdentifyingAttributes(std::string aszAttrs);

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

   std::function<std::string(const Tag&)> m_fnKeyExtractor;
   std::function<std::string(const Tag&)> m_fnValueExtractor;

   void initDefaultSettings();
   void initConfigSettings(std::ifstream& asConfig);

public:
   static Config* Instance();

   static char* Config::TrackingTagId;
   static char* Config::IgnoredTagId;
   static char* Config::MetaFileExtension;
   static char* Config::HistoryFileExtension;
   static char* Config::HashKey;
   static char* Config::NotFoundString;
   static char* Config::CollectionDefinitionKey;

private:
   static Config* Config::ms_pConfig;
};