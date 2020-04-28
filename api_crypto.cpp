// Compile with "g++ api_crypto.cpp -lcryptopp -lcurl -ljsoncpp"

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

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

// Default zip for Boston, MA
string zip_code = "02215";
string api_key = "42f316e651f867e34f36671ab52e6192";
string url = "api.openweathermap.org/data/2.5/weather?zip=";

// Initialize JSON
JSONCPP_STRING err;
Json::Value root;
Json::Reader reader;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


int main(int argc, char* argv[])
{
    // Get zip code as input
    cout << "Please enter a US zip code: ";
    cin >> zip_code;
    // Check if valid zip code
    char* p;
    strtol(zip_code.c_str(), &p, 10);
    while (*p || zip_code.length() != 5) {
        cout << "Please enter a numeric 5 digit US zip code: ";
        cin >> zip_code;
        strtol(zip_code.c_str(), &p, 10);
    }

    AutoSeededRandomPool prng;

    byte key[ AES::DEFAULT_KEYLENGTH ];
    prng.GenerateBlock( key, sizeof(key) );

    byte iv[ AES::BLOCKSIZE];
    iv[AES::BLOCKSIZE] = 0;
    prng.GenerateBlock(iv,  sizeof(iv) );

    string plain = zip_code;
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


    CURL *curl;
    CURLcode res;
    string readBuffer;

    url.append(recovered);
    url.append("&appid=");
    url.append(api_key);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        cout << "OpenWeather API Response: " << readBuffer << endl;

        reader.parse(readBuffer, root);

        // Get desired values
        string location = root["name"].asString();
        string weather = root["weather"][0]["main"].asString();
        string temp_c = to_string(root["main"]["temp"].asDouble() - 273.15);
	string temp_f = to_string((root["main"]["temp"].asDouble() - 273.15) * 9/5 + 32);

	// Format temp
	if (temp_c[2] == '.') {
	    temp_c= temp_c.substr(0,5);
	} else if (temp_c[1] == '.') {
	    temp_c = temp_c.substr(0,4);
	}
	if (temp_f[2] == '.') {
            temp_f= temp_f.substr(0,5);
        } else if (temp_f[1] == '.') {
            temp_f = temp_f.substr(0,4);
        } else if (temp_f[3] == '.') {
	    temp_f = temp_f.substr(0,6);
	}

	// Create output string
	string output = "The current temperature in ";
	output.append(location);
        output.append("(");
        output.append(recovered);
        output.append(")");
	output.append(" is ");
	output.append(temp_c);
        output.append("C/");
        output.append(temp_f);
        output.append("F. The current weather is ");
        output.append(weather);
	output.append(".");
        cout << output << endl;
    }



    assert( plain == recovered );

    return 0;
}
