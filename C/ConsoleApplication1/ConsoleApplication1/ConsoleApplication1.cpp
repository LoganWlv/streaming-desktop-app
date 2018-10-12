// ConsoleApplication1.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//


#include "pch.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <iostream>


//ffmpeg
extern "C" {
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/pixdesc.h>
	#include <libswscale/swscale.h>
}

using namespace std;
using namespace cv;


Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width;

	int srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
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

	src.create(height, width, CV_8UC4);

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
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	if(!src.empty())std::cout << "Created" << "\n";

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
	FILE *outfile)
{
	int ret;

	/* send the frame to the encoder */
	//if (frame) printf("Send frame %3"PRId64"\n", frame->pts);
	//std::cout << frame->pts << "\n";
	ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending a frame for encoding\n");
		exit(1);
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during encoding\n");
			exit(1);
		}

		//printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
		std::cout << "Frame : " << pkt->pts << " - Size : " << pkt->size << "\n";
		fwrite(pkt->data, 1, pkt->size, outfile);
		av_packet_unref(pkt);
	}
}


static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(pkt, *time_base, st->time_base);
	pkt->stream_index = st->index;

	/* Write the compressed frame to the media file. */
	//log_packet(fmt_ctx, pkt);
	return av_interleaved_write_frame(fmt_ctx, pkt);
}


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

	/* allocate the buffers for the frame data */
	ret = av_frame_get_buffer(frame, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");
		exit(1);
	}

	return frame;
}



void ffmpeg() {
	// initialize FFmpeg library
	//av_register_all();
	//  av_log_set_level(AV_LOG_DEBUG);
	int ret;

	FILE *f;

	

	const int dst_width = 1280;
	const int dst_height = 720;
	const int src_width = 1920;
	const int src_height = 1080;
	int fps = 30;
	const AVRational dst_fps = { fps, 1 };

	//1280*720
	//1920*1080

	uint8_t endcode[] = { 0, 0, 1, 0xb7 };

	const char* outfile, *codec_name;
	AVPacket *pkt;

	outfile = "test.mkv";
	codec_name = "libx264"; //id : AV_CODEC_ID_H264

	const AVCodec *codec;

	AVCodecContext *codecContext = NULL;

	HWND hwndDesktop = GetDesktopWindow();

	//Frame here

	// open output format context
	//AVFormatContext* outctx = nullptr;
	//ret = avformat_alloc_output_context2(&outctx, nullptr, nullptr, outfile);
	/*if (ret < 0) {
		std::cerr << "fail to avformat_alloc_output_context2(" << outfile << "): ret=" << ret;
	}*/

	// create new video stream

	/* find the mpeg1video encoder */
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

	AVDictionary *param = NULL;
	av_dict_set(&param, "crf", "25", 0);
	av_dict_set(&param, "preset", "ultrafast", 0);
	av_dict_set(&param, "tune", "zerolatency", 0);
	//av_dict_set(&param, "maxrate", "2M", 0);
	//av_dict_set(&param, "bufsize", "6M", 0);
	
	

	pkt = av_packet_alloc();

/*
	codecContext->bit_rate = 400000;
	codecContext->width = dst_width;
	codecContext->height = dst_height;
	codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	codecContext->time_base = av_inv_q(dst_fps);
	codecContext->framerate = dst_fps;
	codecContext->gop_size = 10;
	codecContext->max_b_frames = 1;
*/

	codecContext->time_base = av_inv_q(dst_fps);
	codecContext->framerate = dst_fps;
	codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	codecContext->width = dst_width;
	codecContext->height = dst_height;
	codecContext->gop_size = 10; /* emit one intra frame every ten frames */
	codecContext->max_b_frames = 1;

	ret = avcodec_open2(codecContext, codec, &param);
	if (ret < 0) {
		//fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
		exit(1);
	}

	//
	errno_t err;
	err = fopen_s(&f, outfile, "wb"); //createoutput file

	AVFrame **pFrame;

	AVFrame *frame;

	frame = alloc_frame(codecContext->pix_fmt, codecContext->width, codecContext->height);
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}

	//AV_PIX_FMT_BGR24
	SwsContext* swsctx = sws_getCachedContext(
		NULL, src_width, src_height, AV_PIX_FMT_BGRA,
		dst_width, dst_height, codecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);

	/* encode loop for video */
	unsigned nb_frames = 0;
	bool end_of_stream = false;
	int got_pkt = 0;
	
	int i = 0;
	while (!end_of_stream) {
		if (waitKey(33) == 0x1b)
			end_of_stream = true;

		fflush(stdout);

		/* make sure the frame data is writable */
		ret = av_frame_make_writable(frame);
		if (ret < 0)
			exit(1);

		Mat src = hwnd2mat(hwndDesktop);

		//Sleep(1000 / fps);

		// convert cv::Mat(OpenCV) to AVFrame(FFmpeg)
		const int stride[] = { static_cast<int>(src.step[0]) };
		sws_scale(swsctx, &src.data, stride, 0, src.rows, frame->data, frame->linesize);
		//std::cout << i << "\n";
		frame->pts = i;



		/* encode the image */
		encode(codecContext, frame, pkt, f);
		i++;
	}
	/* flush the encoder */
	encode(codecContext, NULL, pkt, f);
	
	/* add sequence end code to have a real MPEG file */
	fwrite(endcode, 1, sizeof(endcode), f);

	fclose(f);

	avcodec_free_context(&codecContext);
	av_frame_free(&frame);
	av_packet_free(&pkt);
}

void test_visu() {
	HWND hwndDesktop = GetDesktopWindow();
	namedWindow("output", WINDOW_NORMAL);
	int key = 0;
	int i = 0;
	while (i != 240)
	{
		Mat src = hwnd2mat(hwndDesktop);
		if (src.empty()) break;
		cout << i << endl;
		i++;
		imshow("output", src);
		key = waitKey(60);
	}
}

int main(int argc, char **argv)
{
	//HWND hwndDesktop = GetDesktopWindow();
	//Mat src = hwnd2mat(hwndDesktop);

	//std::cout << src.depth() << "\n";
	//std::cout << src.channels() << "\n";
	ffmpeg();
	//test_visu();
	return 0;

}
