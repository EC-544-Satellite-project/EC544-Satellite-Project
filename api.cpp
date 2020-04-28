#include <iostream>
#include <string>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

// *** Compile with "-lcurl -ljsoncpp"

// Default zip for Boston, MA
std::string zip_code = "02215";
std::string api_key = "42f316e651f867e34f36671ab52e6192";
std::string url = "api.openweathermap.org/data/2.5/weather?zip=";

// Initialize JSON
JSONCPP_STRING err;
Json::Value root;
Json::Reader reader;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(void)
{

  std::cout << "Please enter a US zip code: ";
  std:: cin >> zip_code;
  // Check if valid zip code
  char* p;
  strtol(zip_code.c_str(), &p, 10);
  while (*p || zip_code.length() != 5) {
    std::cout << "Please enter a numeric 5 digit US zip code: ";
    std::cin >> zip_code;
    strtol(zip_code.c_str(), &p, 10);
  }

  url.append(zip_code);
  url.append("&appid=");
  url.append(api_key);

  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::cout << readBuffer << std::endl;

    reader.parse(readBuffer, root);

    // Get desired values
    std::string location = root["name"].asString();
    std::string weather = root["weather"][0]["main"].asString();
    std::string temp = std::to_string(root["main"]["temp"].asDouble() - 273.15).substr(0,5);


    std::cout << temp << std::endl;
    std::cout << root["weather"][0]["main"].asString() << std::endl;


  }
  return 0;
}
