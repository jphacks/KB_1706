//
// CashPage.xaml.cpp
// CashPage クラスの実装
//

#include "pch.h"
#include "CashPage.xaml.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <Windows.h>
#include <Shellapi.h>

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

// 空白ページの項目テンプレートについては、https://go.microsoft.com/fwlink/?LinkId=234238 を参照してください



CashPage::CashPage()
{
	InitializeComponent();
}




void sample_app2::CashPage::Button_RegisterPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	/*
	Windows::UI::Xaml::Interop::TypeName naviname;
	naviname.Name = ref new String(L"sample_app2.MainPage");
	naviname.Kind = Windows::UI::Xaml::Interop::TypeKind::Custom;
	this->Frame->Navigate(naviname, this);
	*/

	json::value postData;

	String^ samount;
	String^ suserdata;

	samount = amount->Text;
	suserdata = amount->Text;
	std::wstring wsstremail(samount->Data());
	std::wstring wsstruserdata(suserdata->Data());
	LPCTSTR str1 = wsstremail.c_str();
	LPCTSTR str2 = wsstruserdata.c_str();
	postData[L"id"] = json::value::number(32);
	postData[L"amount"] = json::value::string(str2);
	http_client mhttpclient(L"http://192.168.179.2:3000/api/customers/pay");

	http_request request(methods::POST);
	request.headers().set_content_type(U("application/json"));
	request.set_body(postData);

	auto resTask = mhttpclient.request(request).then([](http_response response)
	{
		std::wstring s1{ response.extract_string().get().c_str() };
		wprintf(s1.c_str());
	});

	pay_ok->Text = "決算完了";
	//ShellExecute(NULL, "open", "D:\\test.txt", NULL, NULL, SW_SHOWNORMAL);
}


void sample_app2::CashPage::textcheck_Copy_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}