/**
@param should_pass_it_a_file
@authr Brett Cooper
@version 0.5
@title scramble compression

	About: https://docs.google.com/document/d/1VPEX7MCoDrxOEC11lldJhpyzcPqs6bcEY3G-vfjWjvw/edit

 An experimental transformative algorythem

**/

#include <iostream>
#include <iomanip>  // do i need this one now?
#include <fstream>
#include <vector>
#include <math.h>  // do i need this one too
#include <boost/dynamic_bitset.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#define bitset	boost::dynamic_bitset<>

using namespace std;

// predefine functions
//void scramble(uint8_t sKey, boost::dynamic_bitset & inBuff, boost::dynamic_bitset & outBuff);    // pass pointer to source file todo


/** once i workout the 1bit start pos i can then track it for that overlap byte
	easy way to test the split is to have the test file filled with the value of the scramble key
	that way i would end up with all zero and one bits.
*/
int main() {
//  auto myFileName = "C:/test_data/test.zip";
//	auto myFileName = "C:/test_data/test.txt";
//	auto myFileName = "C:/test_data/test_s.txt";
	auto myFileName = "C:/test_data/big_ascii.txt";

	cout.setf  ( ios::right | ios::showbase | ios_base::uppercase);  // set cout default chartype
	cout << endl << "Scramble Compression test file: " << myFileName <<endl;

	//-----------------------------------------------------------------------
	// loadFile

	ifstream myFileStream;
	myFileStream.open ( myFileName, ios::binary );  // open file - should have an if incase of a fail or test good
	if (!myFileStream.is_open()) perror("error while opening file");

	myFileStream.seekg (0, ios::end);				// get length of file:
	unsigned long fileLength;
	fileLength = (unsigned long) myFileStream.tellg();
	myFileStream.seekg (0, ios::beg);				// reset the file index

	unsigned long number_of_bits;
	number_of_bits = (fileLength * 8);
	bitset inputfile_bits(number_of_bits);	// allocate a dynamic bitset block (unsigned int) to the length of the file in bits

	cout << "number of bits = " << number_of_bits << " and the file is " << fileLength << " bytes long" << endl << endl;

	vector <long> bit_occurrence(8,0);		// find the most used bits in the file, used to generate scramble key,
											// only tracking 1 bits
	long inputfile_bitposition = 0;
	long nextbyte_index=0;
	uint8_t byte_read;						// 8 bit byte buffer for reading input file

	/* read the file into binary format and count up what bits are present in each byte of the file.  */

	while(myFileStream >> std::noskipws >> byte_read) {		// read in file loop one byte at a time
		bitset bits_read(8,byte_read);						// convert byte to a 8 bit binary bitset

		for (int bities = 7; bities >=0 ; --bities) {		// process them bytes into the huge bit array
			inputfile_bits[inputfile_bitposition] = bits_read[bities];	// store bit into input file

//			cout << bits_read[bities]; // debug

			if(bits_read[bities]) {					// only counting the number of bits that are 1
				bit_occurrence[bities]++;			// add to bit occurrence array[0-7] to find scramble key
			}
			inputfile_bitposition++;
		}  // end of byte to bit process

		nextbyte_index++;
//		cout << endl; // debug

	} // end read file loop

	if (myFileStream.bad()) perror("error while reading file");
	myFileStream.close();											// could be better but meh

	cout << "bit 0 = " << bit_occurrence[0] << endl;
	cout << "bit 1 = " << bit_occurrence[1] << endl;
	cout << "bit 2 = " << bit_occurrence[2] << endl;
	cout << "bit 3 = " << bit_occurrence[3] << endl;
	cout << "bit 4 = " << bit_occurrence[4] << endl;
	cout << "bit 5 = " << bit_occurrence[5] << endl;
	cout << "bit 6 = " << bit_occurrence[6] << endl;
	cout << "bit 7 = " << bit_occurrence[7] << endl << endl;		// should be zero but there is ascii that is greater than 128(dec) code

/*-------------------------------------------------------------------
	compute the scramble key

 todo still have a bug in my logic something with bit 7
 and my labels and documentation keeps inverting

-------------------------------------------------------------------*/
	int idxbit = 0;
	bitset scrambleKey(8);

	foreach(unsigned long occured, bit_occurrence) {	// work out most used bits for scramble key pattern
		cout << "Bit "<< idxbit << " is set to " ;

			if ((fileLength-occured) > (fileLength/2)) {	// compute scramble key.
				scrambleKey[idxbit]=1;
				cout << "1. ";
			} else {
				scrambleKey[idxbit]=0;
				cout << "0. ";
			}

		cout <<  occured << " bits of " << fileLength << " bits in file. ";
		if ((fileLength-occured) > (fileLength/2)) {	// compute scramble key.
			cout << "less."<<endl;
		} else {
			cout << "greater."<<endl;
		}
		idxbit++;
	}

	cout << "scramble key is " << scrambleKey << endl;
	scrambleKey[7]=0;
	cout << "scramble key is " << scrambleKey << endl;


	bitset outputBitsBuffer(number_of_bits); // allocate output buffer, same size as input file


//	scramble(sk, inputfile_bits, outputBuffer);	// todo build scramble as a function (not needed for ascii test
/** scramble key and pass pointer to source file
	todo unscramble and test to original file. */

//-----------------------------------------------------------------
///----------------------------------------------------------------
///	return 0;
/// }  // end main  todo old end of main as below is the function.
///----------------------------------------------------------------
//-----------------------------------------------------------------


/* ----------------------------------------------------------------------
   slipping into the scramble routine
   todo change to a function once it is working
*/

//void scramble(boost::dynamic_bitset<> sKey, boost::dynamic_bitset<> &inBuff, boost::dynamic_bitset<> *outBuff)
// pass pointer to source file
//void scramble ()
//{


/// ----------------------------------------------------------------------------------------------------
/// computer the bit postion in the output buffer where the scramble keys 1 bits will start at

	int OutputPositionForKeysOneBits;

	uint8_t zeroBitsInScrambleKey = (uint8_t) (8-scrambleKey.count());		// returns number of 0 bits are in scramble key
	OutputPositionForKeysOneBits = (int) zeroBitsInScrambleKey*8;

	cout << endl << "Scramble key has " << (int) zeroBitsInScrambleKey << " zero bits" << endl;
	cout << "The start position for the 1 bits in the output file/buffer starts at bit " << OutputPositionForKeysOneBits << endl;

/// ----------------------------------------------------------------------------------------------------


/*  ---  do the scrambling transformation of the bits   ------------------------------------------------ */

	int ob0x = 0;							// output buffer zero bit index
	int ob1x = OutputPositionForKeysOneBits;// output buffer one  bit index
	int bitProcessed = 0;					//  this is the propper index for the input buffer as i sub look through the scramble key throwing out i

	for (int i=0; i < number_of_bits; i++) {  // loop through all the bits in the input buffer


		for (int theBit = 7; theBit >=0 ; --theBit) {	// loop through the scramble key putting the bits into the correct part of the output buffer
			if (scrambleKey[theBit]==0) { 		// test if scramble key bit is zero
				// store bit
				outputBitsBuffer[ob0x]=inputfile_bits[bitProcessed];
				ob0x++;
			}
			else								// scramble key bit was a one
			{
				outputBitsBuffer[ob1x]= inputfile_bits[bitProcessed];
				ob1x++;
			}
			bitProcessed++;						// bit processed counter
			i++;   /// can i inc this counter here

		}

	}		// end of processing file.  loop through byte extracting bits into the two different parts




//	cout << inputfile_bits << endl;
//	cout << outputBitsBuffer << endl;


	//
	// open file in write file
	std::ofstream outfile ("C:/test_data/new.txt",std::ofstream::binary);

	uint8_t writeByte = 0;
	bitset aByte(8);

	// loop though output bitstream converting it to unsign byte and writing to disk   (reverses data)
	for (int bits = 0; bits < number_of_bits; bits+=8) {
		aByte[7]=outputBitsBuffer[0+bits];
		aByte[6]=outputBitsBuffer[1+bits];
		aByte[5]=outputBitsBuffer[2+bits];
		aByte[4]=outputBitsBuffer[3+bits];
		aByte[3]=outputBitsBuffer[4+bits];
		aByte[2]=outputBitsBuffer[5+bits];
		aByte[1]=outputBitsBuffer[6+bits];
		aByte[0]=outputBitsBuffer[7+bits];

		writeByte = static_cast<uint8_t>(aByte.to_ulong());
		outfile.put(writeByte);
	}

	outfile.close();


} // end main



