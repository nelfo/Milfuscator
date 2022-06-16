#pragma once

class File
{
public:
	File(std::string filename, DWORD attributes, DWORD creationDisposition) {
		this->filename = filename;
		this->attributes = attributes;
		this->creationDisposition = creationDisposition;
	}

	// Getter functions
	std::string GetFilename() {
		return filename;
	}

	DWORD GetAttributes() {
		return attributes;
	}

	DWORD GetCreationDisposition() {
		return creationDisposition;
	}

	DWORD GetFileSize() {
		return fileSize;
	}

	HANDLE getHandle() {
		return h;
	}

	// Basic file functions
	BOOL Exists() {
		return !(GetFileAttributesA(filename.c_str()) == INVALID_FILE_ATTRIBUTES);
	}

	BOOL Open() {
		h = CreateFileA(GetFilename().c_str(), GetAttributes(), 0, NULL, GetCreationDisposition(), FILE_ATTRIBUTE_NORMAL, NULL);

		if(!(h == INVALID_HANDLE_VALUE)) {
			fileSize = ::GetFileSize(h, NULL);
			return true;
		}

		return false;
	}

	VOID Close() {
		if(h == INVALID_HANDLE_VALUE)
			return;

		CloseHandle(h);
	}

	// Read Functions
	BOOL Read(void* buffer, DWORD length) {
		DWORD dummy = 0;
		return (ReadFile(h, buffer, length, &dummy, NULL) == TRUE);
	}

	template<typename T> T* ReadTotal(DWORD* pdwFileSize) {
		DWORD dwFileSize = ::GetFileSize(h, NULL);

		if(dwFileSize == INVALID_FILE_SIZE)
			return NULL;

		if(pdwFileSize) {
			*pdwFileSize = dwFileSize;
		}

		T* r = new T[dwFileSize / sizeof(T)];

		if(r == NULL)
			return NULL;

		BOOL result = Read(r, dwFileSize);

		if(result == FALSE) {
			delete[] r;

			return NULL;
		}

		return r;
	}

	std::string ReadToEnd(DWORD* pdwFileSize) {
		char* total = ReadTotal<char>(pdwFileSize);
		std::string r = total;
		delete[] total;
		return r;
	}

	// Append Functions
	BOOL Append(void* buffer, DWORD length) {
		SetFilePointer(h, 0, 0, FILE_END);

		return Write(buffer, length);
	}

	BOOL Append(const char* buffer) {
		return Append((void*) buffer, strlen(buffer));
	}

	BOOL Append(std::string buffer) {
		return Append(buffer.c_str());
	}

	template<typename T> BOOL Append(T data) {
		return Append(&data, sizeof(T));
	}

	// Write Functions
	BOOL Write(void* buffer, DWORD length) {
		DWORD dummy = 0;
		return (WriteFile(h, buffer, length, &dummy, NULL) == TRUE);
	}

	BOOL Write(const char* buffer) {
		return Write((void*) buffer, strlen(buffer));
	}

	BOOL Write(std::string buffer) {
		return Write(buffer.c_str());
	}

	template<typename T> BOOL Write(T data) {
		return Write(&data, sizeof(T));
	}

protected:
	std::string filename;
	DWORD attributes;
	DWORD creationDisposition;
	DWORD fileSize;

	HANDLE h;
};