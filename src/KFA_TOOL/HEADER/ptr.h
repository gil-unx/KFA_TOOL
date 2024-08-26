#pragma once
#include "struct.h"
//--------------------------------------------------------
void ResetBin0(char* bin, int ref)
{
	char* ptBin = bin;
	int count = *(int*)ptBin;
	if (count > 0)
	{
		ptBin += 4;
		*(int*)ptBin -= ref;
		ptBin = *(int*)ptBin + bin;
		for (size_t i = 0; i < count; i++)
		{
			*(int*)(ptBin) -= ref;
			*(int*)(bin + *(int*)(ptBin)+0x3c) -= ref;
			ptBin += 4;
		}
	}
}
void SetBin0(char* bin, int ref)
{
	char* ptBin = bin;
	int count = *(int*)ptBin;
	if (count > 0)
	{
		ptBin += 4;
		int pos = *(int*)ptBin;
		*(int*)ptBin += ref;
		ptBin = pos + bin;
		for (size_t i = 0; i < count; i++)
		{
			pos = *(int*)(ptBin);
			*(int*)(ptBin) += ref;
			*(int*)(bin + pos + 0x3c) += ref;
			ptBin += 4;
		}
	}
}
//--------------------------------------------------------
void ResetBin1(char* bin, int ref, int count)
{
	int count0;
	char* ptBin = bin;
	char* pt1Bin, * pt2Bin;
	if (count > 0)
	{
		for (size_t i = 0; i < count; i++)
		{
			*(int*)ptBin -= ref;
			pt1Bin = *(int*)ptBin + bin;
			{
				pt1Bin += 4;
				count0 = *(int*)pt1Bin;
				pt1Bin += 4;
				if (count0 > 0)
				{
					*(int*)pt1Bin -= ref;
					pt2Bin = *(int*)pt1Bin + bin;
					{
						for (size_t j = 0; j < count0; j++)
						{
							*(int*)pt2Bin -= ref;
							*(int*)(bin + *(int*)(pt2Bin)+0x18) -= ref;
							pt2Bin += 4;
						}
					}
				}
			}
			ptBin += 4;
		}
	}
}
void SetBin1(char* bin, int ref, int count)
{
	int count0, pos;
	char* ptBin = bin;
	char* pt1Bin, * pt2Bin;
	if (count > 0)
	{
		for (size_t i = 0; i < count; i++)
		{
			pos = *(int*)ptBin;
			*(int*)ptBin += ref;
			ptBin += 4;
			pt1Bin = pos + bin;
			{
				pt1Bin += 4;
				count0 = *(int*)pt1Bin;
				pt1Bin += 4;
				if (count0 > 0)
				{
					pos = *(int*)pt1Bin;
					*(int*)pt1Bin += ref;
					pt2Bin = pos + bin;
					{
						for (size_t j = 0; j < count0; j++)
						{
							pos = *(int*)pt2Bin;
							*(int*)pt2Bin += ref;
							*(int*)(bin + pos + 0x18) += ref;
							pt2Bin += 4;
						}
					}
				}
			}
		}
	}
}
//--------------------------------------------------------
void ResetMesBin(char* bin, int ref, int* size)
{
	int count;
	MessBinHdr* binHdr = reinterpret_cast<MessBinHdr*>(bin);
	*size = binHdr->size;
	binHdr->offset0 -= ref;
	if (binHdr->offset1 != -1)
	{
		binHdr->offset1 -= ref;
	}
	binHdr->offset2 -= ref;
	binHdr->offset3 -= ref;
	char* ptBin = bin;
	ptBin += binHdr->offset0;
	count = *(int*)ptBin;
	ptBin += 4;
	if (count > 0)
	{
		*(int*)ptBin -= (ref + binHdr->offset0);
		ptBin += 12;
		for (size_t i = 0; i < count; i++)
		{
			*(int*)ptBin -= (ref + binHdr->offset0);
			ptBin += 4;
		}
	}
	ptBin = bin + binHdr->offset3;
	count = *(int*)ptBin;
	ptBin += 4;
	if (count > 0)
	{
		*(int*)ptBin -= (ref + binHdr->offset3);
		ptBin += 12;
		for (size_t i = 0; i < count; i++)
		{
			*(int*)ptBin -= (ref + binHdr->offset3);
			*(int*)(bin + *(int*)(ptBin)+0x3c + binHdr->offset3) -= (ref + binHdr->offset3);
			ptBin += 4;
		}
	}
}
void SetMesBin(char* bin, int ref, int* size)
{
	int count;
	MessBinHdr* binHdr = reinterpret_cast<MessBinHdr*>(bin);
	int offset0 = binHdr->offset0;
	int offset1 = binHdr->offset1;
	int offset2 = binHdr->offset2;
	int offset3 = binHdr->offset3;
	*size = binHdr->size;
	binHdr->offset0 += ref;
	if (binHdr->offset1 != -1)
	{
		binHdr->offset1 += ref;
	}
	binHdr->offset2 += ref;
	binHdr->offset3 += ref;
	char* ptBin = bin;
	ptBin += offset0;
	count = *(int*)ptBin;
	ptBin += 4;
	if (count != 0)
	{
		*(int*)ptBin += (ref + offset0);
	}
	ptBin += 12;
	if (count > 0)
	{
		for (size_t i = 0; i < count; i++)
		{
			*(int*)ptBin += (ref + offset0);
			ptBin += 4;
		}
	}
	ptBin = bin + offset3;
	count = *(int*)ptBin;
	ptBin += 4;
	if (count > 0)
	{
		*(int*)ptBin += (ref + offset3);
		ptBin += 12;
		for (size_t i = 0; i < count; i++)
		{
			int pos = *(int*)ptBin;
			*(int*)ptBin += (ref + offset3);
			*(int*)(bin + pos + 0x3c + offset3) += (ref + offset3);
			ptBin += 4;
		}
	}
}

