//
// CashPage.xaml.cpp
// CashPage クラスの実装
//

#include "pch.h"
#include "CashPage.xaml.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <Shellapi.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <time.h>
#include <future>
#include <thread>
#include <iostream>
#include <ctime>

using namespace sample_app2;
using namespace std;

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
using namespace std;



using namespace web::http;
using namespace web::http::client;
using namespace web;

// 空白ページの項目テンプレートについては、https://go.microsoft.com/fwlink/?LinkId=234238 を参照してください

int readcount = 0;
TextBlock^  textBlock1;

CashPage::CashPage()
{
	InitializeComponent();
	
}

int num;

void readuserid() {

	StorageFolder^ folder = KnownFolders::DocumentsLibrary;

	create_task(folder->GetFileAsync(L"userid.txt")).then([](StorageFile^ file) //ファイルはPC\ドキュメントの直下を読み込む
	{
		return FileIO::ReadTextAsync(file);
	}).then([](task<String^> previousTask)
	{
		try
		{

			String^ text = previousTask.get();

			Platform::String^ fooRT = text;
			std::wstring fooW(fooRT->Begin());
			std::string fooA(fooW.begin(), fooW.end());
			const char* charStr = fooA.c_str();
			num = atoi(fooA.c_str());
			


		}
		catch (...)
		{
		}

	});
}

std::string name;

void recognition() {

	StorageFolder^ folder = KnownFolders::DocumentsLibrary;

	create_task(folder->GetFileAsync(L"exe\\output.txt")).then([](StorageFile^ file) //ファイルはPC\ドキュメントの直下を読み込む
	{
		return FileIO::ReadTextAsync(file);
	}).then([](task<String^> previousTask)
	{
		try
		{


			
			
			String^ text = previousTask.get();

			Platform::String^ fooRT = text;
			std::wstring fooW(fooRT->Begin());
			std::string fooA(fooW.begin(), fooW.end());
			const char* charStr = fooA.c_str();
			name = fooA;


			vector<string> list;
			stringstream ss(charStr);
			string item;
			bool f = true;
			while (getline(ss, item, ' ') && !item.empty()) {
				if (f == true) {
					f = false;
					name = item;
				}
				list.push_back(item);
			}

			if (name == "Ohira") {
				readuserid();
				textBlock1->Text = "決算完了";
			}
			else {
				textBlock1->Text = "もう一度";
			}

			/*
			vector<string> list;
			stringstream ss(charStr);
			string item;
			while (getline(ss, item, ' ') && !item.empty()) {
				list.push_back(item);
			}
			*/


		}
		catch (...)
		{
		}
		
	});
}



void sample_app2::CashPage::Button_pay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	//textBlock1 = FindName("pay_ok");
	textBlock1 = (TextBlock^)FindName("pay_ok");
	

	recognition();
	


	String^ samount;
	String^ suserdata;

	samount = amount->Text;
	suserdata = amount->Text;
	std::wstring wsstremail(samount->Data());
	std::wstring wsstruserdata(suserdata->Data());
	LPCTSTR str1 = wsstremail.c_str();
	LPCTSTR str2 = wsstruserdata.c_str();


	json::value postData;
	postData[L"id"] = json::value::number(num);
	postData[L"amount"] = json::value::string(str2);
	//http_client mhttpclient(L"http://192.168.179.2:3000/api/customers/pay");
	http_client mhttpclient(L"http://13.115.47.221/api/customers/pay");

	http_request request(methods::POST);
	request.headers().set_content_type(U("application/json"));
	request.set_body(postData);

	auto resTask = mhttpclient.request(request).then([](http_response response)
	{
		std::wstring s1{ response.extract_string().get().c_str() };
		wprintf(s1.c_str());

		if (response.status_code() == 200) {
		}
		else {
		}
	});
	
	
}

void sample_app2::CashPage::Button_RegisterPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	Windows::UI::Xaml::Interop::TypeName naviname;
	naviname.Name = ref new String(L"sample_app2.MainPage");
	naviname.Kind = Windows::UI::Xaml::Interop::TypeKind::Custom;
	this->Frame->Navigate(naviname, this);

}
void sample_app2::CashPage::textcheck_Copy_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}