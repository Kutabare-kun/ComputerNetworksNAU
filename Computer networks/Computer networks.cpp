#include <iostream>
#include <string>
#include <random>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out) {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}


int generateRandomNumber(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);
    return distr(gen);
}


json fetchData(int id) {
    const std::string url = "https://www.anapioficeandfire.com/api/characters/" + std::to_string(id);
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }
    curl_easy_cleanup(curl);

    return json::parse(response);
}


int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    int randomID = generateRandomNumber(1, 100);
    json data = fetchData(randomID);

    if (data.is_null()) {
        std::cerr << "Failed to fetch data" << std::endl;
        return EXIT_FAILURE;
    }

    std::string name = data["name"];
    std::string gender = data["gender"];
    std::string culture = data["culture"];
    
    std::cout << "ID: " << randomID << std::endl;
    std::cout << "\tName: " << name << std::endl;
    std::cout << "\tGender: " << gender << std::endl;
    std::cout << "\tCulture: " << culture << std::endl;

    curl_global_cleanup();
    return EXIT_SUCCESS;
}
