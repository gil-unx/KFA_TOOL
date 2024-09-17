
#include "HEADER/maindat.h"
#include <iostream>
static void ShowOptions()
{

    std::cout << "King's Field - Additional I/II File unpacker by Gil Unx" << std::endl << std::endl;
	std::cout << "Usage:"  << std::endl;
	std::cout << "	KFA_TOOL -d	main.dat		decrypt main.dat to main.dec" << std::endl;
	std::cout << "	KFA_TOOL -e	main.dec		encrypt main.dec to main_new.dat" << std::endl;
	std::cout << "	KFA_TOOL -x	main.dec		unpack main.dec" << std::endl;
	std::cout << "	KFA_TOOL -c	main.dec		repack main.dec" << std::endl;




}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        ShowOptions();
        return 0;
    }
    else
    {


        if (std::strcmp(argv[1], "-d") == 0)
        {
            MainDatDecrypt(argv[2]);
            std::cout << "Decrypt done!" << std::endl;
        }
        else if (std::strcmp(argv[1], "-e") == 0)
        {
            MainDatEncrypt(argv[2]);
            std::cout << "Encrypt done!" << std::endl;

        }
        else if (std::strcmp(argv[1], "-x") == 0)
        {
            MainDatUnpack(argv[2]);
            std::cout << "Unpack done!" << std::endl;

        }
        else if (std::strcmp(argv[1], "-c") == 0)
        {
            MainDatRepack(argv[2]);
            std::cout << "Repack done!" << std::endl;

        }
        else
        {
            ShowOptions();
        }



    }

}

