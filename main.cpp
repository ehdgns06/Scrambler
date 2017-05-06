/**
@param should_pass_it_a_file
@authr Brett Cooper
@version 0.8
@title scramble compression
@copyright  https://creativecommons.org/licenses/by/3.0/nz/legalcode
@copybrief basically attribute but job offers welcome

	About: https://docs.google.com/document/d/1VPEX7MCoDrxOEC11lldJhpyzcPqs6bcEY3G-vfjWjvw/edit  < should make public but are messier than code

 An experimental prof of concept transformative algorythem

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
//void scramble(uint8_t sKey, boost::dynamic_bitset & inBuff, boost::dynamic_bitset & outBuff);    // pass pointer to source file (one day


/** once i workout the 1bit start pos i can then track it for that overlap byte
	easy way to test the split is to have the test file filled with the value of the scramble key
	that way i would end up with all zero and one bits.
*/
int main() {
//  auto myFileName = "C:/test_data/test.zip";
	auto myFileName = "C:/test_data/test.txt";
//	auto myFileName = "C:/test_data/test_s.txt";
//	auto myFileName = "C:/test_data/big_ascii.txt";

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

			cout << bits_read[bities]; // debug

			if(bits_read[bities]) {					// only counting the number of bits that are 1
				bit_occurrence[bities]++;			// add to bit occurrence array[0-7] to find scramble key
			}
			inputfile_bitposition++;
		}  // end of byte to bit process

		nextbyte_index++;
		cout << endl; // debug

	} // end read file loop

	if (myFileStream.bad()) perror("error while reading file");
	myFileStream.close();											// could be better but meh

/*-------------------------------------------------------------------
	compute the scramble key

 todo still have a bug in my logic something with bit 7
 and my labels and documentation keeps inverting

-------------------------------------------------------------------*/
	int idxbit = 0;
	bitset scrambleKey(8);

	foreach(unsigned long occured, bit_occurrence) {	// work out most used bits for scramble key pattern
					// var bool = (fileLength-occured) > (fileLength/2); todo avoid this becoming a negative number?
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
	cout << "scramble key should be " << scrambleKey << endl;


	bitset outputBitsBuffer(number_of_bits); // allocate output buffer, same size as input file


//	scramble(sk, inputfile_bits, outputBuffer);	// one day  build scramble as a function (not needed for ascii test
/** scramble key and pass pointer to source file
///----------------------------------------------------------------
///	return 0;
/// }  // end main  todo old end of main as below is the function.
///----------------------------------------------------------------*/

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
/*	std::ofstream outfile ("C:/test_data/new.txt",std::ofstream::binary);

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

	outfile.close();*/

	// de scramble

	/*
	 * take scramble key and computer offset of scrambled bits
	 * loop through outputbits and putting them back into newoptput
	 *
	 *   ---  do the DE scrambling transformation of the bits   ---
	 * todo use clean variables?
	 */
/// reusing variables

	bitset newOutPut(fileLength);
	zeroBitsInScrambleKey = (uint8_t) (8-scrambleKey.count());		// returns number of 0 bits are in scramble key
	OutputPositionForKeysOneBits = (int) zeroBitsInScrambleKey*8;   // offset to one bits block in the output file

	ob0x = 0;									// output buffer zero bit index
	ob1x = OutputPositionForKeysOneBits;		// output buffer one  bit index

	for (long theNextBit=0; theNextBit < number_of_bits; theNextBit++) {  	// loop through all the bits in the output buffer  (long?)

		for (int theBit = 7; theBit >=0 ; --theBit) {	// loop through the scramble key pulling bits from the correct part of the output buffer
			if (scrambleKey[theBit]==0) { 				// test if scramble key bit is zero
				newOutPut[theNextBit]=outputBitsBuffer[ob0x]; ///  todo crashing bug
				ob0x++;
			}
			else								// scramble key bit was a one
			{
				newOutPut[theNextBit]=outputBitsBuffer[ob1x];  ///  todo crashing bug
				ob1x++;
			}
		theNextBit++;   /// as we arr doing 8 bits in this sub loop

		}

	}		// end of processing file.  loop through byte extracting bits into the two different parts

	for (int j = 0; j < number_of_bits ; ++j) {
		for (int e = 0; e < 8; ++e) {
			cout << newOutPut[j]; // debug
			j++;
		}
		cout << endl;
	}

} // end main



