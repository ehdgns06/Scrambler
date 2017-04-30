/**
@param should_pass_it_a_file
@authr Brett Cooper
@version 0.5
@title scramble compression

	About: https://docs.google.com/document/d/1VPEX7MCoDrxOEC11lldJhpyzcPqs6bcEY3G-vfjWjvw/edit

**/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
#include <bitset>
#include <boost/dynamic_bitset.hpp>
using namespace std;

// predefine functions
//void scramble(uint8_t sKey, boost::dynamic_bitset & inBuff, boost::dynamic_bitset & outBuff);    // pass pointer to source file


// once i workout the 1bit start pos i can then track it for that overlap byte
//
// easy way to test the split is to have the test file filled with the value of the scramble key
// that way i would end up with all zero and one bits.

int main()
{
//  auto myFileName = "C:/test_data/test.zip";
//	auto myFileName = "C:/test_data/test.txt";
	auto myFileName = "C:/test_data/test_s.txt";

	cout << endl << "Started" << endl;

	cout.setf  ( ios::right | ios::showbase | ios_base::uppercase);  // set cout default chartype
	cout << endl << "Scramble Compression test file: " << myFileName <<endl;

	//-----------------------------------------------------------------------
	// loadFile
    // todo  change to using boost dynamic bitset

	ifstream myFileStream;
	myFileStream.open ( myFileName, ios::binary );  // open file - should have an if incase of a fail or test good
    if (!myFileStream.is_open()) perror("error while opening file");

    myFileStream.seekg (0, ios::end);				// get length of file:
    unsigned long length;
	length = (unsigned long) myFileStream.tellg();
	myFileStream.seekg (0, ios::beg);				// reset the file index

    unsigned long number_of_bits;
    number_of_bits = (length * 8);
    boost::dynamic_bitset<> inputfile_bits{number_of_bits};   // allocate a dynamic bitset block (unsigned int)
                                                                // to the length of the file in bits
    cout << "inputfile.size() = " << inputfile_bits.size() << endl;
    cout << "number of bits = " << number_of_bits << endl;

    vector <int> ncount(255,0);             // a vector to count up the number of types a bytes occurrence.
    vector <long> bit_occurrence(8,0);      // find the most used bits in the file, used to generate scramble key,
                                            // only tracking 1 bits
    long inputfile_bitposition = 0;
	int nextbyte_index=0;
    uint8_t byte_read;
    bitset<8> bits_read;    // todo change to boost??

    // main readfile loop ---------------------------------
    while(myFileStream >> std::noskipws >> byte_read)   // read in file loop one byte at a time
	 {
        ncount[byte_read]++;		// count number of different bytes by adding 1 to an array($ff) 0-FF
        bits_read = byte_read;		// convert the byte read into bits format (boost format?)

        // process them bits,  note reverse order of bits
         for (int bities = 7; bities >=0 ; --bities) {
            inputfile_bits[inputfile_bitposition] = bits_read[bities];         // store bit into input file
			 cout << bits_read[bities];

             if(bits_read[bities]){                 // only track 1 bits
				 bit_occurrence[bities]++;          // add to bits occurrence array (0-7) to find scramble key
				 // cout << "A bit was set at bit " << bities << endl;
			 }
			 ++inputfile_bitposition;
         }  // end of byte to bit process
         cout << endl;
         nextbyte_index++;
	 } // end read file loop
    if (myFileStream.bad()) perror("error while reading file");
	myFileStream.close();
//-------------------------------------------------------------------
	cout << length << " bytes long" << endl;
//--------------------------------------------------------------------

	boost::dynamic_bitset<> outputBuffer(number_of_bits);		// allocate output buffer, same size as input file

	uint8_t sk = 50;	// todo test key hard coded
//	scramble(sk, inputfile_bits, outputBuffer);	// scramble key and pass pointer to source file
// todo unscramble and test to orgional file.

//---------------------------------------------------------------------
	cout << "Total number of bits " << number_of_bits << endl;
//	cout << "input buffer capacity " << inputfile_bits.capacity() << endl;

    return 0;
}  // end main

/* ----------------------------------------------------------------------
   main scramble routine
   todo change from vector use to dynamic bitstrean
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
    int overlapPoz;
    overlapPoz = (int) floor (bitOutputPosition1 / 8);
	cout << "The overlap position is on byte " << overlapPoz;	// overlap byte is at bitOutputPosition1/8

    double overlapBitOffset;
    overlapBitOffset = fmod((bitOutputPosition1), 8);        // if 0 its on a byte boundy
	cout << " and the 1bits start at bit " <<  overlapBitOffset << endl<<endl;

//---------
/*	workout bit positions
	bitpoz = pos/8 with mod being index into bit

	overlapByte

	ob0x and ob1x are byte indexs but i think i need to use a bit index
	i also need to track that overlap between 0bit and 1bit split.

	ob1x = overlapPoz


	// test if overlap bit when processing a zero
	// start with putting the one bits into the overlap byte
	// test if end of file and then write overlap byte

 */

	int ob0x = 0;						// output buffer zero bit index
	int ob1x = (int) bitOutputPosition1/8;	// output buffer one  bit index, also position of overlap bit
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

		tempBits = (bitset<8>) inBuff[i];			// gets a byte from the input file to process, was am int cast
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
		tempBits = (bitset<8>) outBuff[i];  // was (int) cast
//		cout << tempBits << " ";
	}

//	cout << endl;

/*	may not need to worry about the one byte boundy over right as it's low bytes will be undefined
	will need to test position of the bit overlap between 1bit and 0bit area.
	take input byte and put it into the output bytes,  when a byte is fullwrite it out to the output buffer. */

}
// end main



