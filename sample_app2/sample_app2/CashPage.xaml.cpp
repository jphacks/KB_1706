//
// CashPage.xaml.cpp
// CashPage クラスの実装
//

#include "pch.h"
#include "CashPage.xaml.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

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
	postData[L"amount"] = json::value::string(str1);
	postData[L"userdata"] = json::value::string(str2);
	http_client mhttpclient(L"http://54.148.153.249/api/auth");

	http_request request(methods::POST);
	request.headers().set_content_type(U("application/json"));
	request.set_body(postData);

	auto resTask = mhttpclient.request(request).then([](http_response response)
	{
		return response.status_code();
	});

}
