#pragma once
#include <string>
#include "func.h"
void Bin2Txt(std::string fileName)
{
	std::cout << std::format("Decode >>{:s}", fileName) << std::endl;
	int fileSize;
	char* bin = FileLoad(fileName, &fileSize);
	int* pt = (int*)bin;
	char* pos;
	std::string jis;
	std::ofstream txt;
	ReplaceExt(fileName, "txt");
	txt.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
	int count = *pt;
	pt += 4;
	txt << std::format("[{:04d}]", count) << std::endl;
	txt << "********************" << std::endl;
	for (size_t i = 0; i < count; i++)
	{
		pos = bin + *pt++;
		txt << std::format("[{:04d}]", i) << std::endl;
		jis = (char*)pos;
		txt << sjisToUtf8(jis) << std::endl;
		txt << "------------------------------" << std::endl;
	}
	delete[]bin;
	txt.flush();
	txt.close();
}
void Txt2Bin(std::string fileName, bool seekSame = false)
{
	std::cout << std::format("Encode >>{:s}", fileName) << std::endl;
	std::ifstream txt;
	std::string line;
	std::string utf8;
	std::string jis;
	int fileSize;
	char* bin = new char[0x100000];
	memset(bin, 0, 0x100000);
	int* pt = (int*)bin;
	char* pos;
	int index, count;
	int x = 0;
	txt.open(fileName, std::ios::in);
	ReplaceExt(fileName, "bin");
	std::vector<std::string> list;
	while (getline(txt, line))
	{
		list.push_back(line);;
	}
	count = std::stoi(list[x].substr(1, 4));
	x += 2;
	if (count == 0)
	{
		*(int*)(bin + 4) = -1;
		FileSave(fileName, bin, 0x10);
		delete[]bin;
		return;
	}
	int ptSize = ((count + 4 + (4 - (count % 4))) * 4);
	*pt++ = count;
	*pt++ = 0x10;
	pt += 2;
	pos = (char*)pt;
	pos += (count * 4);
	while ((pos - bin) % 0x10 != 0)
	{
		pos++;
	}
	for (size_t i = 0; i < count; i++)
	{
		try
		{
			index = std::stoi(list[x++].substr(1, 4));
		}
		catch (const std::exception&)
		{
			std::cout << std::format("{:s}\nBroken index string at line {:d}", fileName, x) << std::endl;

		}	
		utf8 = "";
		while (!list[x].starts_with("----") && !list[x].ends_with("----"))
		{
			utf8 += list[x++] + "\r\n";
		}
		jis = utf8ToSjis(utf8.substr(0, utf8.length() - 2));
		size_t len = jis.length();
		*pt++ = (int)(pos - bin);
		memcpy(pos, jis.c_str(), len);
		pos += len + 1;
		x++;
	}
	while ((pos - bin) % 0x10 != 0)
	{
		pos++;
	}
	fileSize = (int)(pos - bin);
	FileSave(fileName, bin, fileSize);
	txt.close();
	delete[]bin;
}

