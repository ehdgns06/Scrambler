/**
@param should_pass_it_a_file
@authr Brett Cooper
@version 0.8
@title scramble compression
@copyright  https://creativecommons.org/licenses/by/3.0/nz/legalcode
@copybrief basically attribute but job offers are welcomed too

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
//	auto myFileName = "C:/test_data/test.txt";
	auto myFileName = "C:/test_data/test_s.txt";
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

	cout << "The file is " << fileLength << " bytes long or " << number_of_bits << " bits" << endl << endl;

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
		cout << " " << byte_read << "  " ; // debug
		nextbyte_index++;


	} // end read file loop


	cout << endl << endl;

	if (myFileStream.bad()) perror("error while reading file");
	myFileStream.close();											// could be better but meh

/*-------------------------------------------------------------------
	compute the scramble key

 todo still have a bug in my logic something with bit 7
 and my labels and documentation keeps inverting

 This key might be backwards

-------------------------------------------------------------------*/
	int idxbit = 0;										// index
	bitset scrambleKey(8);
	long halfFileLen = (fileLength/2);

	foreach(unsigned long occured, bit_occurrence) {	// work out most used bits for scramble key pattern

		cout << "Bit "<< idxbit << " is set to " ;

			if (occured < halfFileLen) {	// compute scramble key.
				scrambleKey[idxbit]=0;
				cout << "0. ";
			} else {
				scrambleKey[idxbit]=1;
				cout << "1. ";
			}

		cout << "Because "  << occured << " bits is ";
		if (occured < halfFileLen) {	// compute scramble key.
			cout << "<= " << halfFileLen << endl;
		} else {
			cout << ">= " << halfFileLen << endl;
		}
		idxbit++;
	}

	cout << "scramble key is " << scrambleKey << endl;
//	scrambleKey[7]=1;  // so odd that this is reversed
//	cout << "scramble key changed too " << scrambleKey << endl;


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
/// zero bits * number of bytes = bit size of zero bits block
/// one bits * number of bytes = bit size of one bits block


	int OutputPositionForKeysOneBits;

	uint8_t zeroBitsInScrambleKey = (uint8_t) (8-scrambleKey.count());		// 8-[number of set bits]  returns number of 0 bits are in scramble key
	OutputPositionForKeysOneBits = (int) zeroBitsInScrambleKey*fileLength;	// size of the keys zero bits block = (8-[number of set bits]) * fileLength in bytes

	cout << endl << "Scramble key has " << (int) zeroBitsInScrambleKey << " zero bits" << endl;
	cout << "The start position for the 1 bits in the output file/buffer starts at bit " << OutputPositionForKeysOneBits << endl;

/// ----------------------------------------------------------------------------------------------------


/*  ---  do the scrambling transformation of the bits   ------------------------------------------------ */

	long ob0x = 0;							// output buffer zero bit index
	long ob1x = OutputPositionForKeysOneBits;// output buffer one  bit index
	long bitProcessed = 0;					//  this is the propper index for the input buffer as i sub look through the scramble key throwing out i

	for (long i=0; i < number_of_bits; i++) {  // loop through all the bits in the input buffer


		for (int theBit = 7; theBit >=0 ; --theBit) {	// loop through the scramble key putting the bits into the correct part of the output buffer
			if (scrambleKey[theBit]==0) { 		// test if scramble key bit is zero
				// store bit
				cout << "-" << inputfile_bits[bitProcessed] << " ";
				outputBitsBuffer[ob0x]=inputfile_bits[bitProcessed];
				ob0x++;
			}
			else								// scramble key bit was a one
			{
				outputBitsBuffer[ob1x]= inputfile_bits[bitProcessed];
				cout << "+" << inputfile_bits[bitProcessed] << " ";

				ob1x++;
			}
			bitProcessed++;						// bit processed counter
			i++;   /// can i inc this counter here
		}
		cout << endl;

	}		// end of processing file.  loop through byte extracting bits into the two different parts




	cout << "input  " << inputfile_bits << endl;
	cout << "output " << outputBitsBuffer << endl;


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
	 *
	 */

	bitset newOutPut(number_of_bits);
	zeroBitsInScrambleKey = (uint8_t) (8-scrambleKey.count());		// returns number of 0 bits are in scramble key
	OutputPositionForKeysOneBits = (int) zeroBitsInScrambleKey*fileLength;   // offset to one bits block in the output file

	cout << OutputPositionForKeysOneBits << "  one bits start at"<<endl;
	long outputBufferBits0index = 0;									// output buffer zero bit index
	long outputBufferBits1index = OutputPositionForKeysOneBits;		// output buffer one  bit index

	cout << "fixed  ";
	for (long theNextBit=fileLength-1; theNextBit >= 0; --theNextBit) {  	// loop through all the bits in the output buffer a byte at a time
		for (int TheKeysBit = 7; TheKeysBit >=0 ; --TheKeysBit) {	        // loop through the scramble key pulling bits from the correct part of the output buffer
			if (scrambleKey[TheKeysBit]==0) { 								// test if scramble key bit is zero
	//			newOutPut[theNextBit]=outputBitsBuffer[outputBufferBits0index];
		cout << outputBitsBuffer[outputBufferBits0index];
				outputBufferBits0index++;
			}
			else								// scramble key bit was a one
			{
	//			newOutPut[theNextBit]=outputBitsBuffer[outputBufferBits1index];
		cout << outputBitsBuffer[outputBufferBits1index];
				outputBufferBits1index++;
			}
	//	--theNextBit;   /// as we arr doing 8 bits in this sub loop

		}

	}		// end of processing file.  loop through byte extracting bits into the two different parts


} // end main



