#pragma once
struct MainDatHdr
{
	char unkArray0[0x10];
	char ADVP[0x4];
	int unk;
	int mainDatSize;
	char unkArray1[0x18];
	char ULJS[0x24];
	int sysEnd;
	int sysBinStart;
	char unkArray2[8];
	int bmpOffset;
	int bmpSize;
	int skinOffset;
	int skinSize;
	int soundPtrOffset;
	int soundPtrSize;
	char unkArray3[0x10];
	char KING_S_FIELD_ADDITIONAL[0x40];
	char FROMSOFTWARE[0x60];
};
struct SysBinHdr
{
	int sysBinSize;              //
	int count_1_bin;			 //
	int offset1_bin;			 //_1
	int count_2_bin;			 //
	int offset2_bin;			 //_2
	int offset0_text;			 //_0
	int offset5_bin;			 //_5
	int ptrBinOffset;			 //_8
	int count_unk;				 //
	int offset3_bin;			 //_3
	int offset4_bin;				 //_4
	char unkArray0[0x80];
	int offset6_bin;                  //_6
	int sysBndSize;				 //
	int sysBndOffset;			 //
	char unkArray1[0xc];		 //
	int offset7_bin;			 //_7
	char nullArray1[0xc];		 //
};
struct BinHeader
{
	int binSize;
	int binIndex;
	short count1_bin;
	short unk0;
	short count2_mess;
	short unk2;
	int offset1_bin;
	int offset2_mess;
	int offset3_img;
	int o20000;
	char internalName[0x20];
	int bndSize;
	int bndOffset;
	int offset0_bin;
	int nol;
};
struct MessBinHdr
{
	int size;
	int unk0;
	short unk1;
	short unk2;
	int count;
	int offset1;
	int offset2;
	int offset0;
	int offset3;
	char unkArray[0x60];
};

