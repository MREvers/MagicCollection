#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal_cppstd.h>
#include "../MagicCollection/StoreFrontBackEnd.h"

// This class is how the C# client will access the native c++ code.
// There is a native ServerIFace class that exposes members for this class.
// This "Wraps" the native Server IFace Class.
// THIS NEEDS TO IMPLEMENT IDISPOSABLE
public ref class ServerClientInterface
{
public:
	ServerClientInterface();
	~ServerClientInterface();

	/* LoadCollection
	*  Loads specified collection into memory by file name. If fail, return an empty string.
	*  @param aszCollectionToLoad
	*  @return Name of collection if successful, "" if not.
	*/
	System::String^ 
      LoadCollection(System::String^ aszCollectionName);

   System::String^
	   GetImagesPath();


	System::Collections::Generic::List<System::String^>^ GetLoadedCollections();

	System::Collections::Generic::List<System::String^>^ 
      GetAllCardsStartingWith(System::String^ ahszText);

	System::Collections::Generic::List<System::String^>^
		GetCollectionListWithMeta(System::String^ ahszCollectionName);

	void ImportCollection();
private:
	CStoreFrontBackEnd* m_StoreFrontBackEnd;

	System::Collections::Generic::List<System::String^>^ 
      stringVectorToList(std::vector<std::string> alstTrans);
	std::vector<std::pair<std::string, std::string>> 
      tupleListToVector(System::Collections::Generic::List<System::Tuple<System::String^, System::String^>^>^ hlstMetaTagsOne);
   std::vector<std::string>
      stringListToVector(System::Collections::Generic::List<System::String^>^ hlstChanges);

};

