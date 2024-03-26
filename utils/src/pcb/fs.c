#include "fs.h"

int serialized_fopen_size(t_fopen *fopen)
{
	return sizeof(char) * (strlen(fopen->file_name) + 1) + sizeof(char) * (strlen(fopen->open_mode) + 1) + 2 * sizeof(int);
}

void *serialize_fopen(t_fopen *fopen)
{
	void *buffer = malloc(serialized_fopen_size(fopen));
	int offset = 0;
	int file_name_size = strlen(fopen->file_name) + 1;
	memcpy(buffer + offset, &file_name_size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, fopen->file_name, sizeof(char) * file_name_size);
	offset += sizeof(char) * file_name_size;
	int open_mode_size = strlen(fopen->open_mode) + 1;
	memcpy(buffer + offset, &open_mode_size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, fopen->open_mode, sizeof(char) * open_mode_size);
	offset += sizeof(char) * open_mode_size;
	return buffer;
}

t_fopen *deserialize_fopen(void *buffer)
{
	t_fopen *fopen = malloc(sizeof(*fopen));
	int offset = 0;
	int file_name_size;
	memcpy(&file_name_size, buffer + offset, sizeof(int));
	offset += sizeof(int);
	char *file_name = malloc(sizeof(char) * file_name_size);
	memcpy(file_name, buffer + offset, sizeof(char) * file_name_size);
	fopen->file_name = file_name;
	offset += file_name_size;
	int open_mode_size;
	memcpy(&open_mode_size, buffer + offset, sizeof(int));
	offset += sizeof(int);
	char *open_mode = malloc(sizeof(char) * open_mode_size);
	memcpy(open_mode, buffer + offset, sizeof(char) * open_mode_size);
	fopen->open_mode = open_mode;
	offset += open_mode_size;
	return fopen;
}

int serialized_fchange_size(t_fchange *fchange)
{
	return sizeof(char) * (strlen(fchange->file_name) + 1) + 2 * sizeof(int);
}

void *serialize_fchange(t_fchange *fchange)
{
	void *buffer = malloc(serialized_fchange_size(fchange));
	int offset = 0;
	int file_name_size = strlen(fchange->file_name) + 1;
	memcpy(buffer + offset, &file_name_size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, fchange->file_name, sizeof(char) * file_name_size);
	offset += sizeof(char) * file_name_size;
	memcpy(buffer + offset, &fchange->value, sizeof(int));
	offset += sizeof(int);
	return buffer;
}

t_fchange *deserialize_fchange(void *buffer)
{
	t_fchange *fchange = malloc(sizeof(*fchange));
	int offset = 0;
	int file_name_size;
	memcpy(&file_name_size, buffer + offset, sizeof(int));
	offset += sizeof(int);
	char *file_name = malloc(sizeof(char) * file_name_size);
	memcpy(file_name, buffer + offset, sizeof(char) * file_name_size);
	fchange->file_name = file_name;
	offset += file_name_size;
	memcpy(&fchange->value, buffer + offset, sizeof(int));
	offset += sizeof(int);
	return fchange;
}

int serialized_fmodify_size(t_fmodify *fmodify)
{
	return size_of_page() + sizeof(char) * (strlen(fmodify->file_name) + 1) + sizeof(int);
}

void *serialize_fmodify(t_fmodify *fmodify)
{
	void *buffer = malloc(serialized_fmodify_size(fmodify));
	int offset = 0;
	int file_name_size = strlen(fmodify->file_name) + 1;
	memcpy(buffer + offset, &file_name_size, sizeof(int));
	offset += sizeof(int);
	memcpy(buffer + offset, fmodify->file_name, sizeof(char) * file_name_size);
	offset += sizeof(char) * file_name_size;
	void *serialized_page = serialize_page(fmodify->page);
	memcpy(buffer + offset, serialized_page, size_of_page());
	free(serialized_page);
	offset += size_of_page();
	return buffer;
}

t_fmodify *deserialize_fmodify(void *buffer)
{
	t_fmodify *fmodify = malloc(sizeof(*fmodify));
	int offset = 0;
	int file_name_size;
	memcpy(&file_name_size, buffer + offset, sizeof(int));
	offset += sizeof(int);
	char *file_name = malloc(sizeof(char) * file_name_size);
	memcpy(file_name, buffer + offset, sizeof(char) * file_name_size);
	fmodify->file_name = file_name;
	offset += file_name_size;
	void *buffer_page = malloc(size_of_page());
	memcpy(buffer_page, buffer + offset, size_of_page());
	fmodify->page = deserialize_page(buffer_page);
	return fmodify;
}
