#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

class ListHelper
{
public:
	ListHelper();
	~ListHelper();

 
	std::vector<std::pair<std::string, std::string>>
		ConvertMapToList(std::map<std::string, std::string>  aMap);

	bool CompareKeyValPairList(std::vector<std::pair<std::string, std::string>> alstFirst,
		std::vector<std::pair<std::string, std::string>> alstSecond);

	template<typename T>
	int List_Find(T aFind, std::vector<T>& alstFindList)
	{
		std::vector<T>::iterator iter_list = alstFindList.begin();
		int index = 0;
		for (; iter_list != alstFindList.end(); iter_list++)
		{
			if (*iter_list == aFind)
			{
				return index;
			}
			index++;
		}
		return -1;
	}

	template<class T, class R>
	int List_Find(T& aiFind, std::vector<R>& alstFindList, std::function<T(R)> afuncExtractor)
	{
		std::vector<R>::iterator iter_list = alstFindList.begin();
		int index = 0;
		for (; iter_list != alstFindList.end(); iter_list++)
		{
			T tRetval = afuncExtractor(*iter_list);
			if (aiFind == tRetval)
			{
				return index;
			}
			index++;
		}
		return -1;
	}

	template<class T>
		void List_Insert(T& aInsert, std::vector<T>& alstInsertList, std::function<int(T, T)> afuncComparer)
	{
		std::vector<T>::iterator iter = alstInsertList.begin();
		for (; iter != alstInsertList.end(); ++iter)
		{
			int iCmp = afuncComparer(aInsert, *iter);
			if (iCmp >= 0) { break; }
		}

		alstInsertList.insert(iter, aInsert);
	}

	static ListHelper* Instance();
	
private:
	static ListHelper* ListHelper::ms_Instance;
};

