//
// CashPage.xaml.h
// CashPage クラスの宣言
//

#pragma once

#include "CashPage.g.h"

namespace sample_app2
{
	/// <summary>
	/// それ自体で使用できる空白ページまたはフレーム内に移動できる空白ページ。
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class CashPage sealed
	{
	public:
		CashPage();
	private:
		void Button_pay(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_RegisterPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void textcheck_Copy_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
