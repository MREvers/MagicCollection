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
public ref class ServerClientInterface
{
public:
	ServerClientInterface();
	~ServerClientInterface();
	// Make the client calculate differences
	
	// These also takes "Fancy Cards" i.e. "Seeker of the Way { Set="blah" }
	void AddItem(System::String^ aszCollectionName, System::String^ aszCardName);
	void RemoveItem(System::String^ aszCollectionName, System::String^ aszCardName);
	MCopyObject^ ConvertItemToCopyObject(System::String^ ahszCard);

	// Eventually Add file name arg.
	void SaveCollection(System::String^ aszCollectionName);
	// Returns the collection name
	System::String^ LoadCollection(System::String^ aszCollectionName);
	System::Collections::Generic::List<System::String^>^ GetCollectionList(System::String^ ahszCollectionName);
	System::Collections::Generic::List<System::String^>^ GetLoadedCollections();
	System::Collections::Generic::List<System::String^>^ GetAllCardsStartingWith(System::String^ ahszText);


private:
	CStoreFrontBackEnd* m_StoreFrontBackEnd;

};

