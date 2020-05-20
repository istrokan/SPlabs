#include "pch.h"

#include <string>
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <string.h>
#include <limits>
#include <vector>

#define BUFFER_SIZE 2048
#define NOTE_TEXT_CHARS_NUMBER 80
#define NOTE_TEXT_SIZE NOTE_TEXT_CHARS_NUMBER+1
#define FILE_NAME L"notes.txt"

using namespace std;

struct Note {
	int number;
	FILETIME creation_time;
	WCHAR text[NOTE_TEXT_SIZE];
	int update_count;
};

struct NoteHeader {
	int not_empty_notes_number;
	int file_size;
};

FILETIME get_now_file_time();

void primary_init(HANDLE, int);

void print_menu(NoteHeader note_header);

void perform_action(int, NoteHeader*, HANDLE, int);

void show_all_notes(HANDLE file_handle, int notes_number);

void create_note(HANDLE, int, int);

int get_new_note_position(HANDLE, int);

void get_note_text_from_user(const WCHAR* message, WCHAR* text);

void modify_note(HANDLE, int, int);

void delete_note(HANDLE, int, int);

int get_note_number_from_user(const WCHAR* message, int notes_size);

void update_note_header(HANDLE, bool);

void save_note_to_file(HANDLE, Note, int);

void get_note_from_file(HANDLE, Note*, int);

const LPCWSTR file_name = L"notes.txt";

int main(int argc, char * argv[])
{
	if (argc != 2) {
		printf("Incorrect arguements number.\n");
		return 1;
	}

	const int notes_number = atoi(argv[1]);

	//Open file
	HANDLE file_handle = CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file_handle == INVALID_HANDLE_VALUE) {
		printf("ERROR %x \n", GetLastError());
		CloseHandle(file_handle);
		exit(2);
	}

	LARGE_INTEGER file_size = { 0 };
	GetFileSizeEx(file_handle, &file_size);
	printf("ERROR %x \n", GetLastError());

	if (file_size.QuadPart == 0) {
		primary_init(file_handle, notes_number);
	}

	//Get user input
	int menu;
	do {
		//Get note header from file
		DWORD readBytesCount;
		NoteHeader note_header;
		SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
		ReadFile(file_handle, (LPVOID)&note_header, sizeof(NoteHeader), &readBytesCount, NULL);

		print_menu(note_header);
		wcin >> menu;

		if (menu < 0 && menu > 3) {
			wcout << "\nIncorrect item\n";
		}
		else {
			perform_action(menu, &note_header, file_handle, notes_number);
		}
	} while (menu != 4);

	CloseHandle(file_handle);

	return 0;
}

FILETIME get_now_file_time() {
	FILETIME file_time;
	SYSTEMTIME system_time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);

	return file_time;
}

void primary_init(HANDLE file_handle, int notes_number) {
	NoteHeader note_header;
	note_header.file_size = sizeof NoteHeader;
	note_header.not_empty_notes_number = 0;

	Note primary_note;
	primary_note.update_count = -1;

	DWORD writtenBytesCount;

	//SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
	WriteFile(file_handle, (LPVOID)&note_header, sizeof(NoteHeader), &writtenBytesCount, NULL);

	for (int i = 0; i < notes_number; i++)
	{
		//SetFilePointer(file_handle, sizeof Note, NULL, FILE_CURRENT);
		WriteFile(file_handle, (LPVOID)&primary_note, sizeof(Note), &writtenBytesCount, NULL);
	}
}

void print_menu(NoteHeader note_header) {
	wcout << "\n\n* NOTES *\n\n"
		<< "Files: " << note_header.not_empty_notes_number 
		<< "\nFile size: " <<  note_header.file_size << "\n\n"
		<< "[0] - show all notes\n"
		<< "[1] - create note\n"
		<< "[2] - modify note by number\n"
		<< "[3] - delete note by number\n"
		<< "[4] - exit\n\n";
}

void perform_action(int menu_item, NoteHeader* note_header, HANDLE file_handle, int notes_number) {
	int note_number;
	int note_position;
	switch (menu_item) {
	case 0: show_all_notes(file_handle, notes_number);
		break;
	case 1:
		note_position = get_new_note_position(file_handle, notes_number);
		create_note(file_handle, notes_number, note_position);
		break;
	case 2:
		note_number = get_note_number_from_user(L"\nEnter number of note to modify\n>", notes_number);
		modify_note(file_handle, notes_number, note_number);
		break;
	case 3: 
		note_number = get_note_number_from_user(L"\nEnter number of note to delete\n>", notes_number);
		delete_note(file_handle, notes_number, note_number);
		break;
	case 4: exit(0);
		break;
	}
}

void show_all_notes(HANDLE file_handle, int notes_number) {
	SetFilePointer(file_handle, sizeof NoteHeader, NULL, FILE_BEGIN);
	DWORD readenBytesCount;
	bool is_note_list_empty = true;

	for (int i = 0; i < notes_number; i++) {
		Note note;
		ReadFile(file_handle, (LPVOID)&note, sizeof(Note), &readenBytesCount, NULL);
		//Show only not deleted notes
		if (note.update_count != -1) {
			is_note_list_empty = false;
			SYSTEMTIME st;
			FileTimeToSystemTime(&note.creation_time, &st);

			wcout << "\n [" << note.number << "] - |" << st.wDay << "." << st.wMonth << "." << st.wYear
				<< "|updates number - " << note.update_count << "|\n\t";
			wprintf(note.text);
		}
	}

	if (is_note_list_empty) {
		wcout << "\nThere is no notes in the file.\n";
	}
}

int get_new_note_position(HANDLE file_handle, int notes_number) {
	int note_position = -1;

	SetFilePointer(file_handle, sizeof NoteHeader, NULL, FILE_BEGIN);
	DWORD readenBytesCount;

	for (int i = 0; i < notes_number; i++) {
		Note note;
		ReadFile(file_handle, (LPVOID)&note, sizeof(Note), &readenBytesCount, NULL);

		//if deleted (empty) note
		if (note.update_count == -1) {
			note_position = i;
			break;
		}
	}

	return note_position;
}

void create_note(HANDLE file_handle, int notes_number, int note_position) {
	if (note_position == -1) {
		wcout << "\nYou have already" << notes_number << " notes. Please, delete some to create new one.\n";
		return;
	}

	WCHAR text[NOTE_TEXT_SIZE];
	get_note_text_from_user(L"\nEnter the text for a new note\n", text);

	Note note;
	wcscpy(note.text, text);
	note.update_count = 0;
	note.creation_time = get_now_file_time();
	note.number = note_position;

	update_note_header(file_handle, false);
	save_note_to_file(file_handle, note, note_position);
}
//"\nEnter new text for note <" << note->number << ">\n"
void get_note_text_from_user(const WCHAR* message, WCHAR* text) {
	do {
		wcout << message;
		wcin.ignore(INT_MAX, '\n');
		wcin.getline(text, NOTE_TEXT_SIZE);
	} while (!wcscmp(text, L""));
}

void modify_note(HANDLE file_handle, int notes_number, int note_position) {
	Note note;
	get_note_from_file(file_handle, &note, note_position);
	//if note deleted (empty)
	if (note.update_count == -1) {
		wcout << "\nThere is no existing note with such number\n\n";
		return;
	}

	WCHAR new_text[NOTE_TEXT_SIZE];
	get_note_text_from_user(L"\nEnter new text for the note\n", new_text);

	wcscpy(note.text, new_text);
	note.update_count++;

	save_note_to_file(file_handle, note, note_position);
	wcout << "\nNote has been updated!\n\n";
}

void delete_note(HANDLE file_handle, int notes_number, int note_position) {
	Note note;
	get_note_from_file(file_handle, &note, note_position);

	if (note.update_count == -1) {
		wcout << "\nThere is no existing note with such number\n\n";
		return;
	}

	wcscpy(note.text, L"");
	note.update_count = -1;

	save_note_to_file(file_handle, note, note_position);
	update_note_header(file_handle, true);
	wcout << "\nNote has been deleted!\n\n";
}

void update_note_header(HANDLE file_handle, bool is_note_deleted) {
	DWORD readenBytesCount, writtenBytesCount;
	NoteHeader note_header;
	SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
	ReadFile(file_handle, (LPVOID)&note_header, sizeof(NoteHeader), &readenBytesCount, NULL);

	if (is_note_deleted) {
		note_header.file_size -= sizeof Note;
		note_header.not_empty_notes_number--;
	}
	else {
		note_header.file_size += sizeof Note;
		note_header.not_empty_notes_number++;
	}
	

	SetFilePointer(file_handle, 0, NULL, FILE_BEGIN);
	WriteFile(file_handle, (LPVOID)&note_header, sizeof(NoteHeader), &writtenBytesCount, NULL);
}

void save_note_to_file(HANDLE file_handle, Note note, int note_position) {
	DWORD writtenBytesCount;
	SetFilePointer(file_handle, sizeof NoteHeader + (sizeof Note * note_position), NULL, FILE_BEGIN);
	WriteFile(file_handle, (LPVOID)&note, sizeof(Note), &writtenBytesCount, NULL);
}

void get_note_from_file(HANDLE file_handle, Note* note, int note_position) {
	SetFilePointer(file_handle, sizeof NoteHeader + (sizeof Note * note_position), NULL, FILE_BEGIN);
	DWORD readenBytesCount;
	ReadFile(file_handle, (LPVOID)note, sizeof(Note), &readenBytesCount, NULL);
}

int get_note_number_from_user(const WCHAR* message, int notes_size) {
	int number;
	do {
		wcout << message;
		wcin >> number;
	} while (number < 0 || number > notes_size);

	return number;
}

