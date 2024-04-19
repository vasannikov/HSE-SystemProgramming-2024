#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__CYGWIN__)
    // ��� Windows ���������� ���������� � ���������� ����
    #include <windows.h>
    #include <winerror.h>

    typedef HANDLE MyDescriptor;

    #define INVALID_MYDESCRIPTOR_VALUE INVALID_HANDLE_VALUE
    #define INVALID_MYFILE_SIZE INVALID_FILE_SIZE
    #define INVALID_MYSET_FILE_POINTER INVALID_SET_FILE_POINTER
#elif defined(__linux__)
    // ��� Linux ���������� ���������� � ���������� ����
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <limits.h>

    typedef int MyDescriptor;

    #define INVALID_MYDESCRIPTOR_VALUE -1
    #define INVALID_MYFILE_SIZE SSIZE_MAX
    #define INVALID_MYSET_FILE_POINTER SSIZE_MAX
#endif

//---------------------------------------------------------------------------

// ���� ������

#define LAB_OK 0
#define LAB_ERR -1
#define LAB_BAD_BUF -2
#define LAB_NOT_FOUND -3
#define LAB_STOP -4
#define LAB_SERVER_DISCONNECTED_OK -5
#define LAB_BAD_FILE -6

// ����� ��� ������ ������� � ������ ������

#define LAB_FLAG_WSA 0
#define LAB_FLAG_GET 1
#define LAB_FLAG_ERRNO 2
#define LAB_FLAG_MY 3
#define LAB_FLAG_RESULT 4

// ���������

#define LAB_MAX_BUF_SIZE 128
#define LAB_MIN_BUF_LEN 1
#define LAB_HOST_NAME_SIZE 64

//---------------------------------------------------------------------------

// ��������� �������:
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__CYGWIN__)
// ����� ��������� �� ������ Windows
void ErrorOutputWindows(
        const char *chFuncFailMessage, // ��������� ���������
        int iErr, // ����� ������, ������������ ��������, ��� ��������� ������
        int iFlag // ����, ��� �������� ��������� �� ������
        );
#elif defined(__linux__)
// ����� ��������� �� ������ Linux
void ErrorOutputLinux(
        const char *chFuncFailMessage, // ��������� ���������
        int iErr, // ����� ������, ������������ ��������, ��� ��������� ������
        int iFlag // ����, ��� �������� ��������� �� ������
        );
#endif

// ����� ��������� �� ������
void ErrorOutput(
        const char *chFuncFailMessage, // ��������� ���������
        int iErr, // ����� ������, ������������ ��������, ��� ��������� ������
        int iFlag // ����, ��� �������� ��������� �� ������
        );
        
//---------------------------------------------------------------------------

int ReleaseHandle(
        MyDescriptor *h // ��������� �� ���������� 
        );

//---------------------------------------------------------------------------

void ReleaseVoidMemory(
        void **vMemory // ��������� �� ��������� �� ������ ������ ����
        );

//---------------------------------------------------------------------------

int LabOpenFile(
        const char *chFileName, // ��� �����
        MyDescriptor *hFile, // ��������� �� ����������
        unsigned long *dwFileSize // ��������� �� ���������� � �������� �����   
        );

//---------------------------------------------------------------------------

int LabReadFile(
        MyDescriptor hFile, // ����������
        char **chFileContent, // ��������� �� ��������� �� ������� �����
        unsigned long *dwFileSize // ��������� �� ���������� � �������� �����
        );

//---------------------------------------------------------------------------

int LabWriteFile(
        MyDescriptor hFile, // ����������
        const char *chFileContent, // ��������� �� ������� �����
        unsigned long *dwFileSize // ��������� �� ���������� � �������� �����
        );

//---------------------------------------------------------------------------

int LabCloseFile(
        MyDescriptor *hFile // ��������� �� ����������
        );

//---------------------------------------------------------------------------

#endif // FILE_H