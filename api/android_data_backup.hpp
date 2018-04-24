#pragma once
#include <stdexcept>
#include <string>

#ifdef GEN2
#define ANDROID_DATA_DEV "/dev/nflashb2"
#else
#define ANDROID_DATA_DEV "/dev/nflasha17"
#endif

class android_data_backup_error : public std::runtime_error
{
public:
    android_data_backup_error(const std::string& msg) : std::runtime_error(msg) {}
};

struct bk_header
{
    unsigned short error;
    unsigned short version;
    size_t data_size;
    size_t system_size;
};

class AndroidDataBackup
{
protected:
    std::string dir;
    int last_id;
    bk_header last_header;

    std::string get_bk_filename(int id);
    std::string get_data_dir();
    std::string get_system_dir();
    void write_header();
    void write();

public:
    AndroidDataBackup(std::string dir) : dir(dir), last_id(-1) {}
    std::string get_dir();
    void initialize();
    bool is_available();
    void read();
    void commit();
    void mount();
    void unmount();
};
