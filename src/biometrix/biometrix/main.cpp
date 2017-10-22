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
//bool SaveBMP(BYTE* buffer, int width, int height, int pixelPerVertical, int pixelPerHorizontal, long paddedsize, LPCTSTR bmpfile, BYTE* firstPixel);
bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
void thinningIte(Mat& img, int pattern);
void thinning(const Mat& src, Mat& dst);

/*
int g_MeanBlockSize = 0;
int g_MeanOffset = 0;
int g_GaussianBlockSize = 0;
int g_GaussianOffset = 0;
*/
//-----------------------------------------------------------------------------


int main()
{
	int Flag;
	do {
		HRESULT hr = S_OK;
		hr = CaptureSample();
		wprintf_s(L"Capture Sample: hr = 0x%x\n", hr);

		std::cin;
		cv::Mat src_img = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/fingerprint.bmp", 1);
		cv::imwrite("src_img.bmp", src_img);
		cv::Mat search_img, img_result,out;
		src_img.copyTo(search_img);
		src_img.copyTo(out);
		out = Scalar(Vec3b(0, 0, 0));
		Mat tmp[4];// テンプレート画像の取得

		int count[4] = { 0 };
		for (int i = 0; i < 4; i++)
			tmp[i] = Mat(Size(3, 3), CV_8U, Scalar::all(0));// テンプレート画像の取得
		//tmp[i].at<uchar>(ｙ, x);
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
		cv::imwrite("tmp1.jpg", tmp[0]);
		cv::imwrite("tmp2.jpg", tmp[1]);
		cv::imwrite("tmp3.jpg", tmp[2]);
		cv::imwrite("tmp4.jpg", tmp[3]);

		if (src_img.empty()) return -1;

		/// 画像を表示するウィンドウ
		// ウィンドウの名前，プロパティ
		// CV_WINDOW_AUTOSIZE : ウィンドウサイズを画像サイズに合わせる
		// CV_WINDOW_FREERATIO : ウィンドウのアスペクト比を固定しない
		//cv::namedWindow("image1", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
		cv::cvtColor(search_img, search_img, CV_BGR2GRAY);
		search_img = ~search_img;
		cv::imwrite("src_gray_img.bmp", search_img);
		//medianBlur(search_img, search_img, 3);          // メディアンフィルタ
		//cv::imwrite("Filter_img.bmp", search_img);
		cv::imshow("src", search_img);

		// ウィンドウ名でウィンドウを指定して，そこに画像を描画
		//cv::imshow("image1", src_img);
//		threshold(search_img, search_img, 0, 255, THRESH_BINARY | THRESH_OTSU);
	
		cv::adaptiveThreshold(search_img,                              // 8ビット，シングルチャンネルの入力画像．
			search_img,
			255,                              // 2値化画像を作成する際の０以外の色。ここでは[ 0, 255 ]で作成する.
			cv::ADAPTIVE_THRESH_MEAN_C,   // 近傍の平均値を計算する際に使用するアルゴリズム.ADAPTIVE_THRESH_MEAN_C または ADAPTIVE_THRESH_GAUSSIAN_C.
			cv::THRESH_BINARY,                // 閾値の種類． THRESH_BINARY または THRESH_BINARY_INV のどちらか．
			23,                                // ピクセルの閾値を求めるために利用される近傍領域のサイズ．3, 5, 7, など．
			0                                 // 平均または加重平均から引かれる定数.この値により maxValue の範囲が変わる.
		);
		cv::imwrite("binary_search_img.bmp", search_img);
		//GaussianBlur(search_img, search_img, (3,3),0);
		//medianBlur(search_img, search_img, 3);          // メディアンフィルタ
		//cv::imwrite("Filter_img.bmp", search_img);
		cv::imshow("result", search_img);
		
		//cv::Mat element(2, 2, CV_8U, cv::Scalar(1));
		//cv::morphologyEx(search_img, search_img, MORPH_CLOSE, element, cv::Point(-1, -1), 1);
		//cv::imshow("dilate", search_img);
		
		//thinning(search_img, search_img);
		//cv::imwrite("thin_search_img.bmp", search_img);
		//Canny(search_img, search_img, 50, 200);
		//cv::imwrite("canny_img.bmp", search_img);

		cv::imshow("result2", search_img);
		//cv::imwrite("dilate_img.bmp", search_img);
		
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
		for (int i = 0; i < nLab; i++) {
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
				//cv::rectangle(src_img, roiRects.at(i - 1), cv::Scalar(0, 255, 0), 2);
			}
		}
		cv::imwrite("openning_search_img.bmp", search_img);
		cv::imshow("result3", search_img);

		//thinning(search_img, search_img); 
		cv::imshow("result4", search_img);
		/*
		cv::Mat e(2, 2, CV_8U, cv::Scalar(1));
		cv::erode(search_img, search_img, e, cv::Point(-1, -1), 1);
		cv::imwrite("mofo.bmp", search_img);
		cv::dilate(search_img, search_img, e, cv::Point(-1, -1), 1);
		cv::imwrite("dilate_img.bmp", search_img);

		//thinning(search_img, search_img);
		//cv::imwrite("thin_search_img.bmp", search_img);


		cv::Mat element2(2, 2, CV_8U, cv::Scalar(1));
		cv::dilate(search_img, search_img, element2, cv::Point(-1, -1), 2);
		cv::imwrite("dilate_img2.bmp", search_img);
		cv::erode(search_img, search_img, element2, cv::Point(-1, -1), 2);
		cv::imwrite("mofo2.bmp", search_img);

		*/
		cv::imwrite("search_img.bmp", search_img);
		int N = 500, sum = 0;
		double zx = 0, zy = 0;
		// テンプレートマッチングで探索
		for (int j = 0; j < 4; j++) {
			cv::Mat &img_reso_koma = tmp[j];
			int ox = -1, oy = -1;

			// 50 個検出する
			for (int i = 0; i < N; i++) {
				// 最大のスコアの場所を探す
				Mat match;
				cv::Point minp, maxp;
				double minVal, maxVal;
				//cv::matchTemplate(search_img, img_reso_koma, match, CV_TM_CCOEFF_NORMED);
				cv::matchTemplate(search_img, img_reso_koma, match, CV_TM_SQDIFF);
				cv::minMaxLoc(match, &minVal, &maxVal, &minp, &maxp);

				// 一定スコア以下の場合は処理終了
				//if (maxVal < 0.1) break;
				if ((minVal > 0.01) || ((ox == minp.x) && (oy == minp.y))) {
					i = N;
					break;
				}
				else count[j]++;
				ox = minp.x;
				oy = minp.y;
				zx += ox;
				zy += oy;
				//wprintf_s(L"\tmatch:%f\n", match);
				//wprintf_s(L"\t%d:minVal%lf\n", i,minVal);
				//
				//wprintf_s(L"\tx:%d,y:%d\n", minp.x,minp.y);
				// 探索結果の場所に矩形を描画
				circle(src_img, Point(minp.x, minp.y), 5, cv::Scalar(0, 0, 255), 1, 8, 0);
				circle(out, Point(minp.x, minp.y), 5, cv::Scalar(0, 0, 255), 1, 8, 0);

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
		cv::Mat src = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/out.bmp");
		cv::cvtColor(out, out, CV_BGR2GRAY);
		cv::cvtColor(src, src, CV_BGR2GRAY);
		cv::Mat dist = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/search_img.bmp");
		cv::Mat user_tmp = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/search_img.bmp");
		//cv::Mat user_tmp = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/Ohiratmp.bmp");
		cv::cvtColor(user_tmp, user_tmp, CV_BGR2GRAY);
		// ユーザテンプレートマッチング
		
		//std::cout << "Rect: " << roiRects.at(i - 1) << std::endl;
		//cv::Mat roi = src(roiRects.at(i - 1));    // 対象とするブロブの領域取り出し
		//cv::Mat roi;
		// 切り抜く位置と大きさの指定はcv::Rectを用いる
		//cv::Rect rect(0,0,user_tmp.rows, user_tmp.cols);
		// 下記のようにcv::Matを作成すると，一部切り出せる
		//cv::Mat imgSub(src, roi);
		//double similarity = cv::matchShapes(user_tmp, roi, CV_CONTOURS_MATCH_I2, 0);// huモーメントによるマッチング
		//std::cout << "Similarity: " << similarity << std::endl;

		for (int i = 0; i < 4; i++) {
			sum += count[i];
			wprintf_s(L"\tcount%d:%d\n", i, count[i]);
		}
		double ave = (double)sum / 4;
		wprintf_s(L"\tcount ave:%f\n", ave);
		std::cout << "重心:(" << zx / sum << "," << zy / sum << ") " << std::endl;

		// 最大のスコアの場所を探す
		/*
		Mat match;
		cv::Point minp, maxp;
		double minVal, maxVal;
		//cv::matchTemplate(search_img, img_reso_koma, match, CV_TM_CCOEFF_NORMED);
		cv::matchTemplate(src, user_tmp, match, CV_TM_CCOEFF_NORMED);
		cv::minMaxLoc(match, &minVal, &maxVal, &minp, &maxp);
		// 一定スコア以下の場合は処理終了
		//if (maxVal < 0.1) break;
		std::cout << "Val: " << maxVal << ",x: " << maxp.x << ",y: " << maxp.y << std::endl;
		circle(dist, Point(maxp.x, maxp.y), 5, cv::Scalar(0, 0, 255), 1, 8, 0);
		if (maxVal > 0.6) {
			std::cout << "Match!! " << std::endl;
		}
		else std::cout << "No Match " << std::endl;
		*/
		if (ave > 30) {
			std::cout << "Nishimura " << std::endl;
		}
		else std::cout << "Ohira" << std::endl;
		cv::imwrite("result.bmp", src_img);
		
		cv::imshow("result5", src_img);
		cv::imshow("output", out);
		cv::imshow("user_tmp", user_tmp);
		cv::waitKey(0);
		// キー入力を（無限に）待つ
		cvDestroyWindow("src");
		cvDestroyWindow("result");
		cvDestroyWindow("result2");
		cvDestroyWindow("result3");
		cvDestroyWindow("result4");
		cvDestroyWindow("result5");
		cvDestroyWindow("user_tmp");
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
		/*
		wprintf_s(L"\tDevice instance ID: %s\n",
			unitSchema[index].DeviceInstanceId);
		wprintf_s(L"\tPool type: %d\n",
			unitSchema[index].PoolType);
		wprintf_s(L"\tBiometric factor: %d\n",
			unitSchema[index].BiometricFactor);
		wprintf_s(L"\tSensor subtype: %d\n",
			unitSchema[index].SensorSubType);
		wprintf_s(L"\tSensor capabilities: 0x%08x\n",
			unitSchema[index].Capabilities);
		wprintf_s(L"\tDescription: %s\n",
			unitSchema[index].Description);
		wprintf_s(L"\tManufacturer: %s\n",
			unitSchema[index].Manufacturer);
		wprintf_s(L"\tModel: %s\n",
			unitSchema[index].Model);
		wprintf_s(L"\tSerial no: %s\n",
			unitSchema[index].SerialNumber);
		wprintf_s(L"\tFirmware version: [%d.%d]\n",
			unitSchema[index].FirmwareVersion.MajorVersion,
			unitSchema[index].FirmwareVersion.MinorVersion);
		*/
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

	//wprintf_s(L"\n Swipe processed - Unit ID: %d\n", unitId);
	//wprintf_s(L"\n Captured %d bytes.\n", sampleSize);
	//wprintf_s(L"\n Captured %d bytes.\n", sample);

	PWINBIO_BIR_HEADER BirHeader = (PWINBIO_BIR_HEADER)(((PBYTE)sample) + sample->HeaderBlock.Offset); //header points to the offset of the header block
	PWINBIO_BDB_ANSI_381_HEADER AnsiBdbHeader = (PWINBIO_BDB_ANSI_381_HEADER)(((PBYTE)sample) + sample->StandardDataBlock.Offset); //header points to the beginning of the standard data block
	PWINBIO_BDB_ANSI_381_RECORD AnsiBdbRecord = (PWINBIO_BDB_ANSI_381_RECORD)(((PBYTE)AnsiBdbHeader) + sizeof(WINBIO_BDB_ANSI_381_HEADER)); //record points to the record of the standard data block

	PBYTE firstPixel = (PBYTE)((PBYTE)AnsiBdbRecord) + sizeof(WINBIO_BDB_ANSI_381_RECORD); //points to the data of first pixel

	//wprintf_s(L"\n ID: %d\n", AnsiBdbHeader->ProductId.Owner);
	//wprintf_s(L" Horizontal Img. Res.: %d\n", AnsiBdbHeader->HorizontalImageResolution);
	//wprintf_s(L" Horizontal Scan Img. Res.: %d pixels/centimeter\n", AnsiBdbHeader->HorizontalScanResolution);
	//wprintf_s(L" Vertical Img. Res.: %d\n", AnsiBdbHeader->VerticalImageResolution);
	//wprintf_s(L" Vertical Scan Img. Res.: %d pixels/centimeter\n", AnsiBdbHeader->VerticalScanResolution);
	//wprintf_s(L" Pixel Depth: %d\n", AnsiBdbHeader->PixelDepth); //number of bits in one pixel
	//wprintf_s(L" Element Count: %d\n", AnsiBdbHeader->ElementCount); //number of images

	wprintf_s(L" Width: %d\n", AnsiBdbRecord->HorizontalLineLength);
	wprintf_s(L" Height: %d\n", AnsiBdbRecord->VerticalLineLength);
	//wprintf_s(L" Blocklength: %d\n", AnsiBdbRecord->BlockLength);
	//wprintf_s(L" Compression Algorithm: %d\n", AnsiBdbHeader->ImageCompressionAlg);

	//wprintf_s(L" Address of First Pixel in HEX: 0x%x\n", firstPixel);
	//wprintf_s(L" Address of First Pixel in DEC: %d\n", firstPixel);
	//wprintf_s(L" WINBIO_BDB_ANSI_381_RECORD: %d\n", sizeof(AnsiBdbRecord));

	//AnsiBdbRecord->BlockLength is size of data + record header
	long blocklength = AnsiBdbRecord->BlockLength - sizeof(AnsiBdbRecord); //blocklength is size of raw image data
																		   //long size = sizeof(char) * 4;

	//bool b = SaveBMP(firstPixel, AnsiBdbRecord->HorizontalLineLength, AnsiBdbRecord->VerticalLineLength, AnsiBdbRecord->VerticalLineLength, AnsiBdbRecord->HorizontalLineLength, blocklength, "fingerprint.bmp", firstPixel);
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
	/*
	info.biXPelsPerMeter = 0x0ec4;
	info.biYPelsPerMeter = 0x0ec4;
	*/
	info.biXPelsPerMeter = 500;
	info.biYPelsPerMeter = 500;
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

