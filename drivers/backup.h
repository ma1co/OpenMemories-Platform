#pragma once

int Backup_get_datasize(int id);
int Backup_get_attribute(int id);
int Backup_read(int id, void *addr);
int Backup_write(int subsystem_id, int id, void *addr);
void Backup_sync_all();
int Backup_protect(int mode, void *overwrite_data, int size);
