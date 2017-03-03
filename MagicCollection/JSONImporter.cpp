#include "JSONImporter.h"


JSONImporter::JSONImporter()
{
	m_NonUniqueKeys.push_back("set");
	m_NonUniqueKeys.push_back("multiverseid");
	m_PairedKeys.push_back("set");
	m_PairedKeys.push_back("multiverseid");
}


JSONImporter::~JSONImporter()
{
}

// ConvertJSONCollection
void JSONImporter::ImportJSON(std::string aszFileName)
{
	// Load the JSON object. This may take a while.
	std::cout << "Load the JSON object. This may take a while." << std::endl;

	std::ifstream i(aszFileName);
	if (!i.good())
	{
		return;
	}
	nlohmann::json j;
	i >> j;

	std::cout << "JSON Loaded Into Memory: " << sizeof(j) << std::endl;
	clock_t begin = clock();
	// Allocate on Heap to avoid stack overflow.
	rapidxml::xml_document<>* xmlCardDoc = new rapidxml::xml_document<>;

	// Allocate on Heap to avoid stack overflow.
	// CardAttributes* will actually be "Card Attributes"
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
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		std::string szSetName = iter_set.key();
		std::cout << "Set: " << szSetName << " Cards: " << lstCardNameNode->size() << " " << elapsed_secs << std::endl;
		nlohmann::json jsonListCards = iter_set.value().at("cards");

		// jsonCardsList is a json-list of json objects... same as JSON, but no 'key'
		for (nlohmann::json::iterator iter_cards = jsonListCards.begin(); iter_cards != jsonListCards.end(); ++iter_cards)
		{
			// Get the first card json object.
			nlohmann::json jsonCard = iter_cards.value();
			std::stringstream ssCardName;
			ssCardName << jsonCard.at("name").begin().value();
			std::string szCardName = str_trim(ssCardName.str(), '\"');

			std::vector<std::string> lstFoundPairedTags; // Used to see if we need to add blanks later
			CardAttributes* oCA;
			int iFound = findInBuffer(*lstCardNameNode, str_trim(szCardName, '\"'));
			if (iFound == -1)
			{
				// Create the card node.
				rapidxml::xml_node<>* xmlNode_Card = xmlCardDoc->allocate_node(rapidxml::node_element, "card");

				// Dont add the node now, these get sorted first.
				//xmlNode_Cards->append_node(xmlNode_Card);

				// Create the data structure.
				oCA = new CardAttributes(szCardName);

				std::pair<std::string, CardAttributes*>* pair_Name_CA =
					new std::pair<std::string, CardAttributes*>(str_trim(szCardName, '\"'), oCA);

				oCA->XMLNode = xmlNode_Card;

				// Add the set..
				oCA->Keys[0] = str_trim("set", '\"');
				oCA->Vals[0] = str_trim(szSetName, '\"');
				lstFoundPairedTags.push_back("set");

				rapidxml::xml_node<>* xmlNode_CardAttribute = xmlCardDoc->allocate_node(rapidxml::node_element,
					oCA->Keys[0].c_str(), oCA->Vals[0].c_str());
				xmlNode_Card->append_node(xmlNode_CardAttribute);

				oCA->XMLChildNodes[0] = xmlNode_CardAttribute;

				// Add the other attributes.
				int iNumKeyValPair = 1;
				for (nlohmann::json::iterator iter_card = jsonCard.begin(); iter_card != jsonCard.end(); ++iter_card)
				{
					std::string szKey = iter_card.key();

					if (szKey == "set" ||
						szKey == "power" ||
						szKey == "toughness" ||
						szKey == "manaCost" ||
						szKey == "text" ||
						szKey == "loyalty" ||
						szKey == "name" ||
						szKey == "colors" ||
						szKey == "multiverseid")
					{
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

						// Check if it's a paired tag, if so, record that we found a value for it.
						if (list_Find(szKey, m_PairedKeys) != -1)
						{
							lstFoundPairedTags.push_back(szKey);
						}
					}

				} // End Card Attributes iterator

				  // The pair is built up now add it.
				addToBuffer(*lstCardNameNode, pair_Name_CA);
				delete pair_Name_CA;
			} // End card not yet added
			else
			{
				// Check if its a nonunique key. If so, log the additional value.
				oCA = (CardAttributes*)(lstCardNameNode->at(iFound).second);
				std::vector<std::string> lstCardNameNodeKeys(oCA->Keys, oCA->Keys + 35);
				std::vector<std::string>::iterator iter_NonUniqueKeys = m_NonUniqueKeys.begin();
				for (; iter_NonUniqueKeys != m_NonUniqueKeys.end(); ++iter_NonUniqueKeys)
				{
					int iKeyIndex = -1;
					if ((iKeyIndex = list_Find(*iter_NonUniqueKeys, lstCardNameNodeKeys)) != -1)
					{
						std::string szValue;

						// Get the additional value. The "Set" key is special
						bool bFoundKey = false;
						if (*iter_NonUniqueKeys != "set")
						{
							nlohmann::json::iterator iter_card;
							for (iter_card = jsonCard.begin();
								iter_card != jsonCard.end();
								++iter_card)
							{
								std::string szKey;
								{
									std::stringstream ss;
									ss << iter_card.key();
									szKey = ss.str();
								}
								if (szKey == *iter_NonUniqueKeys)
								{
									bool bIsInt = iter_card.value().is_number_integer();
									if (bIsInt)
									{
										szValue = std::to_string(iter_card.value().get<int>());
									}
									else
									{
										szValue = iter_card.value().get<std::string>();
									}
									break;
								}
							}

							if (iter_card != jsonCard.end())
							{
								bFoundKey = true;
							}
						}
						else
						{
							szValue = szSetName;
							bFoundKey = true;
						}

						if (bFoundKey)
						{
							// Prefer blank of null
							if (szValue.size() == 0)
							{
								szValue = " ";
							}


							// Update the string buffer
							oCA->Vals[iKeyIndex] = str_clean(oCA->Vals[iKeyIndex] + "::" + szValue);

							// Update the node
							rapidxml::xml_node<>* xmlNode_RemoveNode = oCA->XMLChildNodes[iKeyIndex];
							rapidxml::xml_node<>* xmlNode_NewNode = xmlCardDoc->allocate_node(rapidxml::node_element,
								oCA->Keys[iKeyIndex].c_str(),
								oCA->Vals[iKeyIndex].c_str());

							// Swap out the old node for the new.
							// Uncomment this line if "Set" is unique..
							oCA->XMLNode->remove_node(xmlNode_RemoveNode);
							oCA->XMLNode->append_node(xmlNode_NewNode);

							// Update the stored reference.
							oCA->XMLChildNodes[iKeyIndex] = xmlNode_NewNode;

							// Check if it's a paired tag, if so, record that we found a value for it.
							if (list_Find(*iter_NonUniqueKeys, m_PairedKeys) != -1)
							{
								lstFoundPairedTags.push_back(*iter_NonUniqueKeys);
							}
						}

					}
				}
			} // Card already in database

			// Fill in blanks for all the paired tags not present.
			std::vector<std::string>::iterator iter_PairedTags = m_PairedKeys.begin();
			for (; iter_PairedTags != m_PairedKeys.end(); ++iter_PairedTags)
			{
				if (list_Find(*iter_PairedTags, lstFoundPairedTags) == -1)
				{
					// We need to add a blank.
					std::vector<std::string> lstCardNameNodeKeys(oCA->Keys, oCA->Keys + 35);
					int iKeyIndex = list_Find(*iter_PairedTags, lstCardNameNodeKeys);

					// Perhaps this tag has not been encountered yet
					if (iKeyIndex != -1)
					{
						// Update the string buffer
						oCA->Vals[iKeyIndex] = str_clean(oCA->Vals[iKeyIndex] + ":: ");

						// Update the node
						rapidxml::xml_node<>* xmlNode_RemoveNode = oCA->XMLChildNodes[iKeyIndex];
						rapidxml::xml_node<>* xmlNode_NewNode = xmlCardDoc->allocate_node(rapidxml::node_element,
							oCA->Keys[iKeyIndex].c_str(),
							oCA->Vals[iKeyIndex].c_str());

						// Swap out the old node for the new.
						// Uncomment this line if "Set" is unique..
						oCA->XMLNode->remove_node(xmlNode_RemoveNode);
						oCA->XMLNode->append_node(xmlNode_NewNode);

						// Update the stored reference.
						oCA->XMLChildNodes[iKeyIndex] = xmlNode_NewNode;
					}
					else
					{
						// Find the last item
						int iNumKeyVals = 0;
						for (int i = 34; i > 0; i--)
						{
							if (oCA->Vals[i] != "" && oCA->Keys[i] != "")
							{
								iNumKeyVals = i + 1;
								break;
							}
						}

						oCA->Vals[iNumKeyVals] = " ";
						oCA->Keys[iNumKeyVals] = *iter_PairedTags;

						rapidxml::xml_node<>* xmlNode_NewNode = xmlCardDoc->allocate_node(rapidxml::node_element,
							oCA->Keys[iNumKeyVals].c_str(),
							oCA->Vals[iNumKeyVals].c_str());



						// Swap out the old node for the new.
						oCA->XMLNode->append_node(xmlNode_NewNode);

						// Update the stored reference.
						oCA->XMLChildNodes[iNumKeyVals] = xmlNode_NewNode;
					}

				} // End, Is paired tag needed
			} // End, fill in paired tags

		} // End Cards LIST for set

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

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Load Time: " << elapsed_secs << std::endl;

	std::cout << "XML Saved to Disk" << std::endl;

	for (auto card : *lstCardNameNode)
	{
		delete card.second;
	}

	lstCardNameNode->clear();

	std::cout << "Memory Cleared" << std::endl;
}


int JSONImporter::findInBuffer(std::vector<std::pair<std::string, CardAttributes*>>& buffer, std::string aszName)
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

void JSONImporter::addToBuffer(std::vector<std::pair<std::string, CardAttributes*>>& buffer, std::pair<std::string, CardAttributes*>* element)
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


std::string JSONImporter::str_trim(const std::string& src, char removeChar)
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

std::string JSONImporter::str_clean(const std::string& src)
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

int JSONImporter::list_Find(std::string aszString, std::vector<std::string> alstSearch)
{
	std::vector<std::string>::iterator iter_list = alstSearch.begin();
	int index = 0;
	for (; iter_list != alstSearch.end(); iter_list++)
	{
		if (*iter_list == aszString)
		{
			return index;
		}
		index++;
	}
	return -1;
}