#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <winbio.h>
#include <Strsafe.h>
#include <iostream>   // ifstream, ofstream
#include <fstream>   // ifstream, ofstream
#include <sstream>   // istringstream
#include "opencv2\opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>

#pragma comment(lib, "Winbio.lib")

using namespace std;
using namespace cv;

void thinning(const cv::Mat& src, cv::Mat& dst){//細線化処理関数
	dst = src.clone();
	cv::Mat *kpb = new cv::Mat[8];
	cv::Mat *kpw = new cv::Mat[8];
	kpb[0] = (cv::Mat_<float>(3,3) << 1,1,0,1,0,0,0,0,0);
	kpb[1] = (cv::Mat_<float>(3,3) << 1,1,1,0,0,0,0,0,0);
	kpb[2] = (cv::Mat_<float>(3,3) << 0,1,1,0,0,1,0,0,0);
	kpb[3] = (cv::Mat_<float>(3,3) << 0,0,1,0,0,1,0,0,1);
	kpb[4] = (cv::Mat_<float>(3,3) << 0,0,0,0,0,1,0,1,1);
	kpb[5] = (cv::Mat_<float>(3,3) << 0,0,0,0,0,0,1,1,1);
	kpb[6] = (cv::Mat_<float>(3,3) << 0,0,0,1,0,0,1,1,0);
	kpb[7] = (cv::Mat_<float>(3,3) << 1,0,0,1,0,0,1,0,0);

	kpw[0] = (cv::Mat_<float>(3,3) << 0,0,0,0,1,1,0,1,0);
	kpw[1] = (cv::Mat_<float>(3,3) << 0,0,0,0,1,0,1,1,0);
	kpw[2] = (cv::Mat_<float>(3,3) << 0,0,0,1,1,0,0,1,0);
	kpw[3] = (cv::Mat_<float>(3,3) << 1,0,0,1,1,0,0,0,0);
	kpw[4] = (cv::Mat_<float>(3,3) << 0,1,0,1,1,0,0,0,0);
	kpw[5] = (cv::Mat_<float>(3,3) << 0,1,1,0,1,0,0,0,0);
	kpw[6] = (cv::Mat_<float>(3,3) << 0,1,0,0,1,1,0,0,0);
	kpw[7] = (cv::Mat_<float>(3,3) << 0,0,0,0,1,1,0,0,1);

	cv::Mat src_w(src.rows,src.cols, CV_32FC1);
	cv::Mat src_b(src.rows,src.cols, CV_32FC1);
	cv::Mat src_f(src.rows,src.cols, CV_32FC1);
	src.convertTo(src_f, CV_32FC1);
	src_f.mul(1. / 255.);
	cv::threshold(src_f, src_f, 0.5, 1.0, CV_THRESH_BINARY);
	cv::threshold(src_f, src_w, 0.5, 1.0, CV_THRESH_BINARY);
	cv::threshold(src_f, src_b, 0.5, 1.0, CV_THRESH_BINARY_INV);

	double sum = 1;
	while (sum>0) {
		sum = 0;
		for (int i = 0; i<8; i++) {
			cv::filter2D(src_w, src_w, CV_32FC1, kpw[i]);
			cv::filter2D(src_b, src_b, CV_32FC1, kpb[i]);
			cv::threshold(src_w, src_w, 2.99, 1.0, CV_THRESH_BINARY);
			cv::threshold(src_b, src_b, 2.99, 1.0, CV_THRESH_BINARY);
			cv::bitwise_and(src_w, src_b, src_w);
			sum += cv::sum(src_w).val[0];
			cv::bitwise_xor(src_f, src_w, src_f);
			src_f.copyTo(src_w);
			cv::threshold(src_f, src_b, 0.5, 1.0, CV_THRESH_BINARY_INV);
		}
	}
	src_f.convertTo(src_f, CV_8U,255);//画像形式を元に戻す
	dst &= src_f;//参照渡し
}

bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile){//付属デバイス用
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

HRESULT CaptureSample(bool f){//付属デバイス用
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
	wprintf_s(L" AnsiBdbRecord: %ld\n", sizeof(AnsiBdbRecord));
	wprintf_s(L" Blocklength: %ld\n", blocklength);
	//bool b = SaveBMP(firstPixel, AnsiBdbRecord->HorizontalLineLength, AnsiBdbRecord->VerticalLineLength, blocklength, "fingerprint.bmp");
	bool b = SaveBMP(firstPixel, AnsiBdbRecord->HorizontalLineLength, AnsiBdbRecord->VerticalLineLength, 300000, "fingerprint.bmp");
	//bool b = SaveBMP(firstPixel, AnsiBdbRecord->HorizontalLineLength, AnsiBdbRecord->VerticalLineLength, 8000, "fingerprint.bmp");

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



bool SaveBMP(BYTE* buffer, int width, int height, int pixelPerVertical, int pixelPerHorizontal, long data_size, LPCTSTR bmpfile, BYTE* firstPixel) {//USB指紋認証デバイス用
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
	memset(&info, 0, sizeof(BITMAPINFOHEADER));

	//long sizeOfColorTable = 4*255*sizeof(char); //size of the color table is 00-FF with a four byte coding RGB0
	int padding = 4 - (width % 4); //calculate the padding needed

	bmfh.bfType = 19778; // Don't question it. Magic Word (B and M). It's necessary. Seriously.
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + data_size; //size of the whole bitmap
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); //start position of data after the header

	info.biSize = sizeof(BITMAPINFOHEADER); //size of the bitmap info header
	info.biWidth = width; //width of the bitmap
	info.biHeight = height; //height of the bitmap
	info.biPlanes = 1; //dimensions of the image
	info.biBitCount = 8; //encoding of raw data -> 8 bits per pixel
	info.biCompression = 0; //compression algorithm
	info.biSizeImage = (width * sizeof(char) + padding * sizeof(char))*height; //size of image data in bytes including padding
	info.biXPelsPerMeter = pixelPerHorizontal;
	info.biYPelsPerMeter = pixelPerVertical;
	info.biClrUsed = 0;
	info.biClrImportant = 0;


	ofstream bmp("fingerprint.bmp", ios::binary | ios::trunc | ios::out); //open as output and binary file and delete everything in the current .bmp file

	if (bmp.is_open()) {
		wprintf_s(L"\n Opening BMP successful\n");
		bmp.write((char*)&bmfh, sizeof(BITMAPFILEHEADER)); //write 
		bmp.write((char*)&info, sizeof(BITMAPINFOHEADER)); //write BitmapInfoHeader
	}
	else {
		wprintf_s(L"\n Opening BMP failed\n");
		return false;
	}

	//char pixelArray[pWidth][pHeight]; //create a pixel array with the size of the image
	char pixelArray[1];
	BYTE* pixel = new BYTE;
	pixel = &(*firstPixel); //new pointer to the first pixel

	int* x = 0;
	////grey index color array
	for (int i = 0; i < 256; i++) {
		bmp.write((char*)&i, sizeof(char)); //write red
		bmp.write((char*)&i, sizeof(char)); //write green
		bmp.write((char*)&i, sizeof(char)); //write blue
		bmp.write((char*)&x, sizeof(char)); //write 0
											//wprintf(L"\n Saving Color: %x %x %x %x", i, i, i, 0);
	}


	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			memset(&pixelArray[0], *pixel, sizeof(char)); //get data from sample and save into array 
			bmp.write((char*)&pixelArray[0], sizeof(char)); //save pixel to bitmap
			pixel = &(*pixel) + sizeof(char); //set pointer to the next byte
											  //wprintf(L"\n Saving PixelArray[%d][%d] 0x%x", j, i, *pixel);
		}
		//113 pixels * 3 bytes = 339 bytes -> missing 1 byte (bio_key) because 24bit/3byte pixel depth
		//114 pixels * 1 byte = 114 bytes -> missing 2 bytes (small sensor) because 8bit/1byte pixel depth
		for (int j = 0; j < padding; j++) {
			bmp.write((char*)&x, sizeof(char)); //pad to a multiple of 4 bytes
												//wprintf(L"\n Saving PixelArray[%d][%d] 0x%x", 115, i, 0);
		}
	}
	bmp.close();
	return true;
}


HRESULT CaptureSample(){///USB指紋認証デバイス用
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

	wprintf_s(L"\n Swipe processed - Unit ID: %d\n", unitId);
	wprintf_s(L"\n Captured %d bytes.\n", sampleSize);

	PWINBIO_BIR_HEADER BirHeader = (PWINBIO_BIR_HEADER)(((PBYTE)sample) + sample->HeaderBlock.Offset); //header points to the offset of the header block
	PWINBIO_BDB_ANSI_381_HEADER AnsiBdbHeader = (PWINBIO_BDB_ANSI_381_HEADER)(((PBYTE)sample) + sample->StandardDataBlock.Offset); //header points to the beginning of the standard data block
	PWINBIO_BDB_ANSI_381_RECORD AnsiBdbRecord = (PWINBIO_BDB_ANSI_381_RECORD)(((PBYTE)AnsiBdbHeader) + sizeof(WINBIO_BDB_ANSI_381_HEADER)); //record points to the record of the standard data block

	BYTE* firstPixel = (BYTE*)((BYTE*)AnsiBdbRecord) + sizeof(WINBIO_BDB_ANSI_381_RECORD); //points to the data of first pixel

	//AnsiBdbRecord->BlockLength is size of data + record header
	long blocklength = AnsiBdbRecord->BlockLength - sizeof(AnsiBdbRecord); //blocklength is size of raw image data
																		   //long size = sizeof(char) * 4;

	bool b = SaveBMP(firstPixel, AnsiBdbRecord->HorizontalLineLength, AnsiBdbRecord->VerticalLineLength, AnsiBdbRecord->VerticalLineLength, AnsiBdbRecord->HorizontalLineLength, blocklength*2, "C:\\Users\ChickenDuy\Documents\bio_key_extraction\bio_key_extraction\bio_key_extraction\fingerprint.bmp", firstPixel);

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

	wprintf_s(L"\n Press 0 to repeat.\n Press 1 to exit\n. ");
	std::cin >> i;

	if (i == 0) {
		goto restart;
	}

	return hr;
}
void myGrayscaleImage(InputArray _matSrc, OutputArray _matDst) {//位相相関マッチング用2値画像変換関数
	Mat matSrc = _matSrc.getMat();
	Mat matDst = _matDst.getMat();
		for (int sy = 0; sy < matSrc.rows; sy++) {
	         Vec3b* srcData = matSrc.ptr<Vec3b>(sy);
	         double* dstData = matDst.ptr<double>(sy);
	         for (int sx = 0; sx < matSrc.cols; sx++) {
		            double b = srcData[sx][0];
		            double g = srcData[sx][1];
		            double r = srcData[sx][2];
		            dstData[sx] = (0.299 * r + 0.587 * g + 0.114 * b) / 255.0;
		}
	}
    return;
}

void match(double* Nres, double* Sres, double* Ores) {
	HRESULT hr = S_OK;
	hr = CaptureSample();//指紋画像取得
	wprintf_s(L"Capture Sample: hr = 0x%x\n", hr);

	std::cin;
	cv::Mat src_img = cv::imread("fingerprint.bmp", 1);

	cv::imwrite("src_img.bmp", src_img);
	cv::Mat search_img, img_result, out;
	src_img.copyTo(search_img);
	src_img.copyTo(out);
	out = Scalar(Vec3b(0, 0, 0));

	cv::cvtColor(search_img, search_img, CV_BGR2GRAY);//　グレースケールに変換
	search_img = ~search_img;// 背景を黒にするために反転
							 // 切り抜く位置と大きさの指定はcv::Rectを用いる
	cv::Rect rect(0, 0, 90, 90);
	// 下記のようにcv::Matを作成すると，一部切り出せる
	cv::Mat imgSub(search_img, rect);
	search_img = imgSub.clone();
	GaussianBlur(search_img, search_img, cv::Size(3, 3), 0);  //　ガウシアンフィルタ
															  //medianBlur(~search_img, search_img, 2);          // メディアンフィルタ
	
	// 近傍の平均値に合わせて2値化
	cv::adaptiveThreshold(search_img,                              // 8ビット，シングルチャンネルの入力画像．
		search_img,
		255,                              // 2値化画像を作成する際の０以外の色。ここでは[ 0, 255 ]で作成する.
		cv::ADAPTIVE_THRESH_MEAN_C,   // 近傍の平均値を計算する際に使用するアルゴリズム.ADAPTIVE_THRESH_MEAN_C または ADAPTIVE_THRESH_GAUSSIAN_C.
		cv::THRESH_BINARY,                // 閾値の種類． THRESH_BINARY または THRESH_BINARY_INV のどちらか．
		3,//23,                                // ピクセルの閾値を求めるために利用される近傍領域のサイズ．3, 5, 7, など．
		0                                 // 平均または加重平均から引かれる定数.この値により maxValue の範囲が変わる.
	);

	
	cv::Mat element(2, 2, CV_8U, cv::Scalar(1));

	thinning(search_img, search_img);
	// ラベルごとのROIを得る(0番目は背景なので無視)
	cv::Mat roiImg;
	cv::cvtColor(search_img, roiImg, CV_GRAY2BGR);
	std::vector<cv::Rect> roiRects;
	cv::Mat LabelImg;
	cv::Mat stats;
	cv::Mat centroids;
	int nLab = cv::connectedComponentsWithStats(search_img, LabelImg, stats, centroids, 8);
	int num = 0;
	int th = 5;// 20;
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
	cv::imwrite("search_img.bmp", search_img);

	cv::Mat dist = cv::imread("search_img.bmp");
	cv::Mat N_tmp = cv::imread("Ntmp.bmp");
	cv::Mat S_tmp = cv::imread("Stmp.bmp");
	cv::Mat O_tmp = cv::imread("Otmp.bmp");
	
	double result[3];
	Mat imgGray0(dist.rows, dist.cols, CV_64FC1);
	Mat imgGray1(N_tmp.rows, N_tmp.cols, CV_64FC1);
	Mat imgGray2(S_tmp.rows, S_tmp.cols, CV_64FC1);
	Mat imgGray3(O_tmp.rows, O_tmp.cols, CV_64FC1);
	myGrayscaleImage(dist, imgGray0);
	myGrayscaleImage(N_tmp, imgGray1);
	myGrayscaleImage(S_tmp, imgGray2);
	myGrayscaleImage(O_tmp, imgGray3);
	Mat matHann;
	createHanningWindow(matHann, dist.size(), CV_64F);
	phaseCorrelate(imgGray0, imgGray1, matHann, Nres);
	phaseCorrelate(imgGray0, imgGray2, matHann, Sres);
	phaseCorrelate(imgGray0, imgGray3, matHann, Ores);
	//std::remove()
	cv::waitKey(0);
	// キー入力を（無限に）待つ

}


int main(){
	int Flag;
	do {
		double res[3] = { 0 };
		for (int i = 0; i < 3; i++) {
			double Nres = 0, Sres = 0, Ores = 0;
			match(&Nres, &Sres,&Ores);
			res[0] += Nres;
			res[1] += Sres;
			res[2] += Ores;
			std::cout << Nres << ":" << Sres << ":" << Ores << std::endl;
		}
		// search max
		std::vector<double> x;
		x.push_back(res[0]);
		x.push_back(res[1]);
		x.push_back(res[2]);

		std::vector<double>::iterator iter = std::max_element(x.begin(), x.end());
		size_t index = std::distance(x.begin(), iter);
		std::cout << "max element is " << x[index] << std::endl;
		if (x[index] < 0.3) {
			std::cout << "No match" << std::endl;
		}
		else if (index==0) {
			std::cout << "Nishimura" << ":" << res[0] << std::endl;
		}
		else if (index == 1) {
			std::cout << "Sako" << ":" << res[1] << std::endl;
		}
		else if (index == 2) {
			std::cout << "Ohira" << ":" << res[2] << std::endl;
		}
		wprintf_s(L"\n Press 1 to repeat.\n Press 0 to exit\n. ");
		std::cin >> Flag;
	}while (Flag);
	return 0;
}

