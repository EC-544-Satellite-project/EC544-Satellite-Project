#include "crypto++/osrng.h"    
using CryptoPP::AutoSeededRandomPool;    

#include <iostream>    
using std::cout;    
using std::cerr;      
using namespace std; 

#include <string>    
using std::string;       

#include "crypto++/cryptlib.h"    
using CryptoPP::Exception;        

#include "crypto++/hex.h"    
using CryptoPP::HexEncoder;    
using CryptoPP::HexDecoder;        

#include "crypto++/filters.h"    
using CryptoPP::StringSink;    
using CryptoPP::StringSource;    
using CryptoPP::StreamTransformationFilter;        

#include "crypto++/aes.h"    
using CryptoPP::AES;       

#include "crypto++/ccm.h"    
using CryptoPP::CBC_Mode;       

#include "assert.h"        

int main(int argc, char* argv[])    
{    
    AutoSeededRandomPool prng;        

    byte key[ AES::DEFAULT_KEYLENGTH ];    
    prng.GenerateBlock( key, sizeof(key) );        

    byte iv[ AES::BLOCKSIZE];    
    iv[AES::BLOCKSIZE] = 0;    
    prng.GenerateBlock(iv,  sizeof(iv) );        

    string plain = "Today is sunny";    
    string cipher, encoded, recovered;       

 
    encoded.clear();    
    StringSource( key, sizeof(key), true,    
                  new HexEncoder(new StringSink( encoded )) 
    ); 

    cout << "key: " << encoded << endl;        
  
    encoded.clear();

    StringSource( iv, sizeof(iv), true,    
        new HexEncoder(new StringSink( encoded )) 
    ); 

    cout << "iv: " << encoded << endl;       


    try    
    {    
        cout << "plain text: " << plain << endl;            
        CBC_Mode< AES >::Encryption e;    
        e.SetKeyWithIV( key, sizeof(key), iv );     
 
        StringSource( plain, true,     
            new StreamTransformationFilter( e,    
                new StringSink( cipher )    
            )         
        );  
    }    
    catch( CryptoPP::Exception& e )    
    {    
        cerr << "Caught Exception..." << endl;    
        cerr << e.what() << endl;    
        cerr << endl;    
    }    

    encoded.clear();    
    StringSource( cipher, true,    
        new HexEncoder(    
            new StringSink( encoded )    
        )    
    );   
    cout << "cipher text: " << encoded << endl;    



    try    
    {    
        CBC_Mode< AES >::Decryption d;    
        d.SetKeyWithIV( key, sizeof(key), iv );    


        StringSource s( cipher, true,     
            new StreamTransformationFilter( d,    
                new StringSink( recovered )    
            )  
        ); 

        cout << "decrypted text: " << recovered << endl;    
    }    
    catch( CryptoPP::Exception& e )    
    {    
        cerr << "Caught Exception..." << endl;    
        cerr << e.what() << endl;    
        cerr << endl;    
    }    

    assert( plain == recovered );    

    return 0;    
}