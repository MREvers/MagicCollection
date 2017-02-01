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

	void LoadCollection(System::String^ aszCollectionName);
	System::Collections::Generic::List<System::String^>^ GetCollectionList(System::String^ ahszCollectionName);

private:
	CStoreFrontBackEnd* m_StoreFrontBackEnd;

};

