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

#define DST_STREAM_FRAME_RATE 30
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P /* default pix_fmt */
#define DST_WIDTH 1280
#define DST_HEIGHT 720
#define SRC_WIDTH 1920
#define SRC_HEIGHT 1080

//Methode permettant de définir les paramètres du codec
void setCodecContext(AVCodecContext *codecContext, AVRational time_base) {
	/*av_dict_set(&param, "crf", "30", 0);
	av_dict_set(&param, "preset", "ultrafast", 0);
	av_dict_set(&param, "tune", "zerolatency", 0); //AVDictionary *param, */

	av_opt_set(codecContext->priv_data, "crf", "30", 0);
	av_opt_set(codecContext->priv_data, "preset", "ultrafast", 0);
	av_opt_set(codecContext->priv_data, "tune", "zerolatency", 0);
	codecContext->time_base = time_base;
	codecContext->rc_buffer_size = 4000;
	codecContext->pix_fmt = STREAM_PIX_FMT;
	codecContext->width = DST_WIDTH;
	codecContext->height = DST_HEIGHT;
	codecContext->rc_max_rate = 500;
}

static AVFrame *alloc_frame()//enum AVPixelFormat pix_fmt, int width, int height
{
	int ret;
	AVFrame *frame;

	frame = av_frame_alloc();
	if (!frame)
		return NULL;

	frame->format = STREAM_PIX_FMT;
	frame->width = DST_WIDTH;
	frame->height = DST_HEIGHT;

	// allocate the buffers for the frame data
	//ret = av_frame_get_buffer(frame, 32);

	ret = av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, STREAM_PIX_FMT, 32);
	if (ret < 0) {
		std::cout << "Could not allocate frame data.\n";
		exit(1);
	}

	return frame;
}

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

FILE* writeSDPFile(AVFormatContext *oc) {
	/* Write a file for VLC */
	errno_t err;
	char buf[200000];
	AVFormatContext *ac[] = { oc };
	av_sdp_create(ac, 1, buf, 20000);
	std::printf("sdp:\n%s\n", buf);
	FILE* fsdp;
	err = fopen_s(&fsdp, "test.sdp", "w"); //createoutput file
	std::fprintf(fsdp, "%s", buf);
	std::fclose(fsdp);

	return fsdp;
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVFormatContext *formatContext, AVPacket *pkt)
{
	int ret = 0;
	
	//av_init_packet(pkt);
	/*ret = av_frame_make_writable(frame);
	if (ret < 0) {
		exit(1);
	}*/
	ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0) {
		exit(2);
	}
	

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			exit(3);
		}
		std::cout << "Pkt : " << pkt->pts << " - Size : " << pkt->size << "\n";
		//av_packet_rescale_ts(pkt, enc_ctx->time_base, formatContext->streams[0]->time_base);
		//std::fwrite(pkt->data, 1, pkt->size, outfile);
		av_interleaved_write_frame(formatContext, pkt);
		av_packet_unref(pkt);
		frame->pts = frame->pts + 1;
	}
	std::cout << "Frame : " << frame->pts << " - Size : " << frame->pkt_size << "\n";
	//av_packet_free(&pkt);
}

int main(int argc, char **argv)
{
	vector<Mat> srcTab;
	Mat src1;

	HWND hwndDesktop = GetDesktopWindow();
	SwsContext* swsctx = sws_getCachedContext(NULL, SRC_WIDTH, SRC_HEIGHT, AV_PIX_FMT_BGRA, DST_WIDTH, DST_HEIGHT, STREAM_PIX_FMT, SWS_BICUBIC, NULL, NULL, NULL);


	//avformat_network_init();

	AVOutputFormat *outFormat;
	AVFormatContext *format_ctx = avformat_alloc_context();;
	AVStream *video_st;
	const AVCodec *video_c;
	AVCodecContext *video_c_ctx;
	const char* codec_name;
	const AVRational time_base = { 1, DST_STREAM_FRAME_RATE };
	AVPacket *pkt;
	AVFrame *frame;
	const char * ip_destination;

	ip_destination = "rtp://localhost:5540";
	
	//outFormat
	outFormat = av_guess_format("rtp", ip_destination, NULL);
	//

	//format ctx
	avformat_alloc_output_context2(&format_ctx, outFormat, "rtp", ip_destination);
	//

	//codec
	codec_name = "libx264"; //id : AV_CODEC_ID_H264
	video_c = avcodec_find_encoder_by_name(codec_name);
	//

	//stream ctx
	video_st = avformat_new_stream(format_ctx, video_c); //avcodec_close() and avformat_free_context() 
	//

	//Codec Ctx
	video_c_ctx = avcodec_alloc_context3(video_c);

	setCodecContext(video_c_ctx, time_base);

	avcodec_open2(video_c_ctx, video_c, NULL);
	//

	avcodec_parameters_from_context(video_st->codecpar, video_c_ctx);

	avio_open(&format_ctx->pb, ip_destination, AVIO_FLAG_WRITE);

	//AVDictionary **options;
	avformat_write_header(format_ctx, NULL);

	//std::cout << video_c_ctx->time_base.den << "\n"; //format_ctx->oformat->name

	av_dump_format(format_ctx, video_st->index, format_ctx->url, 1);

	//avcodec_parameters_to_context(video_c_ctx,video_st->codecpar);
	

	 frame = alloc_frame();
	 av_frame_make_writable(frame);
	 frame->pts = 0;

	 pkt = av_packet_alloc();

	 av_init_packet(pkt);

	 writeSDPFile(format_ctx);

	while (true) {
		fflush(stdout);
		hwnd2mat(hwndDesktop, &src1);
		srcTab.push_back(src1);
		src1.release();
		const int stride[] = { static_cast<int>(srcTab[0].step[0]) };
		sws_scale(swsctx, &srcTab[0].data, stride, 0, srcTab[0].rows, frame->data, frame->linesize);
		std::cout << frame->pts << "\n";
		encode(video_c_ctx, frame, format_ctx, pkt);
		srcTab.erase(srcTab.begin());
	}

	//TODO FREE ALL MEMORY & ALLOC


	return 0;
}
