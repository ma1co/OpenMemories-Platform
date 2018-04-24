#include <fcntl.h>
#include <sys/mount.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include "android_data_backup.hpp"
#include "util.hpp"

using namespace std;

static const int num_bk_files = 2;
static const char *data_ramdisk = "/dev/ram8";
static const char *system_ramdisk = "/dev/ram9";

static int read_bk_header(const char *bk_file, struct bk_header *header)
{
    int bk_fd;
    if ((bk_fd = open(bk_file, O_RDONLY)) < 0)
        return -1;
    if (read(bk_fd, header, sizeof(*header)) != sizeof(*header))
        return -1;
    if (close(bk_fd))
        return -1;
    return 0;
}

static int write_bk_header(const char *bk_file, const struct bk_header *header)
{
    int bk_fd;
    if ((bk_fd = open(bk_file, O_WRONLY | O_SYNC)) < 0)
        return -1;
    if (write(bk_fd, header, sizeof(*header)) != sizeof(*header))
        return -1;
    if (close(bk_fd))
        return -1;
    return 0;
}

static int copy_file(const char *in_file, size_t in_offset, const char *out_file, size_t out_offset, size_t size, bool out_truncate)
{
    int in_fd, out_fd;
    if ((in_fd = open(in_file, O_RDONLY)) < 0)
        return -1;
    if ((out_fd = open(out_file, O_WRONLY | O_SYNC | (out_truncate ? O_TRUNC : 0))) < 0)
        return -1;
    if (lseek(in_fd, in_offset, SEEK_SET) < 0)
        return -1;
    if (lseek(out_fd, out_offset, SEEK_SET) < 0)
        return -1;
    if (sendfile(out_fd, in_fd, NULL, size) != (int) size)
        return -1;
    if (close(in_fd))
        return -1;
    if (close(out_fd))
        return -1;
    return 0;
}

static int mount_ramdisk(const char *disk_fn, const char *dir)
{
    return mount(disk_fn, dir, "ext2", MS_SYNCHRONOUS | MS_DIRSYNC, NULL);
}

string AndroidDataBackup::get_dir()
{
    return dir;
}

string AndroidDataBackup::get_bk_filename(int id)
{
    return string_format("%s/backup/bk%d.bak", dir.c_str(), id + 1);
}

string AndroidDataBackup::get_data_dir()
{
    return string_format("%s/data", dir.c_str());
}

string AndroidDataBackup::get_system_dir()
{
    return string_format("%s/system", dir.c_str());
}

void AndroidDataBackup::initialize()
{
    last_id = -1;
    for (int i = 0; i < num_bk_files; i++) {
        bk_header header;
        if (!read_bk_header(get_bk_filename(i).c_str(), &header) && !header.error) {
            if (last_id < 0 || header.version > last_header.version) {
                last_id = i;
                last_header = header;
            }
        }
    }
}

bool AndroidDataBackup::is_available()
{
    return last_id >= 0;
}

void AndroidDataBackup::write_header()
{
    if (last_id < 0)
        throw android_data_backup_error("Bk file unavailable");
    if (write_bk_header(get_bk_filename(last_id).c_str(), &last_header))
        throw android_data_backup_error("Cannot write bk header");
}

void AndroidDataBackup::read()
{
    if (last_id < 0)
        throw android_data_backup_error("Bk file unavailable");
    const char *bk_file = get_bk_filename(last_id).c_str();
    if (copy_file(bk_file, sizeof(last_header), data_ramdisk, 0, last_header.data_size, true))
        throw android_data_backup_error("Cannot read data partition");
    if (copy_file(bk_file, sizeof(last_header) + last_header.data_size, system_ramdisk, 0, last_header.system_size, true))
        throw android_data_backup_error("Cannot read system partition");
}

void AndroidDataBackup::write()
{
    if (last_id < 0)
        throw android_data_backup_error("Bk file unavailable");
    const char *bk_file = get_bk_filename(last_id).c_str();
    if (copy_file(data_ramdisk, 0, bk_file, sizeof(last_header), last_header.data_size, false))
        throw android_data_backup_error("Cannot write data partition");
    if (copy_file(system_ramdisk, 0, bk_file, sizeof(last_header) + last_header.data_size, last_header.system_size, false))
        throw android_data_backup_error("Cannot write system partition");
}

void AndroidDataBackup::commit()
{
    if (last_id < 0)
        throw android_data_backup_error("Bk file unavailable");
    last_id = (last_id + 1) % num_bk_files;
    last_header.version++;

    last_header.error = 1;
    write_header();
    write();
    last_header.error = 0;
    write_header();
}

void AndroidDataBackup::mount()
{
    if (mount_ramdisk(data_ramdisk, get_data_dir().c_str()))
        throw android_data_backup_error("Cannot mount data partition");
    if (mount_ramdisk(system_ramdisk, get_system_dir().c_str()))
        throw android_data_backup_error("Cannot mount system partition");
}

void AndroidDataBackup::unmount()
{
    if (umount(get_data_dir().c_str()))
        throw android_data_backup_error("Cannot unmount data partition");
    if (umount(get_system_dir().c_str()))
        throw android_data_backup_error("Cannot unmount system partition");
}
