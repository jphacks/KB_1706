﻿//
// MainPage.xaml.cpp
// MainPage クラスの実装。
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace sample_app2;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage;
using namespace Platform::Collections;
using namespace concurrency;

using namespace web::http;
using namespace web::http::client;
using namespace web;

using namespace utility;                    // Common utilities like string conversions


// 空白ページの項目テンプレートについては、https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x411 を参照してください


Platform::String^ i;
Platform::String^ icoin;
TextBlock^  textBlockRegisterResult;

MainPage::MainPage()
{
	InitializeComponent();
}


std::string make_string(const std::wstring& wstring)
{
	auto wideData = wstring.c_str();
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideData, -1, nullptr, 0, NULL, NULL);
	auto utf8 = std::make_unique<char[]>(bufferSize);
	if (0 == WideCharToMultiByte(CP_UTF8, 0, wideData, -1, utf8.get(), bufferSize, NULL, NULL))
		throw std::exception("Can't convert string to UTF8");

	return std::string(utf8.get());
}

std::wstring make_wstring(const std::string& string)
{
	auto utf8Data = string.c_str();
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8Data, -1, nullptr, 0);
	auto wide = std::make_unique<wchar_t[]>(bufferSize);
	if (0 == MultiByteToWideChar(CP_UTF8, 0, utf8Data, -1, wide.get(), bufferSize))
		throw std::exception("Can't convert string to Unicode");

	return std::wstring(wide.get());
}


void txtset(Platform::String^ itxtset)
{
	i = itxtset;
	StorageFolder^ folder = KnownFolders::DocumentsLibrary;
	create_task(folder->GetFileAsync(L"userid.txt")).then([&](StorageFile^ file) //ファイルはPC\ドキュメントの直下を読み込む
	{
		create_task(FileIO::WriteTextAsync(file, i));
		
	});
}

void sample_app2::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	textBlockRegisterResult = (TextBlock^)FindName("register_ok");
	register_ok->Text = "登録完了";

	json::value postData;

	String^ semail;
	String^ spassword1;
	String^ spassword2;

	semail = email->Text;
	spassword1 = password1->Password;
	spassword2 = password2->Password;

	
	
	icoin = ref new Platform::String(semail->Data());

	std::wstring wsstremail(semail->Data());
	auto utf8Str1 = make_string(wsstremail); // UTF8-encoded text
	wsstremail = make_wstring(utf8Str1); // same as original text

	std::wstring wsstrpass1(spassword1->Data());
	auto utf8Str2 = make_string(wsstrpass1); // UTF8-encoded text
	wsstrpass1 = make_wstring(utf8Str2); // same as original text

	std::wstring wsstrpass2(spassword2->Data());
	auto utf8Str3 = make_string(wsstrpass2); // UTF8-encoded text
	wsstrpass2 = make_wstring(utf8Str3); // same as original text

	LPCTSTR str1 = wsstremail.c_str();
	LPCTSTR str2 = wsstrpass1.c_str();
	LPCTSTR str3 = wsstrpass2.c_str();


	
	postData[L"coinadress"] = json::value::string(str1);
	postData[L"onepass"] = json::value::string(str2);
	postData[L"twopass"] = json::value::string(str3);
	
	//http_client mhttpclient(L"http://192.168.179.2:3000/api/customers");

	http_client mhttpclient(L"http://13.115.47.221/api/customers");

	http_request request(methods::POST);
	request.headers().set_content_type(U("application/json"));
	request.set_body(postData);
	
	std::wstring tmp = L"";
	http_response response;
		
	auto resTask = mhttpclient.request(request).then([](http_response response)
	{
		std::wstring s1{ response.extract_string().get().c_str() };
		wprintf(s1.c_str());
		std::wstring string(s1.c_str());
		Platform::String^ p_string = ref new Platform::String(s1.c_str());
		txtset(p_string);

		if (response.status_code() == 200) {
		}
		else {
		}
	});
	
}




void sample_app2::MainPage::Button_PayPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
	Windows::UI::Xaml::Interop::TypeName naviname;
	naviname.Name = ref new String(L"sample_app2.CashPage");
	naviname.Kind = Windows::UI::Xaml::Interop::TypeKind::Custom;
	this->Frame->Navigate(naviname, this);
	

}
