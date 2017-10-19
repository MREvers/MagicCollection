#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal_cppstd.h>
#include "..\MagicCollection\StoreFrontBackEnd.h"

typedef std::pair<std::string, std::string> Tag;

typedef System::String String;
typedef System::Tuple<String^, String^> HTag;
template<typename T> using List = System::Collections::Generic::List<T>;

using namespace std;

// This class is how the C# client will access the native c++ code.
// There is a native ServerIFace class that exposes members for this class.
// This "Wraps" the native Server IFace Class.
// THIS NEEDS TO IMPLEMENT IDISPOSABLE
public ref class ServerClientInterface
{
public:
	ServerClientInterface();
	~ServerClientInterface();

   // Program Management
   //
   System::Boolean IsConfigLoaded();

   // Collection Manager Accessors
   //
	String^ LoadCollection(String^ aszCollectionName);
	String^ CreateNewCollection(String^ aszCollectionName, String^ ahszParent);
	List<String^>^ GetLoadedCollections();

   // Collection Accessors
   //
	void SaveCollection(String^ aszCollectionName);
	List<String^>^ GetCollectionMetaData(String^ ahszCollectionName);
	List<String^>^ GetCollectionList(String^ ahszCollectionName);
	void SubmitBulkChanges(String^ ahszCollectionName, List<String^>^ ahlstBulkChanges);

   // Card Accessors
   //
   void SetAttribute(String^ ahszCardName, String^ ahszUID, String^ ahszKey, String^ ahszVal);

   // Source Accessors
   //
	List<String^>^ GetAllCardsStartingWith(String^ ahszText);
	String^ GetCardPrototype(String^ ahszCardName);
	void ImportCollection();

   // Config accessors
   //
   String^ GetSourceFilePath();
   String^ GetImportSourceFilePath();
   String^ GetImagesPath();
   List<HTag^>^ GetPairedAttributes();

private:
	CStoreFrontBackEnd* m_StoreFrontBackEnd;

	List<String^>^ convertStrVecToLst(vector<string> alstTrans);
   List<HTag^>^ convertTupVecToLst(vector<Tag> alstTups);

	vector<Tag> revertTupLstToVec(List<HTag^>^ hlstMetaTagsOne);
   vector<string> revertStrLstToVec(List<String^>^ hlstChanges);
  
};

