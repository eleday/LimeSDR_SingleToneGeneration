// LimeSDR_TX.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <lime/LimeSuite.h>
#include <thread>
#include <chrono>
#include <math.h>

using namespace std;

lms_device_t* device;
const double sampleRate = 20e6;
bool running;
const int tx_size = 8196 * 256;
float tx_buffer[2 * tx_size];

int error()
{
	cout << "ERROR: " << LMS_GetLastErrorMessage() << endl;
	LMS_Close(device);
	std::cin.get();
	exit(-1);
}

void StreamTest()
{
	lms_stream_t tx_stream;
	tx_stream.channel = 0;
	tx_stream.fifoSize = 256 * 1024;
	tx_stream.throughputVsLatency = 0.5;
	tx_stream.dataFmt = lms_stream_t::LMS_FMT_F32;
	tx_stream.isTx = true;

	lms_stream_meta_t meta_tx;
	meta_tx.waitForTimestamp = false;
	meta_tx.flushPartialPacket = false;
	meta_tx.timestamp = 0;
	float t = 0;
	for (int i = 0; i <tx_size; i++)
	{

	tx_buffer[2 * i] =0.5;
	tx_buffer[2 * i + 1] = 0 ;

	}

	LMS_SetupStream(device, &tx_stream);
	LMS_StartStream(&tx_stream);

	while (running)
	{
		int ret = LMS_SendStream(&tx_stream, tx_buffer, tx_size, &meta_tx, 1000);
		if (ret != tx_size)
			cout << "error: samples sent: " << ret << "/" << tx_size << endl;;
	}

	LMS_StopStream(&tx_stream);
	LMS_DestroyStream(device, &tx_stream);
}

int main(int argc, char** argv)
{
	int n = LMS_GetDeviceList(nullptr);
	//
	std::cout << "Sample code for farhan296" << std::endl;
	if (n > 0)
	{  
		std::cout << "Open device\n";
		if (LMS_Open(&device, NULL, NULL) != 0) //open first device
			error();

		std::cout << "Init device\n";
		if (LMS_Init(device) != 0)
			error();

		std::cout << "Enable Channel\n";
		if (LMS_EnableChannel(device, LMS_CH_TX, 0, true) != 0)
			error();

		std::cout << "SetSampleRate\n";
		if (LMS_SetSampleRate(device, sampleRate, 0) != 0)
			error();

		std::cout << "SetLOFrequency\n";
		if (LMS_SetLOFrequency(device, LMS_CH_TX, 0, 2.47e9) != 0)
			error();
		std::cout << "SetAtenna\n";
		if (LMS_SetAntenna(device, LMS_CH_TX, 0, LMS_PATH_TX1) != 0)   //TX1_1        
			error();

		std::cout << "The RF output will only last for 30 seconds. \n";

		running = true;
		std::thread thread = std::thread(StreamTest);
		this_thread::sleep_for(chrono::seconds(30));
		running = false;
		thread.join();

		std::cout << "Disable Channel\n";
		if (LMS_EnableChannel(device, LMS_CH_TX, 0, false) != 0)
			error();

		if (LMS_Close(device) == 0)
			cout << "Closed" << endl;
	}

	std::cin.get();
	return 0;
}