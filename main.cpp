/**
 @param should_pass_it_a_file
 @authr Brett Cooper

scramble compression

About: https://docs.google.com/document/d/1VPEX7MCoDrxOEC11lldJhpyzcPqs6bcEY3G-vfjWjvw/edit

**/
// TODO (DevBase#1#): add debuging conditions to code

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
#include <bitset>

using namespace std;

void pbin(int);         // nasty binary print function
void scramble(uint8_t sKey, vector<uint8_t> & inBuff);    // pass pointer to source file

// count number of 0 bits and 1 bits

int main()
{

//	char  * myFileName = "C:/test_data/test.txt"; // should be string
	char  * myFileName = "C:/test_data/test_s.txt"; // should be string

	int length;

	vector<uint8_t> inputBuffer;


	cout.setf  ( ios::right | ios::showbase | ios_base::uppercase);  // set cout default chartype
	cout << "Scramble Compression test file: " << myFileName << endl<< endl;

	//-----------------------------------------------------------------------
	// loadFile

	ifstream myFileStream;
	myFileStream.open ( myFileName, ios::binary );  // open file
													// above should be on an if incase of a fail or test good

	myFileStream.seekg (0, ios::end);				// get length of file:
	length = myFileStream.tellg();
	myFileStream.seekg (0, ios::beg);				// reset the file index

	inputBuffer.reserve( length );					// allocate vector space for file

	vector <int> ncount(255,0);
	int nxd=0;
	while (myFileStream.good())						// loop while extraction from file is possible
	 {
	   inputBuffer[nxd] = myFileStream.get();		// get character from file and put into vector 
	   ncount[inputBuffer[nxd]]++;					// count number of different bytes/characters here.
	   nxd++;
	 }

	myFileStream.close();
//-------------------------------------------------------------------
	cout  << length << " bytes in file" << endl;

//  count the number of same byte values in the file
//    for (int i=0; i < 256; i++) {
//      cout << i << " is " << ncount[i]<<": ";
//
//      if (i%8==0&&i>0) { cout << endl;}         // limit 8 per line
//    }

	cout << endl;
//--------------------------------------------------------------------
// void scramble(vector<int> & inputbuffer);    // pass pointer to source file

	uint8_t sk = 8;				// test key
	scramble(sk, inputBuffer);	// scramble key and pass pointer to source file

//---------------------------------------------------------------------
	cout << "Total number of bits " << length*8 << endl;
	cout << "inputbuffer capacity " << inputBuffer.capacity() << endl;

	return 0;
}  // end main

/*--------------------------------------------------------------------
	TODO scramble function: pass key and a pointer to the file to scramble
	need to pass a pointer to the output file as a function would delete Outputbuffer on exit */

void scramble(uint8_t sKey, vector<uint8_t> & inBuff)    // pass pointer to source file
{
	bitset <8> bKey;
	// computer output buffer position
	
	bKey= (int)sKey;
	cout << "Scramble key = "<< bKey << endl;
 
	// outputBuffSize
	int size=inBuff.capacity();			// using capacity as .size() fails
	
	vector<uint8_t> OutputBuffer(size);	// allocate output buffer
	double bitOutputSize0 = 0;			// contains next position for bit
	double bitOutputPosition1 = 0;
	int kpc0 = 0;  // key percentage 0 bits
	
// computer bit/byte position of 1bits 0 bits go into the first part of the output buffer

	kpc0 = (8-bKey.count());			// work out how many 0 bits are in key

	cout << endl << "number of zero bits the scamble key has = " << kpc0 << endl;
	double percent = (double) kpc0 / 8;
	cout << endl << "percentage of output file zero bits will consume = %" << ((double) percent*100) << endl << endl;
	bitOutputSize0 = size * percent * 8;					// times 8 to convert bytes to bits
	cout << "zero bit output size = " <<  bitOutputSize0 << endl;	

	bitOutputPosition1 = bitOutputSize0;
	//  above is borked and needs testing over a smaller range like 17 bytes  (a prime size test for reminers)
	// could also test in a size loop 2bytes to 255 bytes
	
	cout << "bit position for key 1 bits  = " <<  bitOutputPosition1 << endl << endl;
	cout << "byte position  = " <<  bitOutputPosition1/8 << endl << endl;
	
    double z = fmod((bitOutputPosition1),8);
	cout << "bit offset/remainder to 1bit area = " <<  z << endl<<endl;

//---------
	bitset <8> tempBits;
	for (int i=0; i<size; i++)
	{
		if (i%8==0&&i>0) { cout << endl;}	// limit 8 per line
		cout << inBuff[i] << ":";			// out put input file
		tempBits = (int) inBuff[i];			// gets a byte from the input file to process
		cout << tempBits << " ";			// display it for now.
	}		// loop through byte extraing bits into the two different parts
	cout << endl;
	
/*	work out bit positions  
	bitpoz = pos/8 with mod being index into bit
	check if i can just use whole bytes */ 
	
bitset <8> zeroByte_output;		// starts from zero but ends on a possible non byte boundry
bitset <8> oneByte_output;		// starts from possible non byte boundry, so needs to be loaded withlow bits?

/*	may not need to worry about the one byte boundy over right as it's low bytes will be undefined
	will need to test position of the bit overlap between 1bit and 0bit area. 
	take input byte and put it into the output bytes,  when a byte is fullwrite it out to the output buffer. */

}

//------------------------------------------------
// TODO will also need a BitPickup

// void BitDrop(
//		vecout		// pointer vector output file,
//		bitpos,		// bit position
//		bit			// value of bit being set
// )
// 
// computer byte bit position

//  void BitDrop( 	vector<uint8_t>, long bitpos, int bit)

//	bitOutputSize0 
//    double z = fmod((bitOutputSize0),8);
//	cout << "bit offset/remainder to 1bit area = " <<  z << endl<<endl;
