//
// MainPage.xaml.h
// MainPage クラスの宣言。
//

#pragma once

#include "MainPage.g.h"

namespace sample_app2
{
	/// <summary>
	/// それ自体で使用できる空白ページまたはフレーム内に移動できる空白ページ。
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Button_PayPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
