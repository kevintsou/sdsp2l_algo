#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
#include <windows.h>
#include <stdbool.h>
#include <nvme.h>

#define NVME_ADMIN_COMMAND_SET  0
#define NVME_COMMAND_GET_LOG_PAGE   (NVME_ADMIN_COMMAND_SET|NVME_ADMIN_COMMAND_GET_LOG_PAGE)
#define NVME_COMMAND_IDENTIFY       (NVME_ADMIN_COMMAND_SET|NVME_ADMIN_COMMAND_IDENTIFY)
#define NVME_COMMAND_SET_FEATURES   (NVME_ADMIN_COMMAND_SET|NVME_ADMIN_COMMAND_SET_FEATURES)
#define NVME_COMMAND_GET_FEATURES   (NVME_ADMIN_COMMAND_SET|NVME_ADMIN_COMMAND_GET_FEATURES)
#define NVME_COMMAND_DST            (NVME_ADMIN_COMMAND_SET|NVME_ADMIN_COMMAND_DEVICE_SELF_TEST)
#define NVME_COMMAND_FORMAT_NVM     (NVME_ADMIN_COMMAND_SET|NVME_ADMIN_COMMAND_FORMAT_NVM)
#define NVME_COMMAND_SECURITY_RECV  (NVME_ADMIN_COMMAND_SET|NVME_ADMIN_COMMAND_SECURITY_RECEIVE)

#define NVME_NVM_COMMAND_SET    0x100
#define NVME_COMMAND_READ       (NVME_NVM_COMMAND_SET|NVME_NVM_COMMAND_READ)
#define NVME_COMMAND_WRITE      (NVME_NVM_COMMAND_SET|NVME_NVM_COMMAND_WRITE)
#define NVME_COMMAND_DSM        (NVME_NVM_COMMAND_SET|NVME_NVM_COMMAND_DATASET_MANAGEMENT)
#define NVME_COMMAND_FLUSH      (NVME_NVM_COMMAND_SET|NVME_NVM_COMMAND_FLUSH)

#define NVME_TOOL_COMMAND_QUIT       0xFFFE
#define NVME_TOOL_COMMAND_UNKNOWN    0xFFFF

char* strConvertUTF8toMultiByte(const char* _str);
void printASCII(const char* _strLabel, const char* _strData, bool _bWithNewLine);
int eGetCommandFromConsole(void);
int iGetConsoleInputHex(const char* _strPrompt, char* _strInput);
int iGetConsoleInputDec(const char* _strPrompt, char* _strInput);
char cGetConsoleInput(const char* _strPrompt, char* _strInput);
void PrintDataBuffer(PUCHAR DataBuffer, ULONG BufferLength);

#ifdef __cplusplus
}
#endif