#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const char* PATH_TO_FILE_READER = "/home/ilya/Documents/Projects/Qt/IPC/FileReader/build/Desktop-Debug/FileReader";
const char* SHM_NAME = "/my_shared_memory";
const size_t SHM_SIZE = 4096;

std::string readFromSharedMemory()
{
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    void* ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    std::string data(static_cast<char*>(ptr), SHM_SIZE);
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    return data;
}

void xorFiles(const std::string& outputFilename, const std::string& data1, const std::string& data2)
{
    std::cout << "DATA 1::\n " << data1 << "\n\n" << "DATA 2::\n " << data2;
    std::ofstream outFile(outputFilename, std::ios::binary);

    for (size_t i = 0, j = 0; i < data1.size(); ++i, ++j)
    {
        if(j == data2.size())
        {
            j = 0;
        }
        outFile << (char)(data1[i] ^ data2[j]);
    }

    outFile.close();
}

std::string get_file_data(const std::string& file_path)
{
    pid_t process_id = fork();
    if (process_id == 0)
    {
        execl(PATH_TO_FILE_READER, PATH_TO_FILE_READER, file_path.c_str(), nullptr);
        exit(0);
    }

    waitpid(process_id, nullptr, 0);
    return readFromSharedMemory();
}

int main()
{
    std::string path_to_first_file, path_to_second_file;

    std::cout << "Path to frist file: ";
    std::cin >>path_to_first_file;

    std::cout << "Path to second file: ";
    std::cin >>path_to_second_file;

    std::string firs_file_data = get_file_data(path_to_first_file);
    std::string second_file_data = get_file_data(path_to_second_file);

    xorFiles("output.txt", firs_file_data, second_file_data);

    system("pause");
    return 0;
}
