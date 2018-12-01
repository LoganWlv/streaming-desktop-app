
#include "pch.h"
#include <iostream>
#include <Windows.h>

void emulKey(char recvKey) {
	/* SEND KEY WORKING TEST*/
	// This structure will be used to create the keyboard
	// input event.
	INPUT ip;
	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	HKL currentKBL = GetKeyboardLayout(0);
	// Press the "A" key
	ip.ki.wVk = VkKeyScanEx(recvKey, currentKBL);
	; // virtual-key code for the "a" key
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));
	
	/*
	// Release the "A" key
	ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	SendInput(1, &ip, sizeof(INPUT));*/

}