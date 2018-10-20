// ConsoleApplication1.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//


#include "pch.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <iostream>
#include <thread>
#include <ctime>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include <vector>


//ffmpeg
extern "C" {
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/pixdesc.h>
	#include <libswscale/swscale.h>
	#include <libavutil/opt.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/common.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/samplefmt.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")

using namespace std;
using namespace cv;

static AVFrame *alloc_frame(enum AVPixelFormat pix_fmt, int width, int height)
{
	AVFrame *frame;
	int ret;

	frame = av_frame_alloc();
	if (!frame)
		return NULL;

	frame->format = pix_fmt;
	frame->width = width;
	frame->height = height;

	// allocate the buffers for the frame data
	//ret = av_frame_get_buffer(frame, 32);

	ret = av_image_alloc(frame->data, frame->linesize, width, height, pix_fmt, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");
		exit(1);
	}

	return frame;
}

/*
class ThreadRAII
{
	std::thread & m_thread;
public:
	ThreadRAII(std::thread & threadObj) : m_thread(threadObj)
	{

	}
	~ThreadRAII()
	{
		// Check if thread is joinable then detach the thread
		if (m_thread.joinable())
		{
			m_thread.detach();
		}
	}
};
*/

//Methode permettant la capture du desktop
void hwnd2mat(HWND hwnd, Mat *src)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width;

	int srcheight, srcwidth;
	HBITMAP hbwindow;
	//Mat src;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	(*src).create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, (*src).data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	//if(!src.empty())std::cout << "Created" << "\n";

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	//return src;
}

//Methode permettant l'encodage d'une AVFrame
static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
	FILE *outfile)
{
	//std::cout << "In ENCODE Program \n";
	int ret = 0;

	/* send the frame to the encoder */
	//if (frame) printf("Send frame %3"PRId64"\n", frame->pts);
	//std::cout << frame->pts << "\n";
	//ret = avcodec_send_frame(enc_ctx, frame);
	/*if (ret < 0) {
		fprintf(stderr, "Error sending a frame for encoding\n");
		exit(1);
	}*/

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during encoding\n");
			//exit(1);
		}

		//printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
		//std::cout << "Frame : " << pkt->pts << " - Size : " << pkt->size << "\n";
		std::fwrite(pkt->data, 1, pkt->size, outfile);
		av_packet_unref(pkt);
	}
	
}

/*void capture(HWND hwndDesktop, AVFrame *frame, SwsContext* swsctx) {
	//HWND hwndDesktop = GetDesktopWindow();
	//namedWindow("output", WINDOW_NORMAL);
	int ret = 0;

	//fflush(stdout);

	// make sure the frame data is writable
	ret = av_frame_make_writable(frame);
	if (ret < 0)
		exit(1);


	//clock_t begin = clock();
	Mat src = hwnd2mat(hwndDesktop);
	//clock_t end = clock();

	if (src.empty()) exit(1);
	//imshow("output", src);

	

	// convert cv::Mat(OpenCV) to AVFrame(FFmpeg)
	const int stride[] = { static_cast<int>(src.step[0]) };
	sws_scale(swsctx, &src.data, stride, 0, src.rows, frame->data, frame->linesize);
	//std::cout << frame->pts << "\n";

	frame->pts = frame->pts + 1;
	std::cout << "In CAPTURE Program \n";

	//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//std::cout << elapsed_secs << "\n";
}*/


//Methode permettant de définir les paramètres du codec
void setCodecContext(AVDictionary *param, AVCodecContext *codecContext, AVRational dst_fps, int dst_width, int dst_height) {
	av_dict_set(&param, "crf", "30", 0);
	av_dict_set(&param, "preset", "ultrafast", 0);
	av_dict_set(&param, "tune", "zerolatency", 0);
	codecContext->time_base = av_inv_q(dst_fps);
	codecContext->rc_buffer_size = 4000;
	codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	codecContext->width = dst_width;
	codecContext->height = dst_height;
	//codecContext->gop_size = 0; // emit one intra frame every ten frames
	//codecContext->max_b_frames = 10;
	codecContext->rc_max_rate = 500;
	/*
		AVCodecContext.gop_size = 250;
	AVCodecContext.pix_fmt = PIX_FMT_YUV420P;
	AVCodecContext.bit_rate = (preferredKbps * 1000);
	AVCodecContext.bit_rate_tolerance = (preferredKbps * 1000);
	AVCodecContext.rc_min_rate = 0;
	AVCodecContext.rc_max_rate = 0;
	AVCodecContext.rc_buffer_size = 0;
	AVCodecContext.flags |= CODEC_FLAG_PSNR;
	AVCodecContext.partitions = X264_PART_I4X4 | X264_PART_I8X8 |
		X264_PART_P8X8 | X264_PART_P4X4 | X264_PART_B8X8;
	AVCodecContext.crf = 0.0f;
	AVCodecContext.cqp = 26;
	AVCodecContext.i_quant_factor = 0.769f;
	AVCodecContext.b_quant_factor = 1.4f;
	AVCodecContext.rc_initial_buffer_occupancy = (int)(0.9 *
		encoderContext->rc_buffer_size);
	AVCodecContext.rc_buffer_size = 0;
	AVCodecContext.time_base.num = 1;
	AVCodecContext.time_base.den = framesPerSecond;
	AVCodecContext.me_method = ME_HEX;
	AVCodecContext.qmin = 10;
	AVCodecContext.qmax = 51;
	AVCodecContext.max_qdiff = 4;
	AVCodecContext.max_b_frames = 4;
	*/

	
}

int main(int argc, char **argv)
{
	int ret;
	FILE *f;
	const int dst_width = 1280;
	const int dst_height = 720;//1280*720
	const int src_width = 1920;//1920*1080
	const int src_height = 1080;
	int fps = 30;
	const AVRational dst_fps = { fps, 1 };
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	const char* outfile, *codec_name;
	AVPacket *pkt;
	const AVCodec *codec;
	AVCodecContext *codecContext = NULL;
	AVDictionary *param = NULL;
	errno_t err;
	AVFrame *frame;
	vector<Mat> srcTab;


	HWND hwndDesktop = GetDesktopWindow();
	outfile = "test.mkv";
	codec_name = "libx264"; //id : AV_CODEC_ID_H264
	
	codec = avcodec_find_encoder_by_name(codec_name);
	if (!codec) {
		fprintf(stderr, "Codec '%s' not found\n", codec_name);
		exit(1);
	}
	codecContext = avcodec_alloc_context3(codec);
	if (!codecContext) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
		
	pkt = av_packet_alloc();
		
	setCodecContext(param,codecContext, dst_fps, dst_width, dst_height);

	ret = avcodec_open2(codecContext, codec, &param);
	if (ret < 0) {
		exit(5);
	}
	
	err = fopen_s(&f, outfile, "wb"); //createoutput file
	
	frame = alloc_frame(codecContext->pix_fmt, codecContext->width, codecContext->height);
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}
	frame->pts = 0;

	//AV_PIX_FMT_BGR24
	SwsContext* swsctx = sws_getCachedContext(
		NULL, src_width, src_height, AV_PIX_FMT_BGRA,
		dst_width, dst_height, codecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
	
	Mat src1;
	Mat src2;
	bool switchSrc = true;
	//system_clock::time_point start = system_clock::now();
	/*// Get End Time
	auto end = system_clock::now();

	auto diff = duration_cast & lt; std::chrono::seconds &gt; (end - start).count();
	std::cout &lt; &lt; "Total Time Taken = " &lt; &lt; diff &lt; &lt; " Seconds" &lt; &lt; std::endl;*/

	
	//std::future< std::string> resultFromDB = 
	//Fetch Data from DB
	// Will block till data is available in future&lt;std::string&gt; object.
	//std::string dbData = resultFromDB.get();


	/* encode loop for video */
	//while (true) {

		fflush(stdout);
		//clock_t begin_capture = clock();
		//thread t1 (capture, hwndDesktop, frame, swsctx);
		//clock_t end_capture = clock();
		//ThreadRAII wrapperObj(t1);

		bool encodage = false;

		

		thread threadObj1([hwndDesktop, &src1, &src2, &switchSrc, &srcTab, &encodage] {
			double time_average_capture = 0;
			int num_capture = 0;
			while (true) {
				clock_t begin_capture = clock();
				//Sleep(7);
				if (srcTab.size() == 0) {
					num_capture++;
					if (switchSrc) {
						hwnd2mat(hwndDesktop, &src1);
						srcTab.push_back(src1);
						switchSrc = false;
						//std::cout << "In CAPTURE 1 Program \n";
						encodage = true;
					}
					else if (!switchSrc) {
						hwnd2mat(hwndDesktop, &src2);
						srcTab.push_back(src2);
						switchSrc = true;
						//std::cout << "In CAPTURE 2 Program \n";
						encodage = true;
					}
					clock_t end_capture = clock();
					double elapsed_secs_capture = double(end_capture - begin_capture) / CLOCKS_PER_SEC;
					//std::cout << "Capture : " << elapsed_secs_capture << "\n";
					time_average_capture = (time_average_capture*(num_capture - 1) + elapsed_secs_capture) / num_capture;
					std::cout << "Capture Average : " << time_average_capture << "\n";
				}
			}
		});
		
		Sleep(30);
		//imshow("output", src);

		//frame->pts
		// convert cv::Mat(OpenCV) to AVFrame(FFmpeg)
		
		//std::cout << frame->pts << "\n";

		thread threadObj2([codecContext, frame, pkt, f, swsctx, &srcTab]{
			double time_average_encode = 0;
			int num_encode = 0;
			while (true) {
				//std::cout << srcTab.size() << "\n";
				if (srcTab.size() > 0) {
					clock_t begin_encode = clock();
					//std::cout << "In ENCODE 1 Program \n";
					const int stride[] = { static_cast<int>(srcTab[0].step[0]) };
					sws_scale(swsctx, &srcTab[0].data, stride, 0, srcTab[0].rows, frame->data, frame->linesize);
					if (avcodec_send_frame(codecContext, frame) >= 0) {
						encode(codecContext, frame, pkt, f);
						frame->pts = frame->pts + 1;
						num_encode++;
					}
					srcTab.erase(srcTab.begin());
					
					//std::cout << "Frame : " << frame->pts << "\n";
					clock_t end_encode = clock();
					double elapsed_secs_encode = double(end_encode - begin_encode) / CLOCKS_PER_SEC;
					//std::cout << " Encode : " << elapsed_secs_encode << "\n";
					time_average_encode = (time_average_encode*(num_encode - 1) + elapsed_secs_encode) / num_encode;
					std::cout << "Encode Average : " << time_average_encode << "\n";
				}

				/*if (!src1.empty() & !block) {
					clock_t begin_encode = clock();
					std::cout << "In ENCODE 1 Program \n";
					const int stride[] = { static_cast<int>(src1.step[0]) };
					sws_scale(swsctx, &src1.data, stride, 0, src1.rows, frame->data, frame->linesize);
					src1.release();
					if (avcodec_send_frame(codecContext, frame) >= 0) {
						encode(codecContext, frame, pkt, f);
						frame->pts = frame->pts + 1;
					}
					block = true;
					std::cout << "Frame : " << frame->pts << "\n";
					clock_t end_encode = clock();
					double elapsed_secs_encode = double(end_encode - begin_encode) / CLOCKS_PER_SEC;
					std::cout << " Encode : " << elapsed_secs_encode << "\n";
				}
				else if (!src2.empty() & block) {
					clock_t begin_encode = clock();
					std::cout << "In ENCODE 2 Program \n";
					const int stride[] = { static_cast<int>(src2.step[0]) };
					sws_scale(swsctx, &src2.data, stride, 0, src2.rows, frame->data, frame->linesize);
					src2.release();
					if (avcodec_send_frame(codecContext, frame) >= 0) {
						encode(codecContext, frame, pkt, f);
						frame->pts = frame->pts + 1;
					}
					block = false;
					std::cout << "Frame : " << frame->pts << "\n";
					clock_t end_encode = clock();
					double elapsed_secs_encode = double(end_encode - begin_encode) / CLOCKS_PER_SEC;
					std::cout << " Encode : " << elapsed_secs_encode << "\n";
				}
				Sleep(1000/90);*/
			}
		});

		threadObj1.join();
		threadObj2.join();

		

		//clock_t begin = clock();
		//Mat src = hwnd2mat(hwndDesktop);
		//clock_t end = clock();

		//hwnd2mat(hwndDesktop, &src);
		
		

		// encode the image
		//clock_t begin_encode = clock();
		
		//clock_t end_encode = clock();
		//std::cout << "In MAIN Program \n";
	
		//clock_t begin_capture = clock();
		//thread t2(capture, hwndDesktop, frame2, swsctx);
		//clock_t end_capture = clock();
		//https://thispointer.com/c11-multithreading-part-2-joining-and-detaching-threads/
		// encode the image
		//clock_t begin_encode = clock();
		//encode(codecContext, frame2, pkt, f);
		//clock_t end_encode = clock();

		//double elapsed_secs_capture = double(end_capture - begin_capture) / CLOCKS_PER_SEC;
		//double elapsed_secs_encode = double(end_encode - begin_encode) / CLOCKS_PER_SEC;

		//std::cout << "Capture : " << elapsed_secs_capture << " Encode : " << elapsed_secs_encode << "\n";
		//t1.detach();
		//t1.join();
	//}

	// flush the encoder
	encode(codecContext, NULL, pkt, f);

	// add sequence end code to have a real MPEG file
	std::fwrite(endcode, 1, sizeof(endcode), f);

	std::fclose(f);

	avcodec_free_context(&codecContext);
	av_frame_free(&frame);
	av_packet_free(&pkt);
	return 0;
}
