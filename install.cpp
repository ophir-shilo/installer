#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <stdexcept>
using namespace std;

const int FILES_COUNT = 3;

class Installer {
    private:
        wstring srcDirectory;
        wstring destDirectory;
        wstring files[FILES_COUNT] = { L"file1.txt", L"file2.txt", L"file3.txt" };
        bool hasCreatedDestDir = false;
        bool ensureDestDirCreated() {
            DWORD ftyp = GetFileAttributesW(destDirectory.c_str());
            if (ftyp == INVALID_FILE_ATTRIBUTES) {
                // Path not exists
                int createDirectoryVal = CreateDirectoryExW(srcDirectory.c_str(), destDirectory.c_str(), NULL);
                printf("Create directory return value is %d\n", createDirectoryVal);
                if (createDirectoryVal == 0) {
                    throw runtime_error("Create directory failed");
                    return false;
                }
                hasCreatedDestDir = true;
                return true;
            }
            if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
                // Path exists and it is a directory
                return true;
            }
            // Path exists but it not a directory
            throw runtime_error("Dest path exists but it not a directory");
            return false;
        }
        void copyFiles() {
            for (int i = 0; i < FILES_COUNT; i++) {
                wstring srcPath = srcDirectory + L"\\" + files[i];
                wstring destPath = destDirectory + L"\\" + files[i];
                printf("Copying file %ls to %ls\n", srcPath.c_str(), destPath.c_str());
                int copyFileVal = CopyFileW(srcPath.c_str(), destPath.c_str(), false);
                printf("Copy file return value is %d\n", copyFileVal);
                if (copyFileVal == 0) {
                    throw runtime_error("Copy file failed");
                }
            }
        }
        void cleanDest() {
            printf("Starting cleanup\n");
            for (int i = 0; i < 3; i++) {
                wstring destPath = destDirectory + L"\\" + files[i];
                // Remove the readonly flag for deleting the file
                SetFileAttributesW(destPath.c_str(), GetFileAttributesW(destPath.c_str()) & ~FILE_ATTRIBUTE_READONLY);
                printf("Deleting file %ls\n", destPath.c_str());
                int val = DeleteFileW(destPath.c_str());
                printf("Return value is %d\n", val);
                // TODO: Handle failures
            }
            if (hasCreatedDestDir) {
                printf("Deleting directory %ls\n", destDirectory.c_str());
                int val = RemoveDirectoryW(destDirectory.c_str());
                printf("Return value is %d\n", val);
            }
        }
    
    public:
        Installer(const wstring& srcDir, const wstring& dstDir) {
            srcDirectory = srcDir;
            destDirectory = dstDir;
        }
        bool install() {
            try {
                ensureDestDirCreated();
                copyFiles();
                return true;
            }
            catch (...) {
                cleanDest();
                throw;
            }
        }
};

int wmain(int argc, TCHAR* argv[])
{
    wstring srcDirectory = L"C:\\Users\\Ophir\\Documents\\tmp\\tests\\src";
    wstring destDirectory = L"C:\\Users\\Ophir\\Documents\\tmp\\tests\\dst";

    Installer installer = Installer(srcDirectory, destDirectory);
    if (installer.install()) {
        return 0;
    }
    return 1;
}