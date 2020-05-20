#include "pch.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <locale>
#include <codecvt>

using namespace std;

void print_dir(const WCHAR *root);
void perfrom_command(wstring command, vector<wstring> parameters, WCHAR *path);
void change_directory(vector<wstring> parameters, WCHAR *path);
void copy_file(vector<wstring> parameters, wstring path);
void create_directory(vector<wstring> parameters, wstring path);
void remove_directory(vector<wstring> parameters, wstring path);
void display_file_info(vector<wstring> parameters, wstring path);
wstring replace_all(const wstring &message, const wstring &pattern, const wstring &replace);
int is_directory(wstring path);
vector<wstring> split(wstring str_to_split, WCHAR separator);
bool is_file_exists(const wstring name);
LPWSTR wstring_to_lpwstr(const wstring& s);
string wstring_to_string(const wstring& s);
string filetime_to_string(FILETIME ft);
string bytes_count_to_stirng(DWORD bytes_count);

#define MAX_PATH_SYMBOL_COUNT 260
#define MAX_COMMANMD_SYMBOL_COUNT 10

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	WCHAR path[MAX_PATH] = L"D:";

	cout << "\n* File manager *\n\n";

	WCHAR input[MAX_PATH_SYMBOL_COUNT + MAX_COMMANMD_SYMBOL_COUNT + 2];
	wstring command;
	vector<wstring> parameters;
	//WCHAR command[MAX_COMMANMD_SYMBOL_COUNT + 1];
	//WCHAR parametr[MAX_PATH_SYMBOL_COUNT + 1];
	do {
		wcin.getline(input, sizeof input);
		wstring input_wstr = input;

		int end_command_index = input_wstr.find(L" ");
		command = input_wstr.substr(0, end_command_index);
		parameters = split(input_wstr.substr(end_command_index + 1), L' ');



		perfrom_command(command, parameters, path);
	} while (command.compare(L"exit") != 0);

}

void perfrom_command(wstring command, vector<wstring> parameters, WCHAR *path) {
	wprintf(L"\n");

	if (command.compare(L"ls") == 0) {
		print_dir(path);
	}
	else if (command.compare(L"pwd") == 0) {
		wprintf(L"%s\n", path);
	}
	else if (command.compare(L"cd") == 0) {
		change_directory(parameters, path);
	}
	else if (command.compare(L"cp") == 0) {
		copy_file(parameters, wstring(path));
	}
	else if (command.compare(L"mkdir") == 0) {
		create_directory(parameters, wstring(path));
	}
	else if (command.compare(L"rmdir") == 0) {
		remove_directory(parameters, wstring(path));
	}
	else if (command.compare(L"finfo") == 0) {
		display_file_info(parameters, wstring(path));
	}

	wprintf(L"\n");
}

void print_dir(const WCHAR *path) {
	WCHAR temp[MAX_PATH] = { '\0' };
	wsprintf(temp, L"%s\\*", path);

	WIN32_FIND_DATA fd;
	HANDLE handle = FindFirstFile(temp, &fd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wcscmp(L".", fd.cFileName) != 0 && wcscmp(L"..", fd.cFileName) != 0)
			{
				if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					wprintf(L"dir \t%s\\%s\n", path, fd.cFileName);
				else 
					wprintf(L"file \t%s\\%s\n", path, fd.cFileName);
			}
		} while (FindNextFile(handle, &fd));
	}
	FindClose(handle);
}

void change_directory(vector<wstring> parameters, WCHAR *path) {
	if (parameters.size() != 1) {
		wprintf(L"Incorrect parameters. Structure: cd <path>");
		return;
	}

	wstring wpath = wstring(path);
	wstring parameter = parameters[0];
	parameter = replace_all(parameter, L"/", L"\\");

	if (parameter.compare(L"~") == 0) {
		wcscpy(path, L"");
		return;
	} else if (parameter.find(L"..") != -1) {
		int index_of_parent_dir_end = wpath.find_last_of(L"\\");
		if (index_of_parent_dir_end != -1) {
			wpath = wpath.substr(0, index_of_parent_dir_end);
		}
		//remove ".." from parameter
		parameter = parameter.substr(2);
	}

	if (wpath.empty()) wpath = L"C:";
	wstring new_path = wpath + parameter;

	switch (is_directory(new_path)) {
		case 0: wprintf(L"cannot access %s\n", new_path.c_str());
			break;
		case 1: 
			wcscpy(path, new_path.c_str());
			break;
		case 2:
			wprintf(L"%s is not a directory\n", new_path.c_str());
			break;
	}		
}


void copy_file(vector<wstring> parameters, wstring path) {
	if (parameters.size() != 2) {
		wprintf(L"Incorrect parameters. Structure: cp <file_name> <copied_file_name>\ncp <file_name> <copied_file_full_path>\ncp <file_name> <copied_folder>\n");
		return;
	}

	//Process source path
	wstring source_file_path = path + L"\\" + parameters[0];
	bool is_source_file_exists;
	switch (is_directory(source_file_path))
	{
		case 0: 
			wprintf(L"cannot access %s\n", source_file_path.c_str());
			is_source_file_exists = false;
			break;
		case 1:
			wprintf(L"%s is a directory\n", source_file_path.c_str());
			is_source_file_exists = false;
			break;
		case 2:
			is_source_file_exists = is_file_exists(source_file_path);
			break;
	}

	if (!is_source_file_exists) {
		wprintf(L"\nIncorrect source path. Try again.\n");
		return;
	}

	//Process destination path
	wstring destination = parameters[1];
	destination = replace_all(destination, L"/", L"\\");

	wstring destination_folder = L"";

	if (destination.find(L".") == -1) {
		destination_folder = destination;
	}
	else if (destination.find(L"\\") != -1) {
		destination_folder = destination.substr(0, destination.find_last_of(L"\\"));
	}

	is_source_file_exists = true;
	if (!destination_folder.empty()) {
		wstring destination_folder_path = path + L"\\" + destination_folder;
		switch (is_directory(destination_folder_path))
		{
		case 0:
			wprintf(L"cannot access %s\n", destination_folder_path.c_str());
			is_source_file_exists = false;
			break;
		case 2:
			wprintf(L"%s is a directory\n", destination_folder_path.c_str());
			is_source_file_exists = false;
			break;
		}
	}

	if (!is_source_file_exists) {
		wprintf(L"\nIncorrect destination path. Try again.\n");
		return;
	}

	// if second parameter has new file name
	if (destination.find(L".") != -1) {
		wstring destination_path = path + L"\\" + destination;
		CopyFile(source_file_path.c_str(), destination_path.c_str(), FALSE);
	}
	else {
		wstring destination_path = wstring(path + L"\\" + destination.c_str() + L"\\" + parameters[0]);
		CopyFile(source_file_path.c_str(), destination_path.c_str(), FALSE);
	}
}

void create_directory(vector<wstring> parameters, wstring path) {
	if (parameters.size() != 1) {
		wprintf(L"Incorrect parameters. Structure: mkdir <directory_path>\n");
		return;
	}

	wstring directory_name = parameters[0];
	directory_name = replace_all(directory_name, L"/", L"\\");
	LPWSTR directory_full_name = wstring_to_lpwstr(path + L"\\" + directory_name);

	if (!CreateDirectory(directory_full_name, NULL)) {
		if (is_directory(directory_full_name) == 1)
			cout << "Directory is already exists\n";
		else
			cout << "Creating directory error\n";
	}
}

void remove_directory(vector<wstring> parameters, wstring path) {
	if (parameters.size() != 1) {
		wprintf(L"Incorrect parameters. Structure: rmdir <directory_path>\n");
		return;
	}

	wstring directory_name = parameters[0];
	directory_name = replace_all(directory_name, L"/", L"\\");
	string directory_full_name = wstring_to_string(path + L"\\" + directory_name);//.c_str();

	if (!RemoveDirectoryA(directory_full_name.c_str())) {
		cout << "Removing directory error\n";
	}
}

void display_file_info(vector<wstring> parameters, wstring path) {
	if (parameters.size() != 1) {
		wprintf(L"Incorrect parameters. Structure: finfo <file_name>\nfinfo <file_path>\n");
		return;
	}

	wstring file_name = parameters[0];
	file_name = replace_all(file_name, L"/", L"\\");
	wstring full_path;

	full_path = path + L"\\" + file_name;

	if (is_directory(full_path) == 0) {
		wprintf(L"Can't access file\n");
	} else {
		WIN32_FIND_DATA fd;
		HANDLE handle = FindFirstFile(full_path.c_str(), &fd);
		if (handle != INVALID_HANDLE_VALUE)
		{
			wprintf(L"full path - %s\n", full_path.c_str());
			printf("creation time - %s\n", filetime_to_string(fd.ftCreationTime).c_str());
			printf("last access time - %s\n", filetime_to_string(fd.ftLastAccessTime).c_str());
			printf("last write time - %s\n", filetime_to_string(fd.ftLastWriteTime).c_str());
			wprintf(L"full name - %s\n", fd.cFileName);
			printf("file Size - %s\n", bytes_count_to_stirng((fd.nFileSizeHigh * (MAXDWORD + 1)) + fd.nFileSizeLow).c_str());
		}
		else {
			wprintf(L"Can't access file\n");
		}
		FindClose(handle);
	}
}

wstring replace_all(const wstring &message, const wstring &pattern, const wstring &replace)
{
	wstring result = message;
	int pos = 0;
	int offset = 0;

	while ((pos = result.find(pattern, offset)) != wstring::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

// return value: 
// 0 - Can't access
// 1 - is a directory
// 2 - is not a directory
int is_directory(wstring path) {
	struct _stat64i32 info;
	if (_wstat(path.c_str(), &info) != 0)
		return 0;
	else if (info.st_mode & S_IFDIR)
		return 1;
	else
		return 2;
}

vector<wstring> split(wstring str_to_split, WCHAR separator) {
	std::wstring temp;
	std::vector<wstring> parts;
	std::wstringstream wss(str_to_split);
	while (getline(wss, temp, separator))
		parts.push_back(temp);

	return parts;
}

bool is_file_exists(const wstring name) {
	struct _stat64i32 buffer;
	return (_wstat(name.c_str(), &buffer) == 0);
}

LPWSTR wstring_to_lpwstr(const wstring& s)
{
	LPWSTR ws = new wchar_t[s.size() + 1];
	copy(s.begin(), s.end(), ws);
	ws[s.size()] = 0;
	return ws;
}

string wstring_to_string(const wstring& s)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(s).c_str();
}

string filetime_to_string(FILETIME ft) {
	SYSTEMTIME st, st_local;
	DWORD dwRet;

	// Convert the last-write time to local time.
	FileTimeToSystemTime(&ft, &st);
	SystemTimeToTzSpecificLocalTime(NULL, &st, &st_local);

	char buf[256];
	snprintf(buf, 256, "%lu.%lu.%lu %lu:%lu", st_local.wDay, st_local.wMonth, st_local.wYear, st_local.wHour, st_local.wMinute);
	return string(buf);
}

string bytes_count_to_stirng(DWORD bytes_count) {
	char buf[32];
	if (bytes_count < 1024) {
		snprintf(buf, 32, "%lu B", bytes_count);
	}
	else if (bytes_count < 1024 * 1024) {
		snprintf(buf, 32, "%lu KB",bytes_count/1024);
	}
	else if (bytes_count < 1024 * 1024 * 1024) {
		snprintf(buf, 32, "%lu MB", bytes_count / 1024 / 1024);
	}
	else if (bytes_count < 1024 * 1024 * 1024 * 1024) {
		snprintf(buf, 32, "%lu GB", bytes_count / 1024 / 1024 / 1024);
	}
	return string(buf);
}