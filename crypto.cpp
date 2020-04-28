#include "pch.h"
#include <iostream>
#include "aes.h"
#include <Windows.h>

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>
using std::exit;

#include "cryptlib.h"
using CryptoPP::Exception;

#include "hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "aes.h"
using CryptoPP::AES;

#include "ccm.h"
using CryptoPP::CBC_Mode;

#include "assert.h"

int main(int argc, char* argv[])
{
	AutoSeededRandomPool prng;

// Generate key
/*==========================================*/
	byte key[AES::DEFAULT_KEYLENGTH];
	prng.GenerateBlock(key, sizeof(key));
/*==========================================*/

//Generate initial vector
/*==========================================*/
	byte iv[AES::BLOCKSIZE];
	prng.GenerateBlock(iv, sizeof(iv));
/*==========================================*/

//text to be encrypted
/*==========================================*/
	string plain = "Today is sunny";
	string cipher, encoded, recovered;
/*==========================================*/

// Print key
/*==========================================*/
	encoded.clear();
	StringSource(key, sizeof(key), true,
		new HexEncoder(
			new StringSink(encoded)
		)
	); 
	cout << "key: " << encoded << endl;
/*==========================================*/

// Print initial vector
/*==========================================*/
	encoded.clear();
	StringSource(iv, sizeof(iv), true,
		new HexEncoder(
			new StringSink(encoded)
		) 
	); 
	cout << "iv: " << encoded << endl;
/*==========================================*/

//Encryption
/*==========================================*/
	try
	{
		cout << "plain text: " << plain << endl;

		CBC_Mode< AES >::Encryption e;
		e.SetKeyWithIV(key, sizeof(key), iv);
		StringSource s(plain, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) 
		); 

#if 0
		StreamTransformationFilter filter(e);
		filter.Put((const byte*)plain.data(), plain.size());
		filter.MessageEnd();

		const size_t ret = filter.MaxRetrievable();
		cipher.resize(ret);
		filter.Get((byte*)cipher.data(), cipher.size());
#endif
	}
	catch (const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
/*==========================================*/

//Print cipher
/*==========================================*/
	encoded.clear();
	StringSource(cipher, true,
		new HexEncoder(
			new StringSink(encoded)
		) 
	); 
	cout << "cipher text: " << encoded << endl;
/*==========================================*/

//Decryption
/*==========================================*/
	try
	{
		CBC_Mode< AES >::Decryption d;
		d.SetKeyWithIV(key, sizeof(key), iv);
		StringSource s(cipher, true,
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) 
		); 

#if 0
		StreamTransformationFilter filter(d);
		filter.Put((const byte*)cipher.data(), cipher.size());
		filter.MessageEnd();

		const size_t ret = filter.MaxRetrievable();
		recovered.resize(ret);
		filter.Get((byte*)recovered.data(), recovered.size());
#endif

		cout << "decrypted text: " << recovered << endl;
	}
	catch (const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
/*==========================================*/

	return 0;
}