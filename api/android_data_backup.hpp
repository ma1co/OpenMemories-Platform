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

void android_data_backup_mount(const char *mount_dir);
void android_data_backup_unmount(const char *mount_dir, bool commit);
