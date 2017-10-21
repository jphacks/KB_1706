#include "opencv2\opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;


int g_MeanBlockSize = 0;
int g_MeanOffset = 0;
int g_GaussianBlockSize = 0;
int g_GaussianOffset = 0;

void thinningIte(Mat& img, int pattern){
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

int main(int argc, char *argv[])
{
	cv::Mat src_img = cv::imread("C:/Users/Yusuke/source/repos/biometrix/biometrix/fingerprint.bmp", 1);
	if (src_img.empty()) return -1;

	/// 画像を表示するウィンドウ
	// ウィンドウの名前，プロパティ
	// CV_WINDOW_AUTOSIZE : ウィンドウサイズを画像サイズに合わせる
	// CV_WINDOW_FREERATIO : ウィンドウのアスペクト比を固定しない
	cv::namedWindow("image1", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::cvtColor(src_img, src_img, CV_BGR2GRAY);
	// ウィンドウ名でウィンドウを指定して，そこに画像を描画
	cv::imshow("image1", src_img);
	medianBlur(src_img, src_img, 3);          // メディアンフィルタ
	// デフォルトのプロパティで表示
	cv::adaptiveThreshold(src_img,                              // 8ビット，シングルチャンネルの入力画像．
		src_img,
		255,                              // 2値化画像を作成する際の０以外の色。ここでは[ 0, 255 ]で作成する.
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,   // 近傍の平均値を計算する際に使用するアルゴリズム.ADAPTIVE_THRESH_MEAN_C または ADAPTIVE_THRESH_GAUSSIAN_C.
		cv::THRESH_BINARY,                // 閾値の種類． THRESH_BINARY または THRESH_BINARY_INV のどちらか．
		41,                                // ピクセルの閾値を求めるために利用される近傍領域のサイズ．3, 5, 7, など．
		5                                 // 平均または加重平均から引かれる定数.この値により maxValue の範囲が変わる.
	);
	cv::imshow("image2", src_img);
	/*
	// 画像の適応的閾値処理（MEAN）
	cv::namedWindow("threshold_mean");
	cv::createTrackbar("blockSize", "threshold_mean", &g_MeanBlockSize, 255, onTrackbarMean, &src_img);
	cv::createTrackbar("offset", "threshold_mean", &g_MeanBlockSize, 255, onTrackbarMean, &src_img);
	cv::setTrackbarPos("blockSize", "threshold_mean", 41);
	cv::setTrackbarPos("offset", "threshold_mean", 5);

	// 画像の適応的閾値処理（GAUSSIAN）
	cv::namedWindow("threshold_gaussian");
	cv::createTrackbar("blockSize", "threshold_gaussian", &g_GaussianBlockSize, 255, onTrackbarGaussian, &src_img);
	cv::createTrackbar("offset", "threshold_gaussian", &g_GaussianOffset, 255, onTrackbarGaussian, &src_img);
	cv::setTrackbarPos("blockSize", "threshold_gaussian", 41);
	cv::setTrackbarPos("offset", "threshold_gaussian", 5);
	*/
	thinning(src_img, src_img);
	cv::imshow("image3", src_img);
	/*
	// (1)探索画像全体に対して，テンプレートのマッチング値（指定した手法に依存）を計算
	int dst_size = cvSize(src_img->width - tmp_img->width + 1, src_img->height - tmp_img->height + 1);
	dst_img = cvCreateImage(dst_size, IPL_DEPTH_32F, 1);
	cvMatchTemplate(src_img, tmp_img, dst_img, CV_TM_CCOEFF_NORMED);
	cvMinMaxLoc(dst_img, &min_val, &max_val, &min_loc, &max_loc, NULL);

	// (2)テンプレートに対応する位置に矩形を描画
	cvRectangle(src_img, max_loc,
		cvPoint(max_loc.x + tmp_img->width, max_loc.y + tmp_img->height), CV_RGB(255, 0, 0), 3);
	cvNamedWindow("Image", 1);
	cvShowImage("Image", src_img);
	*/
	// キー入力を（無限に）待つ
	cv::waitKey(0);
	cvDestroyWindow("Image1");
	cvDestroyWindow("Image2");
	cvDestroyWindow("Image3");
	//cvReleaseImage(&src_img);
	return 0;
}