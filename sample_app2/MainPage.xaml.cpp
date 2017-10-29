//
// MainPage.xaml.cpp
// MainPage クラスの実装。
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <string>


using namespace std;
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

using namespace web::http;
using namespace web::http::client;
using namespace web;


// 空白ページの項目テンプレートについては、https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x411 を参照してください


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

void sample_app2::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
	json::value postData;

	String^ semail;
	String^ spassword1;
	String^ spassword2;

	semail = email->Text;
	spassword1 = password1->Password;
	spassword2 = password2->Password;

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

	/*
	postData[L"name"] = json::value::string(L"visualstudio");
	postData[L"email"] = json::value::string(L"visualstudionew@gmail.com");
	postData[L"password"] = json::value::string(L"1111111111");
	*/
	/*
	postData[L"name"] = json::value::string(str1);
	postData[L"email"] = json::value::string(str2);
	postData[L"password"] = json::value::string(L"1111111111");
	*/

	
	postData[L"coinadress"] = json::value::string(str1);
	postData[L"onepass"] = json::value::string(str2);
	postData[L"twopass"] = json::value::string(str3);
	
	http_client mhttpclient(L"http://192.168.179.2:3000/api/customers");

	http_request request(methods::POST);
	request.headers().set_content_type(U("application/json"));
	request.set_body(postData);
	
	auto resTask = mhttpclient.request(request).then([](http_response response) 
	{
		std::wstring s1{ response.extract_string().get().c_str() };
		wprintf(s1.c_str());
	});

	register_ok->Text = "登録完了";

}






void sample_app2::MainPage::Button_PayPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
	Windows::UI::Xaml::Interop::TypeName naviname;
	naviname.Name = ref new String(L"sample_app2.CashPage");
	naviname.Kind = Windows::UI::Xaml::Interop::TypeKind::Custom;
	this->Frame->Navigate(naviname, this);
	

}
