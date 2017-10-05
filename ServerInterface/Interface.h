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

	String^ LoadCollection(String^ aszCollectionName);
	String^ CreateNewCollection(String^ aszCollectionName, String^ ahszParent);
	void SaveCollection(String^ aszCollectionName);

   String^ GetSourceFilePath();
   String^ GetImportSourceFilePath();
   String^ GetImagesPath();
	String^ GetCardPrototype(String^ ahszCardName);

	List<String^>^ GetLoadedCollections();

	List<String^>^ GetAllCardsStartingWith(String^ ahszText);

	List<String^>^ GetCollectionMetaData(String^ ahszCollectionName);
	List<String^>^ GetCollectionList(String^ ahszCollectionName);

	void SubmitBulkChanges(String^ ahszCollectionName, List<String^>^ ahlstBulkChanges);

	void ImportCollection();
private:
	CStoreFrontBackEnd* m_StoreFrontBackEnd;

	List<String^>^ convertStrVecToLst(std::vector<std::string> alstTrans);

	vector<Tag> revertTupLstToVec(List<HTag^>^ hlstMetaTagsOne);
   vector<string> revertStrLstToVec(List<String^>^ hlstChanges);
  
};

