#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal_cppstd.h>
#include "../MagicCollection/StoreFrontBackEnd.h"

public ref class MCopyObject
{
public:
	System::Int32 Amount;
	System::String^ Name;
	System::Collections::Generic::Dictionary<System::String^, System::String^>^ Attributes;
};

// This class is how the C# client will access the native c++ code.
// There is a native ServerIFace class that exposes members for this class.
// This "Wraps" the native Server IFace Class.
// THIS NEEDS TO IMPLEMENT IDISPOSABLE
public ref class ServerClientInterface
{
public:
	ServerClientInterface();
	~ServerClientInterface();
	// Make the client calculate differences
	
	// These also takes "Fancy Cards" i.e. "Seeker of the Way { Set="blah" }
	void AddItem(System::String^ aszCollectionName,
		System::String^ aszCardName,
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ ahlstMeta);
	void RemoveItem(System::String^ aszCollectionName,
		System::String^ aszCardNameLong,
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ ahlstMeta);
	MCopyObject^ ConvertItemToCopyObject(System::String^ ahszCard);

	// Eventually Add file name arg.
	void SaveCollection(System::String^ aszCollectionName);
	// Returns the collection name
	System::String^ LoadCollection(System::String^ aszCollectionName);
	System::Collections::Generic::Dictionary<System::String^, System::String^>^ GetCopyLocations(System::String^ ahszCollectionName, System::String^ ahszLongCardName);
	System::Collections::Generic::List<System::String^>^ 
      GetCollectionList(System::String^ ahszCollectionName);
   System::Collections::Generic::List<
		System::Tuple<
		System::String^, System::String^>^>^ GetCardAttributes(System::String^ hszCardNameLong);
	System::Collections::Generic::List<System::String^>^ GetLoadedCollections();
	System::Collections::Generic::List<System::String^>^ GetAllCardsStartingWith(System::String^ ahszText);
	System::Collections::Generic::List<
		System::Tuple<
			System::String^,
			System::Collections::Generic::List<
				System::Tuple<
					System::String^,
					System::String^>^>^>^>^
		GetCollectionListWithMeta(System::String^ ahszCollectionName);
	System::Collections::Generic::List<System::String^>^ GetCardAttributeRestrictions(System::String^ ahszLongName, System::String^ ahszKey);
	System::Collections::Generic::List<System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^>^
		GetMetaTags(System::String^ ahszCollectionName, System::String^ ahszLongName);
	void AddMetaTag(System::String^ ahszCollectionName,
		System::String^ ahszLongName,
		System::String^ ahszKey,
		System::String^ ahszVal,
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags);
   void SetMetaTags(System::String^ ahszCollectionName,
      System::String^ ahszLongName,
      System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstNewTags,
      System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags);
	void AddMetaTag(System::String^ ahszCollectionName,
		System::String^ ahszLongName,
		System::String^ ahszKey,
		System::String^ ahszSubKey,
		System::String^ ahszVal,
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags);
	void RemoveMetaTag(System::String^ ahszCollectionName,
		System::String^ ahszLongName,
		System::String^ ahszKey,
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags);

   void SetFeatures(System::String^ ahszCollectionName,
      System::String^ ahszLongName,
      System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstNewTags,
      System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstNewAttsr,
      System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTags);

	System::Boolean IsSameIdentity(System::String^ aszLongNameOne, System::String^ aszLongNameTwo);
	System::Boolean IsSameMetaTags(
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTagsOne,
		System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTagsTwo);
	void ImportCollection();
private:
	CStoreFrontBackEnd* m_StoreFrontBackEnd;

	System::Collections::Generic::List<System::String^>^ stringVectorToList(std::vector<std::string> alstTrans);
	std::vector<std::pair<std::string, std::string>> tupleListToVector(System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTagsOne);
};

