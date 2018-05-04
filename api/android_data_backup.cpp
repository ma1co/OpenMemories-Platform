#include <fcntl.h>
#include <sys/mount.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include "android_data_backup.hpp"
#include "util.hpp"

using namespace std;

struct bk_header
{
    unsigned short error;
    unsigned short version;
    size_t data_size;
    size_t system_size;
};

static const int num_bk_files = 2;
static const char *data_ramdisk = "/dev/ram8";
static const char *system_ramdisk = "/dev/ram9";

static int read_bk_header(const char *bk_file, bk_header *header)
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

static int write_bk_header(const char *bk_file, const bk_header *header)
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
    return mount(disk_fn, dir, "ext2", MS_SYNCHRONOUS, NULL);
}

static string get_bk_filename(const char *mount_dir, int id)
{
    return string_format("%s/backup/bk%d.bak", mount_dir, id + 1);
}

static string get_data_dir(const char *mount_dir)
{
    return string_format("%s/data", mount_dir);
}

static string get_system_dir(const char *mount_dir)
{
    return string_format("%s/system", mount_dir);
}

static string android_data_backup_select(const char *mount_dir, bk_header *last_header)
{
    string last_file = "";

    for (int i = 0; i < num_bk_files; i++) {
        string file = get_bk_filename(mount_dir, i);
        bk_header header;
        if (!read_bk_header(file.c_str(), &header) && !header.error) {
            if (last_file == "" || header.version > last_header->version) {
                last_file = file;
                *last_header = header;
            }
        }
    }

    if (last_file == "")
        throw android_data_backup_error("Bk file unavailable");

    return last_file;
}

void android_data_backup_mount(const char *mount_dir)
{
    bk_header header;
    const char *bk_file = android_data_backup_select(mount_dir, &header).c_str();

    if (copy_file(bk_file, sizeof(header), data_ramdisk, 0, header.data_size, true))
        throw android_data_backup_error("Cannot read data partition");
    if (copy_file(bk_file, sizeof(header) + header.data_size, system_ramdisk, 0, header.system_size, true))
        throw android_data_backup_error("Cannot read system partition");

    if (mount_ramdisk(data_ramdisk, get_data_dir(mount_dir).c_str()))
        throw android_data_backup_error("Cannot mount data partition");
    if (mount_ramdisk(system_ramdisk, get_system_dir(mount_dir).c_str()))
        throw android_data_backup_error("Cannot mount system partition");
}

void android_data_backup_unmount(const char *mount_dir, bool commit)
{
    if (umount(get_data_dir(mount_dir).c_str()))
        throw android_data_backup_error("Cannot unmount data partition");
    if (umount(get_system_dir(mount_dir).c_str()))
        throw android_data_backup_error("Cannot unmount system partition");

    if (commit) {
        bk_header header;
        android_data_backup_select(mount_dir, &header);

        const int id = 0;
        const char *bk_file = get_bk_filename(mount_dir, id).c_str();
        header.error = 0;
        header.version = 0;

        if (write_bk_header(bk_file, &header))
            throw android_data_backup_error("Cannot write header");
        if (copy_file(data_ramdisk, 0, bk_file, sizeof(header), header.data_size, false))
            throw android_data_backup_error("Cannot write data partition");
        if (copy_file(system_ramdisk, 0, bk_file, sizeof(header) + header.data_size, header.system_size, false))
            throw android_data_backup_error("Cannot write system partition");

        for (int i = 0; i < num_bk_files; i++) {
            if (i != id)
                unlink(get_bk_filename(mount_dir, i).c_str());
        }
    }
}
