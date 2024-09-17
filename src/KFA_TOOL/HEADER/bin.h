#pragma once
#include "func.h"
#include "string.h"
#include "struct.h"
#include "switch.h"
#include "mess.h"
void UnpackBin(std::string binName, char* bin, int ref,std::string internalName) {
	std::string outF = RemExt(binName);
	std::string outName;
	int size, count;
	char* ptBin = bin;
	BinHeader* binHdr = reinterpret_cast<BinHeader*>(bin);
	binHdr->offset0_bin -= ref;
	binHdr->offset1_bin -= ref;
	binHdr->offset2_mess -= ref;
	binHdr->offset3_img -= ref;
	int bin0Ref = binHdr->offset0_bin + ref;
	int bin1Ref = binHdr->offset1_bin + ref;
	int mess2Ref = binHdr->offset2_mess + ref;
	int img3Ref = binHdr->offset3_img + ref;
	//BIN{idx}/0000.bin tidak di unpack untuk sekarang
	{
		ptBin = bin + binHdr->offset0_bin;
		ResetBin0(ptBin, bin0Ref);
		size = binHdr->offset1_bin - binHdr->offset0_bin;
		outName = std::format("{:s}_{:s}\\0000.bin", outF, internalName);
		FileSave(outName, ptBin, size);
	}
	//BIN{idx}/0001.bin
	{
		ptBin = bin + binHdr->offset1_bin;
		ResetBin1(ptBin, bin1Ref, binHdr->count1_bin);
		size = binHdr->offset2_mess - binHdr->offset1_bin;
		outName = std::format("{:s}_{:s}\\0001.bin", outF, internalName);
		FileSave(outName, ptBin, size);
	}
	//BIN{idx}/0002.mess
	{
		ptBin = bin + binHdr->offset2_mess;
		char* ptptBin = ptBin;
		for (size_t i = 0; i < binHdr->count2_mess; i++)
		{
			*(int*)ptptBin -= mess2Ref;
			char* mess = ptBin + *(int*)ptptBin;
			ResetMesBin(mess, mess2Ref + *(int*)ptptBin, &size);
			outName = std::format("{:s}_{:s}\\mess\\{:04d}.bin", outF, internalName, i);
			MessUnpack(outName, mess);
			ptptBin += 4;
		}
		size = binHdr->offset3_img - binHdr->offset2_mess;
		outName = std::format("{:s}_{:s}\\mess.toc", outF, internalName);
		FileSave(outName, ptBin, *(int*)ptBin);
	}
	//BIN{idx}/0003.Img
	{
		std::string extension;
		ptBin = bin + binHdr->offset3_img;
		char* ptptBin = ptBin;
		int type;
		int offset;
		short width;
		short height;
		char* img;
		count = (*(int*)(ptptBin + 8) - img3Ref) / 0x10;
		for (size_t i = 0; i < count; i++)
		{
			type = *(int*)ptptBin;
			ptptBin += 4;
			size = *(int*)ptptBin;
			ptptBin += 4;
			*(int*)ptptBin -= img3Ref;
			offset = *(int*)ptptBin;
			ptptBin += 4;
			width = *(short*)ptptBin;
			ptptBin += 2;
			height = *(short*)ptptBin;
			ptptBin += 2;
			switch (type)
			{
			case 0:
				extension = "jpg";
				break;
			case 1:
				extension = "none";
				break;
			case 2:
				extension = "gim";
				break;
			case 3:
				extension = "bmp";
				break;
			default:
				extension = "none";
				break;
			}
			img = (ptBin + offset);
			outName = std::format("{:s}_{:s}\\img\\{:04d}.{:s}", outF, internalName, i, extension);
			FileSave(outName, img, size);
		}
		size = count * 0x10;
		outName = std::format("{:s}_{:s}\\img.toc", outF, internalName);
		FileSave(outName, ptBin, size);
	}
	outName = std::format("{:s}.toc", outF);
	FileSave(outName, (char*)binHdr, sizeof(BinHeader));
}
void RepackBin(std::string binName, char* bin, int ref, std::string internalName) {
	std::string inF = RemExt(binName);
	std::string inName;
	int size, count;
	inName = std::format("{:s}.toc", inF);
	BufferLoad(inName, bin, &size);
	BinHeader* binHdr = reinterpret_cast<BinHeader*>(bin);
	binHdr->offset0_bin = size;
	char* ptBin = (bin + binHdr->offset0_bin);
	//BIN{idx}/0000.bin tidak di unpack untuk sekarang
	{
		inName = std::format("{:s}_{:s}\\0000.bin", inF, internalName);
		binHdr->offset0_bin += ref;
		BufferLoad(inName, ptBin, &size);
		SetBin0(ptBin, binHdr->offset0_bin);
		ptBin += size;
		binHdr->offset1_bin = (int)(ptBin - bin);
	}
	//BIN{idx}/0001.bin
	{
		inName = std::format("{:s}_{:s}\\0001.bin", inF, internalName);
		binHdr->offset1_bin += ref;
		BufferLoad(inName, ptBin, &size);
		SetBin1(ptBin, binHdr->offset1_bin, binHdr->count1_bin);
		ptBin += size;
		binHdr->offset2_mess = (int)(ptBin - bin);
	}
	//BIN{idx}/0002.bin
	{
		inName = std::format("{:s}_{:s}\\mess.toc", inF, internalName);
		binHdr->offset2_mess += ref;
		BufferLoad(inName, ptBin, &size);
		char* ptptBin = ptBin;
		char* mess = ptBin + size;
		for (size_t i = 0; i < binHdr->count2_mess; i++)
		{
			inName = std::format("{:s}_{:s}\\mess\\{:04d}.bin", inF, internalName, i);
			*(int*)ptptBin = (int)(mess - bin) + ref;
			MessRepack(inName, mess);
			SetMesBin(mess, (int)(mess - bin) + ref, &size);
			mess += size;
			ptptBin += 4;
		}
		ptBin = mess;
		binHdr->offset3_img = (int)(ptBin - bin);
	}
	//BIN{idx}/0003.Img
	{
		inName = std::format("{:s}_{:s}\\img.toc", inF, internalName);
		binHdr->offset3_img += ref;
		BufferLoad(inName, ptBin, &size);
		std::string extension;
		char* ptptBin = ptBin;
		int type;
		short width;
		short height;
		char* img;
		count = size / 0x10;
		img = ptptBin + size;
		for (size_t i = 0; i < count; i++)
		{
			type = *(int*)ptptBin;
			ptptBin += 4;
			switch (type)
			{
			case 0:
				extension = "jpg";
				break;
			case 1:
				extension = "none";
				break;
			case 2:
				extension = "gim";
				break;
			case 3:
				extension = "bmp";
				break;
			default:
				extension = "none";
				break;
			}
			inName = std::format("{:s}_{:s}\\img\\{:04d}.{:s}", inF, internalName, i, extension);
			BufferLoad(inName, img, &size);
			*(int*)ptptBin = size;
			ptptBin += 4;
			*(int*)ptptBin = (int)(img - bin) + ref;
			ptptBin += 4;
			width = *(short*)ptptBin;
			ptptBin += 2;
			height = *(short*)ptptBin;
			ptptBin += 2;
			
			img += size;
			while ((img - ptBin) % 0x10 != 0)
			{
				*img++ = 0;
			}
		}
		ptBin = img;
	}
	binHdr->binSize = (int)(ptBin - bin);
}

