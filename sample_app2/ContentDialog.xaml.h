//
// ContentDialog.xaml.h
// ContentDialog クラスの宣言
//

#pragma once

#include "ContentDialog.g.h"

namespace sample_app2
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ContentDialog sealed
	{
	public:
		ContentDialog();
	private:
		void ContentDialog_PrimaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
		void ContentDialog_SecondaryButtonClick(Windows::UI::Xaml::Controls::ContentDialog^ sender, Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs^ args);
	};
}
