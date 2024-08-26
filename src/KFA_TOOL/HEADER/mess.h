#pragma once
#include <string>
#include "func.h"
#include "txt.h"
#include "struct.h"
void MessUnpack(std::string fileName, char* bin)
{
	std::string outF = RemExt(fileName);
	std::string outName;
	//int size;
	int size0, size1, size2, size3;
	//char* bin = FileLoad(fileName, &size);
	MessBinHdr* binHdr = reinterpret_cast<MessBinHdr*>(bin);
	char* ptBin = bin;
	size0 = binHdr->offset1 - binHdr->offset0;
	size1 = binHdr->offset2 - binHdr->offset1;
	if (binHdr->offset1 == -1)
	{
		size0 = binHdr->offset2 - binHdr->offset1;
		size1 = 0;
	}
	size2 = binHdr->offset3 - binHdr->offset2;
	size3 = binHdr->size - binHdr->offset3;
	//00
	{
		ptBin = bin + binHdr->offset0;
		outName = std::format("{:s}/0000.bin", outF);
		FileSave(outName, ptBin, size0);
		Bin2Txt(outName);
	}
	//01
	if (binHdr->offset1 != -1) {
		ptBin = bin + binHdr->offset1;
		outName = std::format("{:s}/0001.bin", outF);
		FileSave(outName, ptBin, size1);
	}
	//02
	{
		ptBin = bin + binHdr->offset2;
		outName = std::format("{:s}/0002.bin", outF);
		FileSave(outName, ptBin, size2);
	}
	//03
	{
		ptBin = bin + binHdr->offset3;
		outName = std::format("{:s}/0003.bin", outF);
		FileSave(outName, ptBin, size3);
	}
	outName = std::format("{:s}.toc", outF);
	FileSave(outName, bin, sizeof(MessBinHdr));
}
void MessRepack(std::string fileName, char* bin)
{
	std::string inF = RemExt(fileName);
	std::string inName;
	int size;
	inName = std::format("{:s}.toc", inF);
	//char* buffer = FileLoad(inName, &size);
	//memcpy(bin, buffer, size);
	BufferLoad(inName, bin, &size);
	MessBinHdr* binHdr = reinterpret_cast<MessBinHdr*>(bin);
	char* ptBin = bin;
	ptBin = bin + sizeof(MessBinHdr);
	//00
	{
		binHdr->offset0 = (int)(ptBin - bin);
		inName = std::format("{:s}/0000.txt", inF);
		Txt2Bin(inName);
		inName = std::format("{:s}/0000.bin", inF);
		//buffer = FileLoad(inName, &size);
		//memcpy(ptBin, buffer, size);
		BufferLoad(inName, ptBin, &size);
		ptBin += size;
	}
	//01
	if (binHdr->offset1 != -1) {
		binHdr->offset1 = (int)(ptBin - bin);
		inName = std::format("{:s}/0001.bin", inF);
		//buffer = FileLoad(inName, &size);
		//memcpy(ptBin, buffer, size);
		BufferLoad(inName, ptBin, &size);
		ptBin += size;
	}
	//02
	{
		binHdr->offset2 = (int)(ptBin - bin);
		inName = std::format("{:s}/0002.bin", inF);
		//buffer = FileLoad(inName, &size);
		//memcpy(ptBin, buffer, size);
		BufferLoad(inName, ptBin, &size);
		ptBin += size;
	}
	//03
	{
		binHdr->offset3 = (int)(ptBin - bin);
		inName = std::format("{:s}/0003.bin", inF);
		//buffer = FileLoad(inName, &size);
		//memcpy(ptBin, buffer, size);
		BufferLoad(inName, ptBin, &size);
		ptBin += size;
	}
	binHdr->size = (int)(ptBin - bin);
	//delete[]buffer;
}

