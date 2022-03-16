
#include <iostream>
#include <string>
#include <string_view>
#include <regex>
#include <iostream>
#include <locale>
#include <memory>
#include <vector>
#include <tuple>

#include <string.h>     // for strcpy_s, strcat_s
#include <stdlib.h>     // for _countof

#include "curl_easy.h"
#include "curl_form.h"
#include "curl_exception.h"
#include "curl_header.h"

#include "HmCppInvoke.h"

#include "json.hpp"

using namespace Hidemaru;
using namespace std;

using curl::curl_header;
using curl::curl_pair;
using curl::curl_form;
using curl::curl_easy;
using curl::curl_easy_exception;
using curl::curlcpp_traceback;
using curl::curl_ios;

using json = nlohmann::json;

// jsonから翻訳の結果を得る
std::string deserialize_result(std::string u8text)
{
	try {
		wstring wstr = Text::Encoding::utf8_to_utf16(u8text);
		json j = json::parse(u8text);
		string u8translation_text = j["translations"][0]["text"];
		return u8translation_text;
	} catch (exception& e) {
		return "翻訳エラー"s + e.what();
	}

	return "";
}


// curlでの翻訳
wstring translate(const wstring wstr_auth_key, const wstring wstr_source_lang, const wstring wstr_target_lang, wstring wstr_text) {
	stringstream stream;

	// リクエストの出力を先に宣言したストリームに入れることになる
	curl_ios<stringstream> ios(stream);

	// curl_easyオブジェクトとして宣言
	curl_easy easy(ios);

	curl_header header;
	header.add("Content-Type:application/x-www-form-urlencoded;utf-8");


	// PHPのcurlと同様簡単に扱える
	easy.add<CURLOPT_HTTPHEADER>(header.get());

	string text = Text::Encoding::utf16_to_utf8(wstr_text);
	// URLエスケープしておくこと
	easy.escape(text);
	//
	string auth_key = Text::Encoding::utf16_to_utf8(wstr_auth_key);
	string target_lang = Text::Encoding::utf16_to_utf8(wstr_target_lang);
	string source_lang = Text::Encoding::utf16_to_utf8(wstr_source_lang);

	// 簡易版なのでGETで。PythonとPHP版は、POSTにしている。
	string url = "https://api-free.deepl.com/v2/translate"s;
	url += "?auth_key="s + auth_key.data();
	url += "&target_lang="s + target_lang.data();
	url += "&source_lang="s + source_lang.data();
	url += "&text="s + text;

	easy.add<CURLOPT_URL>(url.data());
	// easy.add<CURLOPT_CUSTOMREQUEST>("GET");
	easy.add<CURLOPT_SSL_VERIFYPEER>(FALSE);
	easy.add<CURLOPT_SSL_VERIFYHOST>(FALSE);

	try {
		easy.perform();

		auto status = easy.get_info<CURLINFO_RESPONSE_CODE>();
		string u8result = deserialize_result(stream.str());
		wstring wresult = Text::Encoding::utf8_to_utf16(u8result);
		return wresult;

	}
	catch (curl_easy_exception& error) {
		wstring wresult = Text::Encoding::utf8_to_utf16(error.what());
		return wresult;
	}
}

wstring ret;
extern "C" __declspec(dllexport) cdecl wchar_t* try_translate(wchar_t* auth_key, wchar_t* source_lang, wchar_t* target_lang, wchar_t* override_text) {
	ret.clear();

	try {
		wstring wtext = L"";
		if (override_text) {
			if ((L""s + override_text).length() != 0) {
				wtext = override_text;
			}
		};
		if (wtext.length() == 0) {
			wtext = Hm.Edit.getSelectedText();
		}
		if (wtext.length()== 0) {
			wtext = Hm.Edit.getTotalText();
		}
		ret = translate(auth_key, source_lang, target_lang, wtext);
		return ret.data();
	}
	catch(exception e) {
		ret = L"翻訳エラー" + Text::Encoding::utf8_to_utf16(e.what());;
		return ret.data();
	}
}

