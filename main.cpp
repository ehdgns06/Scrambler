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
#define testing   true
#define testing2  false

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
	auto myFileName = "C:/test_data/a.txt";
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

	cout << "The file is " << fileLength << " bytes long or " << number_of_bits << " bits" << endl << endl;

	vector <long> bit_occurrence(8,0);		// find the most used bits in the file, used to generate scramble key,
											// only tracking 1 bits
	long inputfile_bitposition = 0;
	long nextbyte_index=0;
	uint8_t byte_read;						// 8 bit byte buffer for reading input file

	/* read the file into binary format and count up what bits are present in each byte of the file. todo << lazy book mark */

	while(myFileStream >> std::noskipws >> byte_read) {		// read in file loop one byte at a time
		bitset bits_read(8,byte_read);						// convert byte to a 8 bit binary bitset

		for (int bities = 7; bities >=0 ; --bities) {		// process them bytes into the huge bit array
			inputfile_bits[inputfile_bitposition] = bits_read[bities];	// store bit into input file

  			#if testing2 == true
			cout << bits_read[bities]; // debug
			  #endif

			// count how many of each bit in file
			if(bits_read[bities]==1) {				// only counting the number of bits that are 1
				bit_occurrence[bities]++;			// add to bit occurrence array[0-7] to find scramble key
			}
			inputfile_bitposition++;
		}  // end of byte to bit process
		nextbyte_index++;

		#if testing2 == true
		cout << " " << byte_read << "  " ; // debug
		#endif
	} // end read file loop

	#if testing2 == true
	cout << endl << endl;
	#endif

	if (myFileStream.bad()) perror("error while reading file");
	myFileStream.close();											// could be better but meh

/*-------------------------------------------------------------------
	compute the scramble key todo << lazy book mark
-------------------------------------------------------------------*/
	int idxbit = 0;										// index
	bitset scrambleKey(8);								// define the variable to hold scramble key
	long halfFileLen = (fileLength/2);

	foreach(unsigned long occured, bit_occurrence) {	// work out most used zero bits for scramble key pattern
		#if testing == true
		cout << "Bit "<< idxbit << " is set to " ;
		#endif
			if (occured < halfFileLen) {	// compute scramble key. if pile of 1 bits is less than half the total
				scrambleKey[idxbit]=0;
				#if testing == true
				cout << "0. ";
				#endif
			} else {
				scrambleKey[idxbit]=1;
				#if testing == true
				cout << "1. ";
				#endif
			}
		#if testing == true
		cout << "Because "  << occured << " bits is ";
		if (occured < halfFileLen) {	// compute scramble key.
			cout << "<= " << halfFileLen << endl;
		} else {
			cout << ">= " << halfFileLen << endl;
		}
		#endif
		idxbit++;
	}
/*	scrambleKey[7]=1;
	scrambleKey[6]=1;
	scrambleKey[5]=1;
	scrambleKey[4]=1;
	scrambleKey[3]=0;
	scrambleKey[2]=0;
	scrambleKey[1]=0;
	scrambleKey[0]=0;*/
	cout << "scramble key is " << scrambleKey << endl;

	bitset outputBitsBuffer(number_of_bits); // allocate output buffer, same size as input file

/// ----------------------------------------------------------------------------------------------------
// add up the one bits in the file where the scramble keys 1 bits will start at todo << lazy book mark

	long OutputPositionForKeysOneBits;
	OutputPositionForKeysOneBits = number_of_bits-(bit_occurrence[0]+bit_occurrence[1]+bit_occurrence[2]+bit_occurrence[3]+bit_occurrence[4]+bit_occurrence[5]+bit_occurrence[6]+bit_occurrence[7]);
	cout << "1 bits takes up " << (bit_occurrence[0]+bit_occurrence[1]+bit_occurrence[2]+bit_occurrence[3]+bit_occurrence[4]+bit_occurrence[5]+bit_occurrence[6]+bit_occurrence[7]) << " bytes" <<endl;
	#if testing == true
	cout << "The start position for the 1 bits in the output file/buffer starts at bit " << OutputPositionForKeysOneBits << endl;
	#endif
/// ----------------------------------------------------------------------------------------------------

/*  ---  do the scrambling transformation of the bits   ------------------------------------------------ */

	long ob0x = 0;								// output buffer zero bit index
	long ob1x = OutputPositionForKeysOneBits;	// output buffer one  bit index
	long bitProcessed = 0;						//  this is the proper index for the input buffer as i sub look through the scramble key throwing out i

	for (long anotherBit=0; anotherBit < number_of_bits; anotherBit++) {// loop through all the bits in the input buffer
		for (int theBit = 7; theBit >=0 ; --theBit) {				// loop through the scramble key putting the bits into the correct part of the output buffer
			if (scrambleKey[theBit]==0) { 							// test if scramble key bit is zero
				outputBitsBuffer[ob0x]=inputfile_bits[bitProcessed];// store bit
				ob0x++;
			} else {							// scramble key bit was a one
				outputBitsBuffer[ob1x]= inputfile_bits[bitProcessed];
				ob1x++;
			}
			bitProcessed++;						// bit processed counter for input file data
			anotherBit++;   /// increment the main look as we processed another bit
		}
		anotherBit--;  // don't skip a bit at the end of a byte.
	}		// end of processing file.  loop through byte extracting bits into the two different parts

	#if testing == true
	cout << "ob0x = " << ob0x << " ob1x = " << ob1x << endl << "number of bits " << number_of_bits << endl;
	cout << endl << "input     " << inputfile_bits << endl;
	cout << endl << "output    " << outputBitsBuffer << endl;
	#endif

	// ----------------------------------------------------------------------------------------
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

	//----------------------------------------------------------------------------
	// de scramble
	/*
	 * take scramble key and computer offset of scrambled bits
	 * loop through outputbits and putting them back into newoptput
	 *
	 *   ---  do the DE scrambling transformation of the bits   ---
	 *-----------------------------------------------------------------------------*/

	long outputBufferBits0index = 0;							// output buffer zero bit index
	long outputBufferBits1index = OutputPositionForKeysOneBits;	// output buffer one  bit index

	bitset decoded(number_of_bits);
	long at_Bit = 0; // index for building decoded data

	// todo backwards  << lazy book mark

	for (long theNextBit=fileLength; theNextBit > 0; --theNextBit) {	// loop through all the bits in the output buffer a byte at a time
		for (int TheKeysBit = 7; TheKeysBit >=0 ; --TheKeysBit) {		// loop through the scramble key pulling bits from the correct part ( 0 or 1 block) of the output buffer
			if (scrambleKey[TheKeysBit]==0) { 							// test if scramble key bit is zero
				#if testing == true
				//cout << outputBitsBuffer[outputBufferBits0index];
				#endif
				decoded[at_Bit] = outputBitsBuffer[outputBufferBits0index];
				outputBufferBits0index++;
				at_Bit++;
			}
			else						// scramble key bit was a one
			{
				#if testing == true
				//cout << outputBitsBuffer[outputBufferBits1index];
				#endif
				decoded[at_Bit] = outputBitsBuffer[outputBufferBits1index];
				outputBufferBits1index++;
				at_Bit++;
			}
		}
	}		// end of processing file.  loop through byte extracting bits into the two different parts
	// todo decode << lazy book mark

	#if testing == true
	cout << endl << "forward" << endl << "decoded   " << decoded << endl;
	#endif

// write restored file
	#if testing == true
	cout << endl << "writing out decoded data" << endl;
	#endif
	std::ofstream outfile ("C:/test_data/restored.txt",std::ofstream::binary);

	uint8_t writeByte = 0;
	bitset aByte(8);


	// loop though output bitstream converting it to unsigned byte and writing to disk   (reverses data)
	for (int bits = 0; bits < number_of_bits; bits+=8) {
		aByte[7]=decoded[0+bits];
		aByte[6]=decoded[1+bits];
		aByte[5]=decoded[2+bits];
		aByte[4]=decoded[3+bits];
		aByte[3]=decoded[4+bits];
		aByte[2]=decoded[5+bits];
		aByte[1]=decoded[6+bits];
		aByte[0]=decoded[7+bits];

		writeByte = static_cast<uint8_t>(aByte.to_ulong());
		outfile.put(writeByte);
	}

	outfile.close();

	cout << "all done" << endl;
	
} // end main



