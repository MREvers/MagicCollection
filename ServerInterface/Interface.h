#pragma once
#using <mscorlib.dll>
#using <System.dll>
#include <msclr\marshal_cppstd.h>
#include "../MagicCollection/StoreFrontBackEnd.h"

// This class is how the C# client will access the native c++ code.
// There is a native ServerIFace class that exposes members for this class.
// This "Wraps" the native Server IFace Class.
public ref class ServerClientInterface
{
public:
	ServerClientInterface();
	~ServerClientInterface();

	// Make the client calculate differences
	
	// These also takes "Fancy Cards" i.e. "Seeker of the Way { Set="blah" }
	void AddItem(System::String^ aszCollectionName, System::String^ aszCardName);
	void RemoveItem(System::String^ aszCollectionName, System::String^ aszCardName);

	// Eventually Add file name arg.
	void SaveCollection(System::String^ aszCollectionName);
	// Returns the collection name
	System::String^ LoadCollection(System::String^ aszCollectionName);
	System::Collections::Generic::List<System::String^>^ GetCollectionList(System::String^ ahszCollectionName);

private:
	CStoreFrontBackEnd* m_StoreFrontBackEnd;

};

