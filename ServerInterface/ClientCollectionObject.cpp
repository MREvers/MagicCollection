#include "ClientCollectionObject.h"



ClientCollectionObject::ClientCollectionObject(CollectionObject* aoColO)
{
	m_ColObject = aoColO;
}



System::String^ ClientCollectionObject::GetAttribute(System::String^ ahszAttr)
{
	std::string szAttr = msclr::interop::marshal_as<std::string>(ahszAttr);
	std::string szVal = m_ColObject->GetAttribute(szAttr);

	return gcnew System::String(szVal.c_str());
}

System::Collections::Generic::List<System::String^>^ ClientCollectionObject::GetAttributes()
{
	System::Collections::Generic::List<System::String^>^ hlstRetval = gcnew System::Collections::Generic::List<System::String^>();

	std::map<std::string, std::string>::iterator iter_attrs = m_ColObject->GetAttributesMap().begin();
	for (; iter_attrs != m_ColObject->GetAttributesMap().end(); ++iter_attrs)
	{
		hlstRetval->Add(gcnew System::String(iter_attrs->first.c_str()));
	}

	return hlstRetval;
}