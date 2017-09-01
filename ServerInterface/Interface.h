#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal_cppstd.h>
#include "..\MagicCollection\StoreFrontBackEnd.h"
// This class is how the C# client will access the native c++ code.
// There is a native ServerIFace class that exposes members for this class.
// This "Wraps" the native Server IFace Class.
// THIS NEEDS TO IMPLEMENT IDISPOSABLE
public ref class ServerClientInterface
{
public:
	ServerClientInterface();
	~ServerClientInterface();

	System::String^ LoadCollection(System::String^ aszCollectionName);
	System::String^ CreateNewCollection(System::String^ aszCollectionName, System::String^ ahszParent);
	void SaveCollection(System::String^ aszCollectionName);

   System::String^ GetImagesPath();
	System::String^ GetCardPrototype(System::String^ ahszCardName);

	System::Collections::Generic::List<System::String^>^ 
      GetLoadedCollections();

	System::Collections::Generic::List<System::String^>^ 
      GetAllCardsStartingWith(System::String^ ahszText);

	System::Collections::Generic::List<System::String^>^ 
      GetCollectionMetaData(System::String^ ahszCollectionName);
	System::Collections::Generic::List<System::String^>^ 
      GetCollectionList(System::String^ ahszCollectionName, System::Boolean ahbCollapsed);

	void SubmitBulkChanges(
      System::String^ ahszCollectionName,
      System::Collections::Generic::List<System::String^>^ ahlstBulkChanges);

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

