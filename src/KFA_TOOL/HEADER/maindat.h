#pragma once
#pragma warning(disable : 4996)
#include <string>
#include "func.h"
#include "struct.h"
#include "txt.h"
#include "ptr.h"
#include "bin.h"
#include "switch.h"
#include "mess.h"
#include <openssl/aes.h>
#define MAINDAT_ALLOC 0x100000 * 300// alokasi 300 MB main.dat 
#define MAX_BIN_SIZE  0x80000
unsigned  char KEY[] = { 0x8D, 0x5A, 0xB5, 0x6A, 0xE7, 0xCC, 0xDA, 0xE9, 0x4D, 0x01, 0x4C, 0x43, 0xBE, 0x36, 0xEA, 0x65 };
unsigned  char IV[] = { 0x36, 0xC9, 0x2E, 0x63, 0xA8, 0x7C, 0x05, 0x4B, 0x8C, 0x2A, 0xAB, 0x1A, 0xA8, 0x4F, 0x15, 0xEA };
void MainDatDecrypt(std::string fileName)
{
	AES_KEY DECKEY;
	unsigned  char DECIV[0x10];
	AES_set_decrypt_key((unsigned char*)KEY, 128, &DECKEY);
	std::string outname = fileName;
	ReplaceExt(outname, "dec");
	char* mainEnc;
	char* mainDec;
	int* ptr;
	int fileSize;
	int count;
	int offset;
	int size;
	int id;
	MainDatHdr* maindatHdr;
	mainEnc = FileLoad(fileName, &fileSize);
	mainDec = new char[fileSize];
	memset(mainDec, 0, fileSize);
	memcpy(mainDec, mainEnc, 0x10);
	memcpy(DECIV, IV, 0x10);
	AES_cbc_encrypt((unsigned char*)(mainEnc + 0x10), (unsigned char*)(mainDec + 0x10), 0x120, &DECKEY, DECIV, AES_DECRYPT);
	maindatHdr = reinterpret_cast<MainDatHdr*>(mainDec);
	memcpy(DECIV, IV, 0x10);
	AES_cbc_encrypt((unsigned char*)mainEnc + maindatHdr->sysBinStart, (unsigned char*)mainDec + maindatHdr->sysBinStart, maindatHdr->sysEnd - maindatHdr->sysBinStart, &DECKEY, DECIV, AES_DECRYPT);
	SysBinHdr* sysBinHdr = reinterpret_cast<SysBinHdr*>(mainDec + maindatHdr->sysBinStart);
	char* sysbnd = (mainEnc + sysBinHdr->sysBndOffset);
	memcpy((mainDec + sysBinHdr->sysBndOffset), sysbnd, sysBinHdr->sysBndSize);
	ptr = (int*)(mainDec + sysBinHdr->ptrBinOffset);
	count = *ptr;
	ptr += 4;
	for (size_t i = 0; i < count; i++)
	{
		size = *ptr++;
		id = *ptr++;
		offset = *ptr++;
		ptr++;
		memcpy(DECIV, IV, 0x10);
		AES_cbc_encrypt((unsigned char*)mainEnc + offset, (unsigned char*)mainDec + offset, size, &DECKEY, DECIV, AES_DECRYPT);
		BinHeader* binHdr = reinterpret_cast<BinHeader*>((unsigned char*)mainDec + offset);
		memcpy((mainDec + binHdr->bndOffset), (mainEnc + binHdr->bndOffset), binHdr->bndSize);
	}
	//BOTTOM
	{
		//clr
		{
			memcpy(DECIV, IV, 0x10);
			AES_cbc_encrypt((unsigned char*)mainEnc + maindatHdr->bmpOffset, (unsigned char*)mainDec + maindatHdr->bmpOffset, maindatHdr->bmpSize, &DECKEY, DECIV, AES_DECRYPT);
		}
		//skin
		{
			memcpy(DECIV, IV, 0x10);
			AES_cbc_encrypt((unsigned char*)mainEnc + maindatHdr->skinOffset, (unsigned char*)mainDec + maindatHdr->skinOffset, maindatHdr->skinSize, &DECKEY, DECIV, AES_DECRYPT);
		}
		//sound ptr
		{
			memcpy(DECIV, IV, 0x10);
			AES_cbc_encrypt((unsigned char*)mainEnc + maindatHdr->soundPtrOffset, (unsigned char*)mainDec + maindatHdr->soundPtrOffset, maindatHdr->soundPtrSize, &DECKEY, DECIV, AES_DECRYPT);
		}
		//extract sound
		{
			ptr = (int*)(mainDec + maindatHdr->soundPtrOffset);
			count = *ptr++;
			ptr = (int*)(*ptr + mainDec);
			for (size_t i = 0; i < count; i++)
			{
				int id = *ptr++;
				size = *ptr++;
				offset = *ptr++;
				ptr++;
				memcpy(DECIV, IV, 0x10);
				AES_cbc_encrypt((unsigned char*)(mainEnc + offset), (unsigned char*)(mainDec + offset), size, &DECKEY, DECIV, AES_DECRYPT);
			}
		}
	}
	FileSave(outname, mainDec, fileSize);
	delete[]mainEnc;
	delete[]mainDec;
}
void MainDatEncrypt(std::string fileName)
{
	AES_KEY ENCKEY;
	unsigned  char ENCIV[0x10];
	AES_set_encrypt_key((unsigned char*)KEY, 128, &ENCKEY);
	std::string outName;
	
	char* mainDec;
	unsigned char* buffer = new unsigned char[0x100000 * 20];
	int* ptr;
	int count;
	int offset;
	int size;
	int mainDecSize;
	int id;
	MainDatHdr* maindatHdr;
	mainDec = FileLoad(fileName, &mainDecSize);
	maindatHdr = reinterpret_cast<MainDatHdr*>(mainDec);
	SysBinHdr* sysBinHdr = reinterpret_cast<SysBinHdr*>(mainDec + sizeof(MainDatHdr));
	//encrypt files
	{
		ptr = (int*)(mainDec + sysBinHdr->ptrBinOffset);
		count = *(int*)ptr++;
		ptr = (int*)(*ptr + mainDec);
		for (size_t i = 0; i < count; i++)
		{
			size = *ptr++;
			id = *ptr++;
			offset = *ptr++;
			ptr++;
			memcpy(ENCIV, IV, 0x10);
			AES_cbc_encrypt((unsigned char*)mainDec + offset, buffer, size, &ENCKEY, ENCIV, AES_ENCRYPT);
			memcpy((unsigned char*)mainDec + offset, buffer, size);
		}
	}
	//file on foot
	{
		//clr
		{
			memcpy(ENCIV, IV, 0x10);
			AES_cbc_encrypt((unsigned char*)(mainDec + maindatHdr->bmpOffset), buffer, maindatHdr->bmpSize, &ENCKEY, ENCIV, AES_ENCRYPT);
			memcpy((unsigned char*)(mainDec + maindatHdr->bmpOffset), buffer, maindatHdr->bmpSize);
		}
		//skin
		{
			memcpy(ENCIV, IV, 0x10);
			AES_cbc_encrypt((unsigned char*)(mainDec + maindatHdr->skinOffset), buffer, maindatHdr->skinSize, &ENCKEY, ENCIV, AES_ENCRYPT);
			memcpy((unsigned char*)(mainDec + maindatHdr->skinOffset), buffer, maindatHdr->skinSize);
		}
		//encrypt sound
		{
			ptr = (int*)(mainDec + maindatHdr->soundPtrOffset);
			count = *ptr;
			ptr += 4;;
			for (size_t i = 0; i < count; i++)
			{
				id = *ptr++;
				size = *ptr++;
				offset = *ptr++;
				ptr++;
				memcpy(ENCIV, IV, 0x10);
				AES_cbc_encrypt((unsigned char*)(mainDec + offset), buffer, size, &ENCKEY, ENCIV, AES_ENCRYPT);
				memcpy((unsigned char*)(mainDec + offset), buffer, size);
			}
		}
		//sound ptr
		{
			memcpy(ENCIV, IV, 0x10);
			AES_cbc_encrypt((unsigned char*)(mainDec + maindatHdr->soundPtrOffset), buffer, maindatHdr->soundPtrSize, &ENCKEY, ENCIV, AES_ENCRYPT);
			memcpy((unsigned char*)(mainDec + maindatHdr->soundPtrOffset), buffer, maindatHdr->soundPtrSize);
		}
	}
	memcpy(ENCIV, IV, 0x10);
	AES_cbc_encrypt((unsigned char*)mainDec + maindatHdr->sysBinStart, buffer, maindatHdr->sysEnd - maindatHdr->sysBinStart, &ENCKEY, ENCIV, AES_ENCRYPT);
	memcpy((unsigned char*)mainDec + maindatHdr->sysBinStart, buffer, maindatHdr->sysEnd - maindatHdr->sysBinStart);
	memcpy(ENCIV, IV, 0x10);
	AES_cbc_encrypt((unsigned char*)(mainDec + 0x10), (unsigned char*)buffer, sizeof(MainDatHdr), &ENCKEY, ENCIV, AES_ENCRYPT);
	memcpy(mainDec + 0x10, buffer, sizeof(MainDatHdr) - 0x10);
	delete[]buffer;
	
	outName = std::format("{:s}_new.dat", RemExt(fileName));
	FileSave(outName, mainDec, mainDecSize);
	delete[]mainDec;
}
void MainDatUnpack(std::string maindatName) {
	std::string outF = RemExt(maindatName);
	std::string outName,internalName;
	char* mainDat, * ptSys, * ptBin;
	int* ptr;
	int size, count, index, offset;
	mainDat = FileLoad(maindatName, &size);
	MainDatHdr* maindatHdr = reinterpret_cast<MainDatHdr*>(mainDat);
	ptSys = mainDat + maindatHdr->sysBinStart;
	SysBinHdr* sysBinHdr = reinterpret_cast<SysBinHdr*>(ptSys);
	//MAIN/SYS
	{
		ptBin = mainDat + sysBinHdr->sysBndOffset;
		outName = std::format("{:s}\\_SYS.bnd", outF);
		FileSave(outName, ptBin, sysBinHdr->sysBndSize);
		//SYS/0000.bin
		{
			ptSys = mainDat + sysBinHdr->offset0_text;
			ptBin = ptSys;
			count = *(int*)ptBin;
			ptBin += 4;
			*(int*)ptBin -= (sysBinHdr->offset0_text);
			ptBin += 12;
			for (size_t i = 0; i < count; i++)
			{
				*(int*)ptBin -= (sysBinHdr->offset0_text);
				ptBin += 4;
			}
			size = sysBinHdr->offset1_bin - sysBinHdr->offset0_text;
			outName = std::format("{:s}\\_SYS\\0000.bin", outF);
			FileSave(outName, ptSys, size);
			Bin2Txt(outName);
		}
		//SYS/0001.bin
		{
			ptSys = mainDat + sysBinHdr->offset1_bin;
			size = sysBinHdr->offset2_bin - sysBinHdr->offset1_bin;
			outName = std::format("{:s}\\_SYS\\0001.bin", outF);
			FileSave(outName, ptSys, size);
		}
		//SYS/0002.bin
		{
			ptSys = mainDat + sysBinHdr->offset2_bin;
			size = sysBinHdr->offset3_bin - sysBinHdr->offset2_bin;
			outName = std::format("{:s}\\_SYS\\0002.bin", outF);
			FileSave(outName, ptSys, size);
		}
		//SYS/0003.bin
		{
			ptSys = mainDat + sysBinHdr->offset3_bin;
			size = sysBinHdr->offset4_bin - sysBinHdr->offset3_bin;
			outName = std::format("{:s}\\_SYS\\0003.bin", outF);
			FileSave(outName, ptSys, size);
		}
		//SYS/0004.bin
		{
			ptSys = mainDat + sysBinHdr->offset4_bin;
			size = sysBinHdr->offset5_bin - sysBinHdr->offset4_bin;
			outName = std::format("{:s}\\_SYS\\0004.bin", outF);
			FileSave(outName, ptSys, size);
		}
		//SYS/0005.bin
		{
			ptSys = mainDat + sysBinHdr->offset5_bin;
			ptBin = ptSys;
			count = *(int*)ptBin;
			ptBin += 4;
			*(int*)ptBin -= (sysBinHdr->offset5_bin);
			ptBin += 12;
			for (size_t i = 0; i < count; i++)
			{
				*(int*)ptBin -= (sysBinHdr->offset5_bin);
				*(int*)(mainDat + *(int*)(ptBin)+0x3c + sysBinHdr->offset5_bin) -= (sysBinHdr->offset5_bin);
				ptBin += 4;
			}
			size = sysBinHdr->offset6_bin - sysBinHdr->offset5_bin;
			outName = std::format("{:s}\\_SYS\\0005.bin", outF);
			FileSave(outName, ptSys, size);
		}
		//SYS/0006/
		{
			ptSys = mainDat + sysBinHdr->offset6_bin;
			ptBin = ptSys;
			ptBin += 4;
			*(int*)ptBin -= (sysBinHdr->offset6_bin);
			int off6_0 = *(int*)ptBin;
			ptBin += 4;
			*(int*)ptBin -= (sysBinHdr->offset6_bin);
			int off6_1 = *(int*)ptBin;
			ptBin += 4;
			*(int*)ptBin -= (sysBinHdr->offset6_bin);
			int off6_2 = *(int*)ptBin;
			ptBin += 4;
			//SYS/0006/0000.bin
			{
				ptBin = mainDat + sysBinHdr->offset6_bin + off6_0;
				outName = std::format("{:s}\\_SYS\\0006\\0000.bin", outF);
				FileSave(outName, ptBin, off6_1 - off6_0);
			}
			//SYS/0006/0001/
			{
				ptBin = mainDat + sysBinHdr->offset6_bin + off6_1;
				char* pt6_1 = ptBin;
				int size1_gim = *(int*)ptBin;
				ptBin += 4;
				*(int*)ptBin -= (sysBinHdr->offset6_bin + off6_1);
				int offset1_gim = *(int*)ptBin;
				ptBin += 4;
				count = *(int*)ptBin;
				ptBin += 4;
				*(int*)ptBin -= (sysBinHdr->offset6_bin + off6_1);
				int offset0_bin = *(int*)ptBin;
				ptBin = mainDat + sysBinHdr->offset6_bin + off6_1 + offset0_bin;
				for (size_t i = 0; i < count; i++)
				{
					ptBin += 4;
					*(int*)ptBin -= (sysBinHdr->offset6_bin + off6_1);
					ptBin += 12;
				}
				//SYS/0006/0001/0000.bin
				{
					outName = std::format("{:s}\\_SYS\\0006\\0001\\0000.bin", outF);
					FileSave(outName, pt6_1 + offset0_bin, offset1_gim - offset0_bin);
				}
				//SYS/0006/0001/0001.gim
				{
					outName = std::format("{:s}\\_SYS\\0006\\0001\\0001.gim", outF);
					FileSave(outName, pt6_1 + offset1_gim, size1_gim);
				}
				outName = std::format("{:s}\\_SYS\\0006\\0001.toc", outF);
				FileSave(outName, pt6_1, offset0_bin);
			}
			//SYS/0006/0002.bin
			{
				ptBin = mainDat + sysBinHdr->offset6_bin + off6_2;
				char* pt6_2 = ptBin;
				count = *(int*)ptBin;
				ptBin += 4;
				*(int*)ptBin -= (sysBinHdr->offset6_bin + off6_2);
				ptBin = mainDat + sysBinHdr->offset6_bin + off6_2 + *(int*)ptBin;
				for (size_t i = 0; i < count; i++)
				{
					ptBin += 4;
					*(int*)ptBin -= (sysBinHdr->offset6_bin + off6_2);
					ptBin += 12;
				}
				outName = std::format("{:s}\\_SYS\\0006\\0002.bin", outF);
				FileSave(outName, pt6_2, (sysBinHdr->offset7_bin - sysBinHdr->offset6_bin) - off6_2);
			}
			size = sysBinHdr->offset7_bin - sysBinHdr->offset6_bin;
			outName = std::format("{:s}\\_SYS\\0006.toc", outF);
			FileSave(outName, ptSys, off6_0);
		}
		//SYS/0007.bin
		{
			ptSys = mainDat + sysBinHdr->offset7_bin;
			size = sysBinHdr->ptrBinOffset - sysBinHdr->offset7_bin;
			outName = std::format("{:s}\\_SYS\\0007.bin", outF);
			FileSave(outName, ptSys, size);
		}
		outName = std::format("{:s}\\_SYS.toc", outF);
		FileSave(outName, (char*)sysBinHdr, sysBinHdr->offset0_text - maindatHdr->sysBinStart);
		outName = std::format("{:s}\\_PTR_BIN.toc", outF);
		FileSave(outName, mainDat + sysBinHdr->ptrBinOffset, maindatHdr->sysEnd - sysBinHdr->ptrBinOffset);
	}
	//MAIN/XXXX,bin
	{
		ptr = (int*)(mainDat + sysBinHdr->ptrBinOffset);
		count = *(int*)ptr++;
		ptr = (int*)(*ptr + mainDat);
		for (size_t i = 0; i < count; i++)
		{
			size = *ptr++;
			index = *ptr++;
			offset = *ptr++;
			ptr++;
			ptBin = (mainDat + offset);
			outName = std::format("{:s}\\{:04d}.bin", outF, i);
			UnpackBin(outName, ptBin, offset);
			BinHeader* binHdr = reinterpret_cast<BinHeader*>(ptBin);
			auto c = sjisToWs(binHdr->internalName);
			internalName = switch_name[c];
			ptBin = mainDat + binHdr->bndOffset;
			outName = std::format("{:s}\\{:04d}_{:s}.bnd", outF, i, internalName);
			FileSave(outName, ptBin, binHdr->bndSize);
		}
	}
	//BOTOM
	{
		//clr
		{
			ptBin = mainDat + maindatHdr->bmpOffset;
			outName = std::format("{:s}\\unk.bmp", outF);
			std::cout << std::format("Unpack >>{:s}\\clear\\clear.bmp", outF) << std::endl;
			FileSave(outName, ptBin, maindatHdr->bmpSize);
		}
		//skin
		{
			ptBin = mainDat + maindatHdr->skinOffset;
			outName = std::format("{:s}\\unk.SKN", outF);
			std::cout << std::format("Unpack >>{:s}\\skin.SKN", outF) << std::endl;
			FileSave(outName, ptBin, maindatHdr->skinSize);
		}
		//sound ptr
		{
			ptBin = mainDat + maindatHdr->soundPtrOffset;
			outName = std::format("{:s}\\sound.toc", outF);
			std::cout << std::format("Unpack >>\\sound.toc", outF) << std::endl;
			FileSave(outName, ptBin, maindatHdr->soundPtrSize);
		}
		//extract sound
		{
			ptr = (int*)(mainDat + maindatHdr->soundPtrOffset);
			count = *ptr++;
			ptr = (int*)(*ptr + mainDat);
			for (size_t i = 0; i < count; i++)
			{
				int id = *ptr++;
				size = *ptr++;
				offset = *ptr++;
				ptr++;
				ptBin = mainDat + offset;
				outName = std::format("{:s}\\sound\\{:4d}.at3", outF, id);
				std::cout << std::format("Unpack >>{:s}\\sound\\{:4d}.at3", outF, id) << std::endl;
				FileSave(outName, ptBin, size);
			}
		}
	}
	outName = std::format("{:s}.toc", outF);
	FileSave(outName, mainDat, maindatHdr->sysBinStart);
}
void MainDatRepack(std::string maindatName, bool alignSys = false) {
	std::string inF = RemExt(maindatName);
	std::string inName,internalName;
	int size, count, reference, pos, offset, newOffset, index;
	char *mainDat, * ptMain, * sysBin, * ptSysBin, * ptBin;
	int* ptr;
	inName = std::format("{:s}.toc", inF);
	mainDat = new char[MAINDAT_ALLOC];
	memset(mainDat, 0, MAINDAT_ALLOC);
	BufferLoad(inName, mainDat, &size);
	ptMain = mainDat;
	MainDatHdr* maindatHdr = reinterpret_cast<MainDatHdr*>(mainDat);
	sysBin = mainDat + maindatHdr->sysBinStart;
	ptSysBin = sysBin;
	inName = std::format("{:s}\\_SYS.toc", inF);
	BufferLoad(inName, sysBin, &size);
	SysBinHdr* sysBinHdr = reinterpret_cast<SysBinHdr*>(sysBin);
	if (alignSys)
	{
		while (sysBinHdr->offset0_text % 0x10 != 0)
		{
			sysBinHdr->offset0_text++;
		}
	}
	//SYS
	{
		reference = sysBinHdr->offset0_text - maindatHdr->sysBinStart;
		ptSysBin += reference;
		//SYS/0000.bin
		{
			inName = std::format("{:s}\\_SYS\\0000.txt", inF);
			Txt2Bin(inName);
			inName = std::format("{:s}\\_SYS\\0000.bin", inF);
			BufferLoad(inName, ptSysBin, &size);
			count = *(int*)ptSysBin;
			ptSysBin += 4;
			*(int*)ptSysBin += (maindatHdr->sysBinStart + reference);
			ptSysBin += 12;
			for (size_t i = 0; i < count; i++)
			{
				*(int*)ptSysBin += (maindatHdr->sysBinStart + reference);
				ptSysBin += 4;
			}
			sysBinHdr->offset1_bin = (sysBinHdr->offset0_text + size);
			if (alignSys)
			{
				while (sysBinHdr->offset1_bin % 0x10 != 0)
				{
					sysBinHdr->offset1_bin++;
				}
			}
			ptSysBin = (char*)(mainDat + sysBinHdr->offset1_bin);
		}
		//SYS/0001.bin
		{
			inName = std::format("{:s}\\_SYS\\0001.bin", inF);
			BufferLoad(inName, ptSysBin, &size);
			sysBinHdr->offset2_bin = (sysBinHdr->offset1_bin + size);
			if (alignSys)
			{
				while (sysBinHdr->offset2_bin % 0x10 != 0)
				{
					sysBinHdr->offset2_bin++;
				}
			}
			ptSysBin = (char*)(mainDat + sysBinHdr->offset2_bin);
		}
		//SYS/0002.bin
		{
			inName = std::format("{:s}\\_SYS\\0002.bin", inF);
			BufferLoad(inName, ptSysBin, &size);
			sysBinHdr->offset3_bin = (sysBinHdr->offset2_bin + size);
			if (alignSys)
			{
				while (sysBinHdr->offset3_bin % 0x10 != 0)
				{
					sysBinHdr->offset3_bin++;
				}
			}
			ptSysBin = (char*)(mainDat + sysBinHdr->offset3_bin);
		}
		//SYS/0003.bin
		{
			inName = std::format("{:s}\\_SYS\\0003.bin", inF);
			BufferLoad(inName, ptSysBin, &size);
			sysBinHdr->offset4_bin = (sysBinHdr->offset3_bin + size);
			if (alignSys)
			{
				while (sysBinHdr->offset4_bin % 0x10 != 0)
				{
					sysBinHdr->offset4_bin++;
				}
			}
			ptSysBin = (char*)(mainDat + sysBinHdr->offset4_bin);
		}
		//SYS/0004.bin
		{
			inName = std::format("{:s}\\_SYS\\0004.bin", inF);
			BufferLoad(inName, ptSysBin, &size);
			sysBinHdr->offset5_bin = (sysBinHdr->offset4_bin + size);
			if (alignSys)
			{
				while (sysBinHdr->offset5_bin % 0x10 != 0)
				{
					sysBinHdr->offset5_bin++;
				}
			}
			ptSysBin = (char*)(mainDat + sysBinHdr->offset5_bin);
		}
		//SYS/0005.bin
		{
			inName = std::format("{:s}\\_SYS\\0005.bin", inF);
			BufferLoad(inName, ptSysBin, &size);
			count = *(int*)ptSysBin;
			ptSysBin += 4;
			*(int*)ptSysBin += sysBinHdr->offset5_bin;
			ptSysBin += 12;
			for (size_t i = 0; i < count; i++)
			{
				*(int*)ptSysBin += sysBinHdr->offset5_bin;
				pos = *(int*)ptSysBin;
				*(int*)(mainDat + pos + 0x3c) += sysBinHdr->offset5_bin;
				ptSysBin += 4;
			}
			sysBinHdr->offset6_bin = (sysBinHdr->offset5_bin + size);
			if (alignSys)
			{
				while (sysBinHdr->offset6_bin % 0x10 != 0)
				{
					sysBinHdr->offset6_bin++;
				}
			}
			ptSysBin = (char*)(mainDat + sysBinHdr->offset6_bin);
		}
		//SYS/0006/
		{
			inName = std::format("{:s}\\_SYS\\0006.toc", inF);
			BufferLoad(inName, ptSysBin, &size);
			ptSysBin += 4;
			int* off6_0 = (int*)ptSysBin;
			ptSysBin += 4;
			int* off6_1 = (int*)ptSysBin;
			ptSysBin += 4;
			int* off6_2 = (int*)ptSysBin;
			ptSysBin += 4;
			//SYS/0006/0000.bin
			{
				*off6_0 += sysBinHdr->offset6_bin;
				ptSysBin = (char*)(mainDat + *off6_0);
				inName = std::format("{:s}\\_SYS\\0006\\0000.bin", inF);
				BufferLoad(inName, ptSysBin, &size);
				if (alignSys)
				{
					while (size % 0x10 != 0)
					{
						size++;
					}
				}
				ptSysBin += size;
			}
			//SYS/0006/0001/
			{
				*off6_1 = *off6_0 + size;
				inName = std::format("{:s}\\_SYS\\0006\\0001.toc", inF);
				BufferLoad(inName, ptSysBin, &size);
				char* pt6_1 = ptSysBin;
				int* size1_gim = (int*)ptSysBin;
				ptSysBin += 4;
				int* offset1_gim = (int*)ptSysBin;
				ptSysBin += 4;
				count = *(int*)ptSysBin;
				ptSysBin += 4;
				int* offset0_bin = (int*)ptSysBin;
				*offset0_bin += (int)(pt6_1 - mainDat);
				//SYS/0006/0001/0000.bin
				{
					ptSysBin = mainDat + *offset0_bin;
					inName = std::format("{:s}\\_SYS\\0006\\0001\\0000.bin", inF);
					BufferLoad(inName, ptSysBin, &size);
					for (size_t i = 0; i < count; i++)
					{
						ptSysBin += 4;
						*(int*)ptSysBin += (*off6_1);
						ptSysBin += 12;
					}
					if (alignSys)
					{
						while (size % 0x10 != 0)
						{
							size++;
						}
					}
					ptSysBin += size;
					*offset1_gim = *offset0_bin + size;
				}
				//SYS/0006/0001/0001.gim
				{
					ptSysBin = mainDat + *offset1_gim;
					inName = std::format("{:s}\\_SYS\\0006\\0001\\0001.gim", inF);
					BufferLoad(inName, ptSysBin, &size);
					if (alignSys)
					{
						while (size % 0x10 != 0)
						{
							size++;
						}
					}
					ptSysBin += size;
				}
			}
			//SYS/0006/0002.bin
			{
				*off6_2 = (int)(ptSysBin - mainDat);
				inName = std::format("{:s}\\_SYS\\0006\\0002.bin", inF);
				BufferLoad(inName, ptSysBin, &size);
				char* pt6_2 = ptSysBin;
				count = *(int*)ptSysBin;
				ptSysBin += 4;
				*(int*)ptSysBin += *off6_2;
				ptSysBin = mainDat + *(int*)ptSysBin;
				for (size_t i = 0; i < count; i++)
				{
					ptSysBin += 4;
					*(int*)ptSysBin += *off6_2;
					ptSysBin += 12;
				}
				ptSysBin = pt6_2 + size;
				sysBinHdr->offset7_bin = (int)(ptSysBin - mainDat);
				if (alignSys)
				{
					while (sysBinHdr->offset7_bin % 0x10 != 0)
					{
						sysBinHdr->offset7_bin++;
					}
				}
			}
		}
		//SYS/0007/bin
		{
			inName = std::format("{:s}\\_SYS\\0007.bin", inF);
			BufferLoad(inName, ptSysBin, &size);
			ptSysBin += size;
			sysBinHdr->ptrBinOffset = (sysBinHdr->offset7_bin + size);
			if (alignSys)
			{
				while (sysBinHdr->ptrBinOffset % 0x10 != 0)
				{
					sysBinHdr->ptrBinOffset++;
				}
			}
		}
	}
	sysBinHdr->sysBinSize = (int)((ptSysBin - mainDat) - maindatHdr->sysBinStart);
	//ptr_bin
	{
		inName = std::format("{:s}\\_PTR_BIN.toc", inF);
		BufferLoad(inName, ptSysBin, &size);
		maindatHdr->sysEnd = (int)(ptSysBin - mainDat) + size;
		ptSysBin += 4;
		*(int*)ptSysBin = (sysBinHdr->ptrBinOffset + 0x10);
	}
	//Start 
	ptBin = mainDat + maindatHdr->sysEnd;
	//align
	while ((ptBin - mainDat) % 0x800 != 0)
	{
		ptBin++;
	}
	newOffset = (int)(ptBin - mainDat);
	//Main/XXXX.bin
	{
		ptr = (int*)(mainDat + sysBinHdr->ptrBinOffset);
		count = *(int*)ptr++;
		ptr = (int*)(*ptr + mainDat);
		for (size_t i = 0; i < count; i++)
		{
			inName = std::format("{:s}\\{:04d}.bin", inF, i);
			RepackBin(inName, ptBin, newOffset);
			BinHeader* binHdr = reinterpret_cast<BinHeader*>(ptBin);
			*ptr++ = binHdr->binSize;
			index = *ptr++;
			*ptr++ = newOffset;
			ptr++;
			ptBin += binHdr->binSize;
			while ((ptBin - mainDat) % 0x800 != 0)
			{
				ptBin++;
			}
			newOffset = (int)(ptBin - mainDat);
		}
	}
	//Bnd
	{
		//SYS Bnd
		{
			inName = std::format("{:s}\\_SYS.bnd", inF);
			BufferLoad(inName, ptBin, &size);
			sysBinHdr->sysBndOffset = newOffset;
			sysBinHdr->sysBndSize = size;
			ptBin += size;
			while ((ptBin - mainDat) % 0x800 != 0)
			{
				ptBin++;
			}
			newOffset = (int)(ptBin - mainDat);
		}
		//BINXXXX Bnd
		{
			ptr = (int*)(mainDat + sysBinHdr->ptrBinOffset);
			count = *(int*)ptr++;
			ptr = (int*)(*ptr + mainDat);
			for (size_t i = 0; i < count; i++)
			{
				ptr++;
				ptr++;
				offset = *ptr++;
				ptr++;
				ptBin = (mainDat + offset);
				BinHeader* binHdr = reinterpret_cast<BinHeader*>(ptBin);
				internalName = switch_name[sjisToWs(binHdr->internalName)];
				ptBin = (mainDat + newOffset);
				inName = std::format("{:s}\\{:04d}_{:s}.bnd", inF, i,internalName);
				BufferLoad(inName, ptBin, &size);
				binHdr->bndOffset = newOffset;
				binHdr->bndSize = size;
				ptBin += size;
				while ((ptBin - mainDat) % 0x800 != 0)
				{
					ptBin++;
				}
				newOffset = (int)(ptBin - mainDat);
			}
		}
	}
	//Bottom File
	{
		//bmp
		{
			inName = std::format("{:s}\\unk.bmp", inF);
			BufferLoad(inName, ptBin, &size);
			maindatHdr->bmpOffset = newOffset;
			maindatHdr->bmpSize = size;
			ptBin += size;
			while ((ptBin - mainDat) % 0x800 != 0)
			{
				ptBin++;
			}
			newOffset = (int)(ptBin - mainDat);
		}
		//skin
		{
			inName = std::format("{:s}\\unk.SKN", inF);
			BufferLoad(inName, ptBin, &size);
			maindatHdr->skinOffset = newOffset;
			maindatHdr->skinSize = size;
			ptBin += size;
			while ((ptBin - mainDat) % 0x800 != 0)
			{
				ptBin++;
			}
			newOffset = (int)(ptBin - mainDat);
		}
		//sound ptr
		{
			inName = std::format("{:s}\\sound.toc", inF);
			BufferLoad(inName, ptBin, &size);
			maindatHdr->soundPtrOffset = newOffset;
			maindatHdr->soundPtrSize = size;
			ptBin += size;
			newOffset = (int)(ptBin - mainDat);
		}
		//import sound
		{
			ptr = (int*)(mainDat + maindatHdr->soundPtrOffset);
			count = *ptr++;
			*ptr = (int)(maindatHdr->soundPtrOffset + 0x10);
			ptr = (int*)(*ptr + mainDat);
			for (size_t i = 0; i < count; i++)
			{
				int id = *ptr++;
				inName = std::format("{:s}\\sound\\{:4d}.at3", inF, id);
				BufferLoad(inName, ptBin, &size);
				*ptr++ = size;
				*ptr++ = newOffset;
				ptr++;
				ptBin += size;
				while ((ptBin - mainDat) % 0x10 != 0)
				{
					ptBin++;
				}
				newOffset = (int)(ptBin - mainDat);
			}
		}
	}
	//align
	while ((ptBin - mainDat) % 0x800 != 0)
	{
		ptBin++;
	}
	newOffset = (int)(ptBin - mainDat);
	maindatHdr->mainDatSize = (int)(ptBin - mainDat);
	inName = std::format("{:s}.dec", inF);
	FileSave(inName, mainDat, maindatHdr->mainDatSize);
	delete[]mainDat;
}

