#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <filesystem>
#include <stdexcept>
using namespace std;
namespace fs = std::filesystem;

const int FILES_COUNT = 3;

bool dirExists(const fs::path dirPath) {
    DWORD ftyp = GetFileAttributesW(dirPath.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
}

class Installer {
    private:
        fs::path srcDirectory;
        fs::path destDirectory;
        fs::path files[FILES_COUNT] = { L"file1.txt", L"file2.txt", L"file3.txt" };
        bool hasCreatedDestDir = false;
        void createDestDir() {
            hasCreatedDestDir = true;
            // TODO: Create recursively
            int createDirectoryVal = CreateDirectoryExW(srcDirectory.c_str(), destDirectory.c_str(), NULL);
            _tprintf(TEXT("Create directory return value is %d\n"), createDirectoryVal);
            if (createDirectoryVal == 0) {
                throw runtime_error("Create directory failed");
            }
        }
        void copyFiles() {
            for (int i = 0; i < FILES_COUNT; i++) {
                fs::path srcPath = srcDirectory / files[i];
                fs::path destPath = destDirectory / files[i];
                _tprintf(TEXT("Copying file %s to %s\n"), (char*) srcPath.c_str(), (char*) destPath.c_str());
                int copyFileVal = CopyFileW(srcPath.c_str(), destPath.c_str(), false);
                _tprintf(TEXT("Copy file return value is %d\n"), copyFileVal);
                if (copyFileVal == 0) {
                    throw runtime_error("Copy file failed");
                }
            }
        }
        void cleanDest() {
            _tprintf(TEXT("Starting cleanup\n"));
            for (int i = 0; i < 3; i++) {
                fs::path destPath = destDirectory / files[i];
                // Remove the readonly flag for deleting the file
                SetFileAttributesW(destPath.c_str(), GetFileAttributesW(destPath.c_str()) & ~FILE_ATTRIBUTE_READONLY);
                _tprintf(TEXT("Deleting file %s\n"), (char*) destPath.c_str());
                int val = DeleteFileW(destPath.c_str());
                _tprintf(TEXT("Return value is %d\n"), val);
                // TODO: Handle failures
            }
            if (hasCreatedDestDir) {
                _tprintf(TEXT("Deleting directory %s\n"), (char*) destDirectory.c_str());
                int val = RemoveDirectoryW(destDirectory.c_str());
                _tprintf(TEXT("Return value is %d\n"), val);
            }
        }
    
    public:
        Installer(const string& srcDir, const string& dstDir) {
            srcDirectory = srcDir;
            destDirectory = dstDir;
        }
        bool install() {
            try {
                if (!dirExists(destDirectory)) {
                    createDestDir();
                }
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
    string srcDirectory = "C:\\Users\\Ophir\\Documents\\tmp\\tests\\src";
    string destDirectory = "C:\\Users\\Ophir\\Documents\\tmp\\tests\\dst";

    Installer installer = Installer(srcDirectory, destDirectory);
    if (installer.install()) {
        return 0;
    }
    return 1;
}