#ifndef MYHTTP_CURL_WRITERS_HPP
#define MYHTTP_CURL_WRITERS_HPP
#include <string_view>
#include <filesystem.h>
#include <curl/curl.h>

namespace MyHTTP {
	namespace CurlWriters {
		static size_t WriteToFileHandler(void* data, size_t size, size_t nmemb, FileHandle_t fh)
		{
			size_t realsize = size * nmemb;
			size_t written = g_pFullFileSystem->Write(data, realsize, fh);
			return written;
		}

		static FileHandle_t WriteToFile(CURL* curl, std::string_view filename, const char* pathID = 0)
		{
			FileHandle_t fh = g_pFullFileSystem->Open(filename.data(), "wb", pathID);
			if (!fh)
				return fh;

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFileHandler);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fh);
			return fh;
		}
	}
}

#endif