#pragma once
#include <curl/curl.h>
#include <memory>
#include <string>

namespace st {
struct HttpResponse { long status = 0; std::string body; };
class HttpClient {
    std::string error;
    static size_t Write(char* data, size_t size, size_t count, void* target) {
        try { static_cast<std::string*>(target)->append(data, size * count); }
        catch (...) { return 0; }
        return size * count;
    }
public:
    static std::string Escape(const std::string& value) {
        char* escaped = curl_easy_escape(nullptr, value.c_str(), static_cast<int>(value.size()));
        if (!escaped) throw std::bad_alloc();
        std::string result(escaped); curl_free(escaped); return result;
    }
    const std::string& Error() const { return error; }
    std::unique_ptr<HttpResponse> Get(const std::string& path) {
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> request(curl_easy_init(), curl_easy_cleanup);
        if (!request) { error = "Could not initialize HTTPS client."; return nullptr; }
        std::unique_ptr<HttpResponse> response(new HttpResponse);
        std::string url = "https://www.shadertoy.com" + path;
        curl_easy_setopt(request.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(request.get(), CURLOPT_CONNECTTIMEOUT, 10L);
        curl_easy_setopt(request.get(), CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(request.get(), CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(request.get(), CURLOPT_PROTOCOLS_STR, "https");
        curl_easy_setopt(request.get(), CURLOPT_WRITEFUNCTION, Write);
        curl_easy_setopt(request.get(), CURLOPT_WRITEDATA, &response->body);
        CURLcode result = curl_easy_perform(request.get());
        if (result != CURLE_OK) {
            error = std::string("HTTPS request failed: ") + curl_easy_strerror(result);
            return nullptr;
        }
        curl_easy_getinfo(request.get(), CURLINFO_RESPONSE_CODE, &response->status);
        return response;
    }
};
}
