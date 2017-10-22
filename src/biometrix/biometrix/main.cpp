#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <winbio.h>
//#include "stdafx.h"
#include <Strsafe.h>
#include <iostream>   // ifstream, ofstream
#include <fstream>   // ifstream, ofstream
#include <sstream>   // istringstream
#include "opencv2\opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#pragma comment(lib, "Winbio.lib")

using namespace std;
using namespace cv;


//-----------------------------------------------------------------------------
// Forward declarations of local functions for Private Pool Setup.

HRESULT CaptureSample();
bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
void thinningIte(Mat& img, int pattern);
void thinning(const Mat& src, Mat& dst);


int main()
{
	int Flag;
	do {
		HRESULT hr = S_OK;
		hr = CaptureSample();//指紋画像取得
		wprintf_s(L"Capture Sample: hr = 0x%x\n", hr);

		std::cin;
		cv::Mat src_img = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/fingerprint.bmp", 1);

		// エラー処理
		if (src_img.empty()) return -1;

		cv::imwrite("src_img.bmp", src_img);
		cv::Mat search_img, img_result,out;
		src_img.copyTo(search_img);
		src_img.copyTo(out);
		out = Scalar(Vec3b(0, 0, 0));
		

		// 特徴点描画用パレット
		std::vector<cv::Vec3b> colors(4);
		colors[0] = cv::Vec3b(0, 0, 255);
		colors[1] = cv::Vec3b(0, 255, 0);
		colors[2] = cv::Vec3b(255, 0, 0);
		colors[3] = cv::Vec3b(0, 255, 255);

		Mat tmp[4];// マニューシャ特徴量パターン
		for (int i = 0; i < 4; i++)
			tmp[i] = Mat(Size(3, 3), CV_8U, Scalar::all(0));// 初期化
		tmp[0].at<uchar>(1, 1) = 255;
		tmp[0].at<uchar>(2, 1) = 255;
		tmp[1].at<uchar>(1, 1) = 255;
		tmp[1].at<uchar>(2, 2) = 255;
		tmp[2].at<uchar>(0, 1) = 255;
		tmp[2].at<uchar>(1, 1) = 255;
		tmp[2].at<uchar>(2, 0) = 255;
		tmp[2].at<uchar>(2, 2) = 255;
		tmp[3].at<uchar>(0, 0) = 255;
		tmp[3].at<uchar>(1, 1) = 255;
		tmp[3].at<uchar>(2, 1) = 255;
		tmp[3].at<uchar>(2, 1) = 255;

		// パターンを画像として保存
		cv::imwrite("tmp1.jpg", tmp[0]);
		cv::imwrite("tmp2.jpg", tmp[1]);
		cv::imwrite("tmp3.jpg", tmp[2]);
		cv::imwrite("tmp4.jpg", tmp[3]);

		cv::cvtColor(search_img, search_img, CV_BGR2GRAY);//　グレースケールに変換
		search_img = ~search_img;// 背景を黒にするために反転
		cv::imwrite("src_gray_img.bmp", search_img);// 元画像を保存
		cv::imshow("src", search_img);// 元画像表示

//		threshold(search_img, search_img, 0, 255, THRESH_BINARY | THRESH_OTSU);//固定値の2値化
	
		// 近傍の平均値に合わせて2値化
		cv::adaptiveThreshold(search_img,                              // 8ビット，シングルチャンネルの入力画像．
			search_img,
			255,                              // 2値化画像を作成する際の０以外の色。ここでは[ 0, 255 ]で作成する.
			cv::ADAPTIVE_THRESH_MEAN_C,   // 近傍の平均値を計算する際に使用するアルゴリズム.ADAPTIVE_THRESH_MEAN_C または ADAPTIVE_THRESH_GAUSSIAN_C.
			cv::THRESH_BINARY,                // 閾値の種類． THRESH_BINARY または THRESH_BINARY_INV のどちらか．
			23,                                // ピクセルの閾値を求めるために利用される近傍領域のサイズ．3, 5, 7, など．
			0                                 // 平均または加重平均から引かれる定数.この値により maxValue の範囲が変わる.
		);

		// 2値化した画像を保存
		cv::imwrite("binary_search_img.bmp", search_img);
		//GaussianBlur(search_img, search_img, (3,3),0);  //　ガウシアンフィルタ
		//medianBlur(search_img, search_img, 3);          // メディアンフィルタ
		//cv::imwrite("Filter_img.bmp", search_img);
		cv::imshow("result", search_img);// 途中経過を表示
		
		cv::Mat element(2, 2, CV_8U, cv::Scalar(1));
		
		// ラベルごとのROIを得る(0番目は背景なので無視)
		cv::Mat roiImg;
		cv::cvtColor(search_img, roiImg, CV_GRAY2BGR);
		std::vector<cv::Rect> roiRects;
		cv::Mat LabelImg;
		cv::Mat stats;
		cv::Mat centroids;
		int nLab = cv::connectedComponentsWithStats(search_img, LabelImg, stats, centroids,8);
		int num = 0;
		int th = 2;
		// ラベリング結果の描画
		for (int i = 1; i < nLab; i++) {
			int *param = stats.ptr<int>(i);
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
			if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] < th) {
				//std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;
				for (int dx = 0; dx < width; dx++) {
					for (int dy = 0; dy < height; dy++) {
						search_img.at<u_char>(y + dy, x + dx) = 0;
					}
				}
			}
			else {
				roiRects.push_back(cv::Rect(x, y, width, height));
				num++;
			}
		}
		cv::imwrite("openning_search_img.bmp", search_img);
		cv::imshow("result2", search_img);
		cv::imwrite("search_img.bmp", search_img);
		int N = 500, sum = 0,count[4] = { 0 };
		double zx[4] = { 0 }, zy[4] = { 0 };
		// テンプレートマッチングで探索
		for (int j = 1; j < 4; j++) {
			cv::Mat &img_reso_koma = tmp[j];
			int ox = -1, oy = -1;

			// 50 個検出する
			for (int i = 0; i < N; i++) {
				// 最大のスコアの場所を探す
				Mat match;
				cv::Point minp, maxp;
				double minVal, maxVal;
				cv::matchTemplate(search_img, img_reso_koma, match, CV_TM_SQDIFF);
				cv::minMaxLoc(match, &minVal, &maxVal, &minp, &maxp);

				// 一定スコア以下の場合は処理終了
				//if (maxVal < 0.1) break;
				if ((minVal > 0.001) || ((ox == minp.x) && (oy == minp.y))) {
					i = N;
					break;
				}
				else count[j]++;
				ox = minp.x;
				oy = minp.y;
				zx[j] += ox;
				zy[j] += oy;
				//wprintf_s(L"\tmatch:%f\n", match);
				//wprintf_s(L"\t%d:minVal%lf\n", i,minVal);
				//wprintf_s(L"\tx:%d,y:%d\n", minp.x,minp.y)

				// 探索結果の場所に矩形を描画
				circle(src_img, Point(minp.x, minp.y), 5, colors[j], 1, 8, 0);
				circle(out, Point(minp.x, minp.y), 5, colors[j], 1, 8, 0);

				// 検出済みは 0.0 で塗りつぶし
				for (int y = 0; y < img_reso_koma.rows; y++) {
					for (int x = 0; x < img_reso_koma.cols; x++) {
						int xx = minp.x + x - img_reso_koma.cols / 2;
						int yy = minp.y + y - img_reso_koma.rows / 2;
						if (0 <= xx && xx < match.cols - 1) {
							if (0 <= yy && yy < match.rows - 1) {
								search_img.at<u_char>(yy, xx) = 0;
							}
						}
					}
				}
			}
		}
		cv::imwrite("out.bmp", out);
		cv::Mat dist = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/search_img.bmp");
		cv::Mat N_tmp = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/Ntmp.bmp");
		cv::Mat O_tmp = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/Otmp.bmp");
		
		for (int i = 1; i < 4; i++) {
			zx[i] = zx[i] / count[i];
			zy[i] = zy[i] / count[i];
			sum += count[i];
			wprintf_s(L"\tcount%d:%d\n", i, count[i]);
			std::cout << "重心"<<i<<":(" << zx[i] << "," << zy[i] << ") " << std::endl;
		}
		double ave = (double)sum / 4;
		wprintf_s(L"\tcount ave:%f\n", ave);
		
		// ユーザテンプレートマッチング
		// 最大のスコアの場所を探す
		Mat match;
		cv::Point minp, maxp;
		double minVal, maxVal;
		
		cv::matchTemplate(out, N_tmp, match, CV_TM_CCOEFF_NORMED);
		cv::minMaxLoc(match, &minVal, &maxVal, &minp, &maxp);
		std::cout << "Val: " << maxVal << ",x: " << maxp.x << ",y: " << maxp.y << std::endl;
		circle(dist, Point(maxp.x, maxp.y), 5, cv::Scalar(0, 0, 255), 1, 8, 0);
		
		// 一定スコア以下の場合は処理終了
		if (maxVal > 0.1) {
			std::cout << "Nishimura" << std::endl;
		}
		else {
			std::cout << "Ohira" << std::endl;
		}
		cv::imwrite("result.bmp", dist);
		
		cv::imshow("result3", dist);
		cv::imshow("output", out);
		cv::imshow("N_tmp", N_tmp);
		cv::imshow("O_tmp", O_tmp);
		cv::waitKey(0);
		// キー入力を（無限に）待つ
		cvDestroyWindow("src");
		cvDestroyWindow("result");
		cvDestroyWindow("result2");
		cvDestroyWindow("result3");
		cvDestroyWindow("N_tmp");
		cvDestroyWindow("O_tmp");
		cvDestroyWindow("output");
		wprintf_s(L"\n Press 1 to repeat.\n Press 0 to exit\n. ");
		std::cin >> Flag;
	}while (Flag);
	return 0;
}

//-----------------------------------------------------------------------------

HRESULT CaptureSample()
{
restart:
	HRESULT hr = S_OK;
	WINBIO_SESSION_HANDLE sessionHandle = NULL;
	WINBIO_UNIT_ID unitId = 0;
	WINBIO_REJECT_DETAIL rejectDetail = 0;
	PWINBIO_BIR sample = NULL;
	SIZE_T sampleSize = 0;
	PWINBIO_UNIT_SCHEMA unitSchema = NULL;
	SIZE_T unitCount = 0;
	SIZE_T index = 0;
	ofstream ofs;
	string name;

	// Connect to the system pool.  
	hr = WinBioOpenSession(
		WINBIO_TYPE_FINGERPRINT,    // Service provider
		WINBIO_POOL_SYSTEM,         // Pool type
		WINBIO_FLAG_RAW,            // Access: Capture raw data
		NULL,                       // Array of biometric unit IDs
		0,                          // Count of biometric unit IDs
		WINBIO_DB_DEFAULT,          // Default database
		&sessionHandle              // [out] Session handle
	);
	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioOpenSession failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}
	// Enumerate the installed biometric units.
	hr = WinBioEnumBiometricUnits(
		WINBIO_TYPE_FINGERPRINT,        // Type of biometric unit
		&unitSchema,                    // Array of unit schemas
		&unitCount);                   // Count of unit schemas

	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioEnumBiometricUnits failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Display information for each installed biometric unit.
	wprintf_s(L"\nSensors: \n");
	for (index = 0; index < unitCount; ++index)
	{
		wprintf_s(L"\n[%d]: \tUnit ID: %d\n",
			index,
			unitSchema[index].UnitId);
	}

	wprintf_s(L"\n Pick a Sensor: ");
	int i;
	std::cin >> i;

	unitId = unitSchema[i].UnitId;

	// Capture a biometric sample.
	wprintf_s(L"\n Calling WinBioCaptureSample - Swipe sensor...\n");
	hr = WinBioCaptureSample(
		sessionHandle,
		WINBIO_NO_PURPOSE_AVAILABLE,
		WINBIO_DATA_FLAG_RAW,
		&unitId,
		&sample,
		&sampleSize,
		&rejectDetail
	);
	if (FAILED(hr))
	{
		if (hr == WINBIO_E_BAD_CAPTURE)
		{
			wprintf_s(L"\n Bad capture; reason: %d\n", rejectDetail);
		}
		else
		{
			wprintf_s(L"\n WinBioCaptureSample failed. hr = 0x%x\n", hr);
		}
		goto e_Exit;
	}

	PWINBIO_BIR_HEADER BirHeader = (PWINBIO_BIR_HEADER)(((PBYTE)sample) + sample->HeaderBlock.Offset); //header points to the offset of the header block
	PWINBIO_BDB_ANSI_381_HEADER AnsiBdbHeader = (PWINBIO_BDB_ANSI_381_HEADER)(((PBYTE)sample) + sample->StandardDataBlock.Offset); //header points to the beginning of the standard data block
	PWINBIO_BDB_ANSI_381_RECORD AnsiBdbRecord = (PWINBIO_BDB_ANSI_381_RECORD)(((PBYTE)AnsiBdbHeader) + sizeof(WINBIO_BDB_ANSI_381_HEADER)); //record points to the record of the standard data block

	PBYTE firstPixel = (PBYTE)((PBYTE)AnsiBdbRecord) + sizeof(WINBIO_BDB_ANSI_381_RECORD); //points to the data of first pixel

	wprintf_s(L" Width: %d\n", AnsiBdbRecord->HorizontalLineLength);
	wprintf_s(L" Height: %d\n", AnsiBdbRecord->VerticalLineLength);
	
	long blocklength = AnsiBdbRecord->BlockLength - sizeof(AnsiBdbRecord); //blocklength is size of raw image data
	bool b = SaveBMP(firstPixel, AnsiBdbRecord->HorizontalLineLength, AnsiBdbRecord->VerticalLineLength, blocklength, "fingerprint.bmp");

	if (b) {
		wprintf_s(L"\n SaveBMP succeeded");
	}
	else {
		wprintf_s(L"\n SaveBMP failed");
	}


e_Exit:
	if (sample != NULL)
	{
		WinBioFree(sample);
		sample = NULL;
	}
	if (unitSchema != NULL)
	{
		WinBioFree(unitSchema);
		unitSchema = NULL;
	}
	if (sessionHandle != NULL)
	{
		WinBioCloseSession(sessionHandle);
		sessionHandle = NULL;
	}

	wprintf_s(L"\n Press 1 to repeat.\n Press 0 to exit\n. ");
	std::cin >> i;

	if (i == 1) {
		goto restart;
	}

	return hr;
}

bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile)
{

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
	memset(&info, 0, sizeof(BITMAPINFOHEADER));
	//Next we fill the file header with data:
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;
	//and the info header:
	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;

	info.biBitCount = 8;
	info.biCompression = 0;

	info.biSizeImage = 0;
	
	info.biXPelsPerMeter = 0x0ec4;
	info.biYPelsPerMeter = 0x0ec4;

	//info.biXPelsPerMeter = 500;
	//info.biYPelsPerMeter = 500;
	info.biClrUsed = 0;

	info.biClrImportant = 0;

	HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//Now we write the file header and info header:
	unsigned long bwritten;
	if (WriteFile(file, &bmfh, sizeof(BITMAPFILEHEADER),
		&bwritten, NULL) == false)
	{
		CloseHandle(file);
		return false;
	}

	if (WriteFile(file, &info, sizeof(BITMAPINFOHEADER),
		&bwritten, NULL) == false)
	{
		CloseHandle(file);
		return false;
	}
	//and finally the image data:
	if (WriteFile(file, Buffer, paddedsize, &bwritten, NULL) == false)
	{
		CloseHandle(file);
		return false;
	}
	//Now we can close our function with
	CloseHandle(file);
	return true;
}

void thinningIte(Mat& img, int pattern) {
	Mat del_marker = Mat::ones(img.size(), CV_8UC1);
	int x, y;

	for (y = 1; y < img.rows - 1; ++y) {
		for (x = 1; x < img.cols - 1; ++x) {

			int v9, v2, v3;
			int v8, v1, v4;
			int v7, v6, v5;

			v1 = img.data[y   * img.step + x   * img.elemSize()];
			v2 = img.data[(y - 1) * img.step + x   * img.elemSize()];
			v3 = img.data[(y - 1) * img.step + (x + 1) * img.elemSize()];
			v4 = img.data[y   * img.step + (x + 1) * img.elemSize()];
			v5 = img.data[(y + 1) * img.step + (x + 1) * img.elemSize()];
			v6 = img.data[(y + 1) * img.step + x   * img.elemSize()];
			v7 = img.data[(y + 1) * img.step + (x - 1) * img.elemSize()];
			v8 = img.data[y   * img.step + (x - 1) * img.elemSize()];
			v9 = img.data[(y - 1) * img.step + (x - 1) * img.elemSize()];

			int S = (v2 == 0 && v3 == 1) + (v3 == 0 && v4 == 1) +
				(v4 == 0 && v5 == 1) + (v5 == 0 && v6 == 1) +
				(v6 == 0 && v7 == 1) + (v7 == 0 && v8 == 1) +
				(v8 == 0 && v9 == 1) + (v9 == 0 && v2 == 1);

			int N = v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9;

			int m1 = 0, m2 = 0;

			if (pattern == 0) m1 = (v2 * v4 * v6);
			if (pattern == 1) m1 = (v2 * v4 * v8);

			if (pattern == 0) m2 = (v4 * v6 * v8);
			if (pattern == 1) m2 = (v2 * v6 * v8);

			if (S == 1 && (N >= 2 && N <= 6) && m1 == 0 && m2 == 0)
				del_marker.data[y * del_marker.step + x * del_marker.elemSize()] = 0;
		}
	}
	img &= del_marker;
}

void thinning(const Mat& src, Mat& dst) {
	dst = src.clone();
	dst /= 255;         // 0は0 , 1以上は1に変換される

	Mat prev = Mat::zeros(dst.size(), CV_8UC1);
	Mat diff;

	do {
		thinningIte(dst, 0);
		thinningIte(dst, 1);
		absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (countNonZero(diff) > 0);

	dst *= 255;
}

