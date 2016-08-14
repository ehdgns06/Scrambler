/**
 @param should_pass_it_a_file

 @authr Brett Cooper

	scramble compression

	About: https://docs.google.com/document/d/1VPEX7MCoDrxOEC11lldJhpyzcPqs6bcEY3G-vfjWjvw/edit

**/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
#include <bitset>

using namespace std;

/*!
 * scramble()
 * @param sKey
 * @param inBuff
 * @param outBuff
 */


// predefine functions
void scramble(uint8_t sKey, vector<uint8_t> & inBuff, vector<uint8_t> & outBuff);    // pass pointer to source file


// once i workout the 1bit start pos i can then track it for that overlap byte
//
// easy way to test the split is to have the test file filled with the value of the scramble key
// that way i would end up with all zero and one bits.


int main()
{
//    auto myFileName = "C:/test_data/test.zip";
    auto myFileName = "C:/test_data/test.txt";

    unsigned long long int length;

	vector<uint8_t> inputBuffer;

	cout.setf  ( ios::right | ios::showbase | ios_base::uppercase);  // set cout default chartype
	cout << endl << "Scramble Compression test file: " << myFileName;

	//-----------------------------------------------------------------------
	// loadFile

	ifstream myFileStream;
	myFileStream.open ( myFileName, ios::binary );  // open file - should be on an if incase of a fail or test good
	myFileStream.seekg (0, ios::end);				// get length of file:
	length = (unsigned long long int) myFileStream.tellg();
	myFileStream.seekg (0, ios::beg);				// reset the file index

	inputBuffer.reserve( length );					// allocate vector space for file

	vector <int> ncount(255,0);
	int nxd=0;
	while (myFileStream.good())						// loop while extraction from file is possible
	 {
	   inputBuffer[nxd] = (uint8_t) myFileStream.get();		// get character from file and put into vector
	   ncount[inputBuffer[nxd]]++;					// count number of different bytes/characters here.
	   nxd++;
	 }

	myFileStream.close();
//-------------------------------------------------------------------
	cout << ", " << length << " bytes long" << endl;
//--------------------------------------------------------------------

	vector<uint8_t> outputBuffer(length);		// allocate output buffer, same size as input file

	uint8_t sk = 50;	// test key
	scramble(sk, inputBuffer, outputBuffer);	// scramble key and pass pointer to source file

// todo unscramble and test to orgional file.

//---------------------------------------------------------------------
	cout << "Total number of bits " << length*8 << endl;
	cout << "input buffer capacity " << inputBuffer.capacity() << endl;

	return 0;
}  // end main

/* ----------------------------------------------------------------------
   main routine
*/

void scramble(uint8_t sKey, vector<uint8_t> &inBuff, vector<uint8_t> &outBuff)    // pass pointer to source file
{
	bitset <8> scrKey;
	scrKey= (int)sKey;		// convert scramble key into a binary value
	cout << "Scramble key = "<< scrKey << " ";

	// computer output buffer position and output bit counters to manage over writing boundry
	// computer bit/byte position of 1bits 0 bits go into the first part of the output buffer

	unsigned long long int inputFileSize=inBuff.capacity();			// using capacity as .size() fails

	double bitOutputSize0 = 0;			// contains next position for bit, zero bits start from postion 0
	double bitOutputPosition1 = 0;

    uint8_t zeroBitsInScrambleKey = (uint8_t) (8-scrKey.count());		// returns number of 0 bits are in scramble key

	cout << "containing " << (int) zeroBitsInScrambleKey << " bits that are zero " << endl;

	double percentOfZeroBitsInScrambleKey = (double) zeroBitsInScrambleKey / 8;
	cout << endl << "The Output file has %" << ( percentOfZeroBitsInScrambleKey*100) << " zero bits, ";
	bitOutputSize0 = (inputFileSize * percentOfZeroBitsInScrambleKey) * 8;	// times 8 to convert bytes to bits

	cout << "That is " <<  bitOutputSize0 << " bits." << endl << endl;

	bitOutputPosition1 = bitOutputSize0; // + the bit offset. which is backwards due to bin coding <- and logic coding ->

	/* todo byte poz is out */
	cout << "The bit position for the 1 bits file/block starts at bit " <<  bitOutputPosition1 << endl;
//	cout << ", byte position " <<  bitOutputPosition1/8 << endl;
	int overlapPoz = (int) floor (bitOutputPosition1/8);
	cout << "The overlap position is on byte " << overlapPoz;	// overlap byte is at bitOutputPosition1/8

	double overlapBitOffset = fmod((bitOutputPosition1),8);		// if 0 its on a byte boundy
	cout << " and the 1bits start at bit " <<  overlapBitOffset << endl<<endl;

//---------
/*	workout bit positions
	bitpoz = pos/8 with mod being index into bit

	overlapByte

	ob0x and ob1x are byte indexs but i think i need to use a bit index
	i also need to track that overlap between 0bit and 1bit split.

	ob1x = overlapPoz


	// test if overlap bit when processsng a zero
	// start with putting the one bits into the overlap byte
	// test if end of file and then write overlap byte

 */

	int ob0x = 0;						// output buffer zero bit index
	int ob1x = bitOutputPosition1/8;	// output buffer one  bit index, also position of overlap bit
	cout << "ob1x " << ob1x << endl;
	int zeroBitIdx = 0;
	int oneBitIdx = (int) overlapBitOffset+1;		// start of one bits poz or overlap ( +1  could be zero ?????)
	int bitProcessed = 0;			//

	bitset <8> tempBits;

	bitset <8> zeroByte_output;		// starts from zero but ends on a possible non byte boundry
	bitset <8> oneByte_output;		// starts from possible non byte boundry, so needs to be loaded withlow bits?
//	bitset <8> overlapByte;			// used to store the overlap byte

	for (int i=0; i<inputFileSize; i++)
	{
//		if (i%7==0&&i>0) { cout << endl;}  // text output formating

		tempBits = (int) inBuff[i];			// gets a byte from the input file to process
//		cout << tempBits << " ";			// display input data.

		for( int theBit = 0; theBit < 8 ; theBit++ )   // should this loop be 8->0 -- to cottect bit order
		{
			bitProcessed++;						// bit processed counter
			if (scrKey[theBit]==0)  			// test if scramble key bit is zero
			{
				if (zeroBitIdx==8)				// is byte full
				{
					/* todo out of input data - bits are backwards :(
                     * test is out of input data to process the bit overlap
                     */
					// write byte function
					outBuff[ob0x]= (uint8_t) zeroByte_output.to_ulong();
					ob0x++;

					zeroBitIdx=0;	// reset the zero bit index for forming a byte
//					cout << "." << zeroByte_output << ". ";
				}
				// store bit
				zeroByte_output[zeroBitIdx]=tempBits[theBit];
				zeroBitIdx++;				// output bytes bit index

			}
			else							// scramble key bit was a one
			{
				if (oneBitIdx==8)
				{
					// write byte function

					outBuff[ob1x]= (uint8_t) oneByte_output.to_ulong();
					ob1x++;

					oneBitIdx=0;
//					cout << "'" << oneByte_output << "' ";

				}
				// store bit
				oneByte_output[oneBitIdx]=tempBits[theBit];
				++oneBitIdx;
			}
		}


	}		// end of processing file.  loop through byte extraing bits into the two different parts

// write overlap byte to output file

// display processed file
//	cout << endl << endl;
	for (int i=0; i<inputFileSize; i++)
	{
	//	if (i%7==0&&i>0) { cout << endl;}
		tempBits = (int) outBuff[i];
//		cout << tempBits << " ";
	}

//	cout << endl;

/*	may not need to worry about the one byte boundy over right as it's low bytes will be undefined
	will need to test position of the bit overlap between 1bit and 0bit area.
	take input byte and put it into the output bytes,  when a byte is fullwrite it out to the output buffer. */

}
// end main


