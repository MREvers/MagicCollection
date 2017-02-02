#pragma once
#include <msclr\marshal_cppstd.h>
#include "../MagicCollection/CollectionObject.h"
ref class ClientCollectionObject
{
public:
	ClientCollectionObject(CollectionObject* aoColObject);

	System::String^ GetAttribute(System::String^ ahszAttr);
	System::Collections::Generic::List<System::String^>^ GetAttributes();
private:
	CollectionObject* m_ColObject;
};

