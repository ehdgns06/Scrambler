/**
@param should_pass_it_a_file
@authr Brett Cooper
@version 0.5
@title scramble compression

	About: https://docs.google.com/document/d/1VPEX7MCoDrxOEC11lldJhpyzcPqs6bcEY3G-vfjWjvw/edit

 An experimental transformative algorythem

**/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
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
	auto myFileName = "C:/test_data/test_s.txt";

	cout.setf  ( ios::right | ios::showbase | ios_base::uppercase);  // set cout default chartype
	cout << endl << "Scramble Compression test file: " << myFileName <<endl;

	//-----------------------------------------------------------------------
	// loadFile
	// todo  change to using boost dynamic bitset

	ifstream myFileStream;
	myFileStream.open ( myFileName, ios::binary );  // open file - should have an if incase of a fail or test good
	if (!myFileStream.is_open()) perror("error while opening file");

	myFileStream.seekg (0, ios::end);				// get length of file:
	unsigned long fileLength;
	fileLength = (unsigned long) myFileStream.tellg();
	myFileStream.seekg (0, ios::beg);				// reset the file index

	unsigned long number_of_bits;
	number_of_bits = (fileLength * 8);
	bitset inputfile_bits(number_of_bits);   // allocate a dynamic bitset block (unsigned int)
																// to the length of the file in bits
	cout << "number of bits = " << number_of_bits << endl;
	cout << fileLength << " bytes long" << endl;

	vector <int> ncount(255,0);             // a vector to count up the number of types a bytes occurrence.
	vector <long> bit_occurrence(8,0);      // find the most used bits in the file, used to generate scramble key,
											// only tracking 1 bits
	long inputfile_bitposition = 0;
	int nextbyte_index=0;
	uint8_t byte_read;						// 8 bit byte buffer for reading input file
	bitset bits_read(8);

	// main readfile loop ---------------------------------
	while(myFileStream >> std::noskipws >> byte_read) {   // read in file loop one byte at a time
		bitset bits_read(8,byte_read);  // convert byte to a 8 bit bitset

		ncount[byte_read]++;			// count number of different bytes by adding 1 to an array($ff) 0-FF

		for (int bities = 7; bities >=0 ; --bities) { 		// process them bytes into the huge bit array
        	inputfile_bits[inputfile_bitposition] = bits_read[bities];         // store bit into input file
			cout << bits_read[bities]; // debug

			if(bits_read[bities]) {                // only count the number of 1 bits
				bit_occurrence[bities]++;          // add to bits occurrence array[0-7] to find scramble key
			}
			inputfile_bitposition++;
		}  // end of byte to bit process

		nextbyte_index++;
		cout << endl; // debug

	} // end read file loop

	if (myFileStream.bad()) perror("error while reading file");
	myFileStream.close();
//-------------------------------------------------------------------
	int idxbit = 0;
	bitset scrambleKey(8);
	foreach(int occured, bit_occurrence) {  // work out most used bits for scramble key pattern
		if (occured==0) {
			occured = number_of_bits/8;   			// All the bits where zeros so define occurred to avoid division by 0
		}
		cout << "At bit position "<< idxbit << ", " << occured << " zero bits where present in file or "
             << (occured * 100)/fileLength << "%" << " of " << fileLength << " bytes" << endl;

			if (((occured * 100)/fileLength) > 99) {  // compute scramble key
				scrambleKey[idxbit]=0;
			} else {
				scrambleKey[idxbit]=1;
			}

		idxbit++;
	}
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

	cout << "Scramble key has " << (int) zeroBitsInScrambleKey << " zero bits" << endl;
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

	/// i should display the output there.

	cout << inputfile_bits << endl;
	cout << outputBitsBuffer << endl;

} // end main



