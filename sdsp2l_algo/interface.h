#pragma once

// Export funciton interface
extern "C" __declspec(dllexport) int lp2lHitCnt();
extern "C" __declspec(dllexport) long lpl2ChkCnt();
extern "C" __declspec(dllexport) int iGetDevCap();
extern "C" __declspec(dllexport) int iGetDdrSize();

extern "C" __declspec(dllexport) int iIssueFlashCmd(int cmd, int ch, int blk, int plane, int page, int* pPayload);
extern "C" __declspec(dllexport) int iIssueFlashCmdEn(int cmd, int pAddr, int* pPayload);
extern "C" __declspec(dllexport) int iInitDeviceConfig(int devCap, int ddrSize, int chCnt, int planeCnt, int pageCnt, int* bufPtr);
extern "C" __declspec(dllexport) int iGetTableSize();

extern "C" __declspec(dllexport) int* iGetEraseCntTable(int ch);
extern "C" __declspec(dllexport) int* iGetReadCntTable(int ch);
extern "C" __declspec(dllexport) long iGetIoBurstCnt(int ch);
extern "C" __declspec(dllexport) int iClearChkHitCnt();