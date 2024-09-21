#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

const char* SHM_NAME = "/my_shared_memory";
const size_t SHM_SIZE = 4096;

void writeToSharedMemory(const std::string& data)
{
    //создание или подключение объекта разделяемой памяти POSIX по его имени
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    //задаёт или изменяет размер разделяемой памяти (или отображённого в память файла)
    ftruncate(shm_fd, SHM_SIZE);
    // подключает существующий или создаёт анонимный сегмент разделяемой памяти к адресному пространству процесса
    void* ptr = mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    //очистка памяти
    memset(ptr, 0, SHM_SIZE);
    memcpy(ptr, data.c_str(), data.size());
    // освобождает все сопоставления для региона, начиная с ptr и заканчивая SHM_SIZE.
    munmap(ptr, SHM_SIZE);
    close(shm_fd);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    writeToSharedMemory(content);

    std::cout << "file " << argv[1] << " finished\n";
    return 0;
}
