#pragma once
#include "stdafx.h"

class CChineseCode{
public:

	static void UTF_8ToUnicode(wchar_t* pOut,char *pText);  // 把UTF-8转换成Unicode
	static void UnicodeToUTF_8(char* pOut,wchar_t* pText);  //Unicode 转换成UTF-8
	static void UnicodeToGB2312(char* pOut,wchar_t uData);  // 把Unicode 转换成 GB2312 
	static void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);// GB2312 转换成　Unicode
	static void GB2312ToUTF_8(string& pOut,char *pText, int pLen);//GB2312 转为 UTF-8
	static void UTF_8ToGB2312(string &pOut, char *pText, int pLen);//UTF-8 转为 GB2312
};

class CCSWordFilter
{
public:
	static CCSWordFilter& GetWordFilterInstance(){
		static CCSWordFilter lCCSWordFilter;
		return lCCSWordFilter;
	}

	CCSWordFilter() {}

	~CCSWordFilter() 
	{
		Clean(&m_Filter);
	}

	void AddWord(const char* word)
	{
		int len = (int)strlen(word);
		Filter* filter = &m_Filter;

		for (int i = 0; i < len; i++)
		{
			unsigned char c = word[i];
			if (i == len - 1)
			{
				filter->m_NodeArray[c].m_Flag |= FilterNode::NODE_IS_END;
				break;
			}
			else
			{
				filter->m_NodeArray[c].m_Flag |= FilterNode::NODE_HAS_NEXT;
			}

			if (filter->m_NodeArray[c].m_NextFilter == NULL)
			{
				Filter* tmpFilter = new (Filter)();
				filter->m_NodeArray[c].m_NextFilter = tmpFilter;
			}

			filter = (Filter *)filter->m_NodeArray[c].m_NextFilter;
		}
	}

	void AddWords(const std::vector<std::string>& wordList)
	{
		for (std::vector<std::string>::const_iterator it = wordList.begin();
			it != wordList.end(); it++)
		{
			AddWord(it->c_str());
		}

		return;
	}

	int Check(const char* str)
	{
		Filter* filter = NULL;
		for (int i = 0; i < (int)strlen(str) - 1; i++)
		{
			filter = &m_Filter;
			for (int j = i; j < strlen(str); j++)
			{
				unsigned char c = str[j]; 
				if ((c >= 'A' && c <= 'Z'))
				{
					c += 32;
				}

				if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_NULL)
				{
					break;
				}
				else if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_IS_END)
				{
					return i;
				}
				else // NODE_HAS_NEXT
				{
					filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
				}
			}
		}
		return -1;
	}

	void CheckAndModify(char* str, const char replace = '*')
	{
		Filter* filter = NULL;
		for (int i = 0; i < (int)strlen(str) - 1; i++)
		{
			filter = &m_Filter;
			for (int j = i; j < strlen(str); j++)
			{
				unsigned char c = str[j]; 
				if ((c >= 'A' && c <= 'Z'))
				{
					c += 32;
				}

				if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_NULL)
				{
					break;
				}
				else if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_IS_END)
				{
					for (int k = i; k <= j; k++)
					{
						str[k] = replace;
					}

					if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_HAS_NEXT)
					{
						filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
					}
					else
					{
						continue;
					}
				}
				else // NODE_HAS_NEXT
				{
					filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
				}
			}
		}
	}

	void CheckAndModify(std::string& str, const char replace = '*')
	{
		Filter* filter = NULL;
		string tempstr = str;
		for (INT32 i = 0; i < tempstr.length(); i ++){
			if ('A' <=tempstr[i] && tempstr[i] <= 'Z'){
				tempstr[i] += 32;
			}
		}

		for (int i = 0; i < (int)tempstr.size() - 1; i++)
		{
			filter = &m_Filter;
			for (int j = i; j < tempstr.size(); j++)
			{
				unsigned char c = tempstr[j]; 
				if (filter->m_NodeArray[c].m_Flag == FilterNode::NODE_IS_NULL)
				{
					break;
				}
				else if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_IS_END)
				{
					int numDel =0;
					if (0 < tempstr[j]){
						numDel = j - i + 1;
					}
					else{
						numDel = (j - i + 1) / 3;
					}
					
					for (int k = i; k < numDel + i; ++k){
						tempstr[k] = replace;
					}
// 					if (0 > str[j])
// 					{
// 						str = str.erase(numDel+i, j-numDel-i+1);
// 					}

					if (filter->m_NodeArray[c].m_Flag & FilterNode::NODE_HAS_NEXT)
					{
						filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
					}
					else
					{
						continue;
					}
				}
					else // NODE_HAS_NEXT
				{
					filter = (Filter*)filter->m_NodeArray[c].m_NextFilter;
				}
			}
		}

		for (INT32 i = 0; '\0' != tempstr[i]; i++){
			if ('*' == tempstr[i]){
				if (0 > str[i]){
					str.erase(i, 2); 
					tempstr.erase(i, 2);
				}
				str[i] = '*';
			}
		}

	}

private:
	//friend class CCSUser;

	struct FilterNode
	{
		char m_Flag;
		void* m_NextFilter;

		enum Flag
		{
			NODE_IS_NULL          = 0x00,
			NODE_HAS_NEXT         = 0x01,
			NODE_IS_END           = 0x10,
		};
		FilterNode() : m_Flag(NODE_IS_NULL), m_NextFilter(NULL) {}
	};

	struct Filter
	{
		FilterNode m_NodeArray[256];
	} m_Filter;

	void Clean(Filter* filter){
		for (int i = 0; i < 256; i++){
			if (filter->m_NodeArray[i].m_NextFilter){
				Clean((Filter *)filter->m_NodeArray[i].m_NextFilter);
				delete ((Filter*)filter->m_NodeArray[i].m_NextFilter);
			}
		}
	}
};

