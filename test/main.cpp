
/********************************************************************************
 * Date: 2019-1-26																*
 * By Calix Fei																	*
 * Copyright© 2019, 1952 NanJing University of aeronaustics and atronaustics.	*
 * All rights reserved.															*
 * This program is used for formatting SQL.										*
 ********************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <afx.h>
#include <iostream>

VOID DataDefine();
VOID LineFeed(CString &Buffer);
VOID WordReplace(CString &Buffer);
VOID Insert(CString &String, CString &word, CString location, int num);
VOID StrToWordArray(CStringArray &wordArray, CString &Buffer);
BOOL CopyToClipboard(const char* pszData, const int nDataLen);
BOOL GetTextFromClipboard(CString &Buffer);
BOOL In(CStringArray &list, CString &word);

CStringArray foreFeedSearchWord;		//前换行查找词
CStringArray lattFeedSearchWord;		//后换行查找词
CStringArray SearchWord;				//所有查找词
CStringArray ReplaceWord;				//替换词
const int fileLen = 100000;				//设置文件内容最大长度

const CString SPACE = " ";				//空格
const CString ENTER = "\n";				//回车


int main()
{
	
	//TCHAR* pszFileNameRead  = _T("C:/Users/Nightingale/Desktop/cope_doc.txt");					//读文件路径
	//TCHAR* pszFileNameWrite = _T("C:/Users/Nightingale/Desktop/ok_doc.sql");						//写文件路径

	CStdioFile myFile;																				//文件对象
	CString Buffer;																					//文件内容
	
	CFileException fileException;																	//文件异常
	setlocale(LC_CTYPE, ("chs"));																	//中文支持
	DataDefine();																					//数据定义
	
	
	/*
	if (!myFile.Open(pszFileNameRead, CFile::typeText | CFile::modeReadWrite, &fileException))		//打开文件	
	{
		TRACE("Can't open file %s, error = %u\n", pszFileNameRead, fileException.m_cause);
	}
	else
	{
		char Buf[fileLen]="";																		//读取文件，存入Buffer
		myFile.Read(Buf, fileLen);
		Buffer = Buf;
		
		LineFeed(Buffer);																			//换行
		WordReplace(Buffer);																		//替换
		//printf("%S", Buffer);	
		//std::wcout << Buffer.GetBuffer();
	}
	
	//读完清空
	myFile.SetLength(0);
	myFile.Close();
	*/

	
	if (!GetTextFromClipboard(Buffer))//读剪贴板
	{
		TRACE("Can't open ClipBoard");
	}
	else
	{
		WordReplace(Buffer);																		//替换
		LineFeed(Buffer);																			//换行
		printf("%S", Buffer);
		//std::wcout << Buffer.GetBuffer();
	}	

	//显示文件
	/*
	if (!myFile.Open(pszFileNameWrite, CFile::modeCreate | CFile::modeWrite, &fileException))
	{
		TRACE("Can't open file %s, error = %u\n", pszFileNameWrite, fileException.m_cause);
	}
	else
	{
		myFile.WriteString(Buffer);		//写文件									
	}

	
	myFile.Close();						//关闭文件

	//显示文件
	if(system("E:/feicaixian/SublimeText3/subl.exe C:/Users/Nightingale/Desktop/ok_doc.sql"))
		MessageBox(NULL, _T("请在程序源代码中配置subl路径"), _T("提示"), MB_OK);
	*/

	//复制结果到剪贴板
	//char temp[fileLen];
	USES_CONVERSION;
	std::string str(W2A(Buffer));
	//::wsprintfA(temp, "%ls", (LPCTSTR)Buffer);//将一系列的字符和数值输入到缓冲区，最大支持1024字节。
	const char * pszData = str.c_str();
	if (!CopyToClipboard(pszData, fileLen))
		MessageBox(NULL, _T("复制到剪贴板出现错误"), _T("提示"), MB_OK);

	return 0;
}


//自动换行
VOID LineFeed(CString &Buffer)
{
	int m_index = 0;																		//当前查找到的位置
	int m_index_pre = 0;																	//上次查找到的位置
	int tabNums = 0;																		//Tab缩进个数
	
	CStringArray wordArray;																	//单词数组
	CString temp;

	StrToWordArray(wordArray, Buffer);														//将字符串转换为单词数组
	Buffer = "";
	for (int i = 0; i < wordArray.GetSize(); i++)
	{
		temp = wordArray.GetAt(i);
		if (In(SearchWord, temp))
		{
			if (In(foreFeedSearchWord, temp))
			{
				Insert(Buffer, temp, "LEFT",tabNums);
				if (temp == ")")
					tabNums--;
			}
			else {
				if (temp == ",")
				{
					Insert(Buffer, temp, "RIGHT", 1);
				}
				else if (temp == "(")
				{
					tabNums++;
					Insert(Buffer, temp, "RIGHT", tabNums);
				}
			}
		}
		else if (Buffer=="")
			Buffer = Buffer + temp;
		else
			Buffer = Buffer + " " + temp;
	}
}

VOID StrToWordArray(CStringArray &wordArray,CString &Buffer)
{
	int length = 0;
	int start;

	for (int i = 0; i < Buffer.GetLength(); i++)
	{
		if (Buffer[i] != ' ' && Buffer[i] != '\0' && Buffer[i] != '\n')
		{
			start = i;
			if (Buffer[i]== ',' || Buffer[i] == '(' || Buffer[i] == ')')
			{
				wordArray.Add(Buffer.Mid(start, 1));
				continue;
			}
			while ((Buffer[i] != ' ') && (Buffer[i] != ')') && (Buffer[i] != ',') && Buffer[i] != '\n'&& Buffer[i] != '\0')
			{
				i++;
				length++;
			}
			i--;
			wordArray.Add(Buffer.Mid(start, length));
			length = 0;
		}
	}
}

BOOL In(CStringArray &list, CString &word)
{
	int i;
	for (i = 0; i < list.GetSize(); i++)
	{
		if (word==list.GetAt(i))
		{
			return true;
			//break;
		}
	}
	if (i== list.GetSize())
	{
		return false;
	}
}

//单词替换
VOID WordReplace(CString &Buffer)
{
	int m_index = 0;																	//当前查找到的位置
	int m_index_pre = 0;																//上次查找到的位置
	CString BufferTemp = Buffer;
	CString Word;
	BufferTemp.MakeUpper();																//将文件内容复制一份，进行大写转换，因为CString的find函数区分大小写
	for (int i = 0; i < ReplaceWord.GetSize(); i++)										//遍历SearchWord中的所有查找词
	{
		m_index = BufferTemp.Find(ReplaceWord.GetAt(i));								//从0开始检索，检索不到则返回 - 1

		if (m_index != -1)																//找到指定字符才能插入。
		{
			Word = Buffer.Mid(m_index, ReplaceWord.GetAt(i).GetLength());
			Buffer.Replace(Word, ReplaceWord.GetAt(i));
		}
		m_index = 0;																	//m_index复位，上一个查找词查完时m_index=-1
	}
}

VOID Insert(CString &String, CString &word, CString location, int num)
{
	switch (num)
	{
	case 0:
		if (location="LEFT")
			String = String + "\n" + word;
		else
			String=String + word +"\n";
		break;
	case 1:
		if (location = "LEFT")
			String = String + "\n\t" + word;
		else
			String = String + word + "\n\t";
		break;
	case 2:
		if (location = "LEFT")
			String = String + "\n\t\t" + word;
		else
			String = String + word + "\n\t\t";
		break;
	case 3:
		if (location = "LEFT")
			String = String + "\n\t\t\t" + word;
		else
			String = String + word + "\n\t\t\t";
		break;
	default:
		break;
	}
}

//数据定义
VOID DataDefine()
{
	foreFeedSearchWord.Add("FROM");													//定义前换行查找词，加空格以全字匹配
	foreFeedSearchWord.Add("ON");
	foreFeedSearchWord.Add("ORDER");
	foreFeedSearchWord.Add("AND");
	foreFeedSearchWord.Add("WHERE");
	foreFeedSearchWord.Add(")");

	SearchWord.Add(",");															//全部查找词
	SearchWord.Add("(");
	SearchWord.Add("FROM");
	SearchWord.Add("ON");
	SearchWord.Add("ORDER");
	SearchWord.Add("AND");
	SearchWord.Add("WHERE");
	SearchWord.Add(")");

	ReplaceWord.Add("FROM");														//定义替换词
	ReplaceWord.Add("SELECT");
	ReplaceWord.Add("ON");
	ReplaceWord.Add("ORDER");
	ReplaceWord.Add("AND");
	ReplaceWord.Add("WHERE");
}

// 复制数据到剪贴板
BOOL CopyToClipboard(const char* pszData, const int nDataLen)
{
	if (::OpenClipboard(NULL))
	{
		::EmptyClipboard();
		HGLOBAL clipbuffer;
		char *buffer;
		clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, nDataLen + 1);
		buffer = (char *)::GlobalLock(clipbuffer);
		strcpy(buffer, pszData);
		::GlobalUnlock(clipbuffer);
		::SetClipboardData(CF_TEXT, clipbuffer);
		::CloseClipboard();
		return TRUE;
	}
	return FALSE;
}


// 从剪切板中取得数据
BOOL GetTextFromClipboard(CString &Buffer)
{
	if (::OpenClipboard(NULL))
	{
		//获得剪贴板数据
		HANDLE hClip = GetClipboardData(CF_TEXT);
		if (NULL != hClip)
		{
			char* lpStr = (char*)::GlobalLock(hClip);
			if (NULL != lpStr)
			{
				::GlobalUnlock(hClip);
				Buffer = lpStr;
			}
		}
		::CloseClipboard();

		return TRUE;

	}
	return FALSE;
}