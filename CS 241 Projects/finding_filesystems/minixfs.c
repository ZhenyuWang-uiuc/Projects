/**
 * finding_filesystems
 * CS 241 - Spring 2022
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE ((NUM_DIRECT_BLOCKS + NUM_INDIRECT_BLOCKS) * 16 * KILOBYTE)

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string,
             "Free blocks: %zd\n"
             "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    inode* node = get_inode(fs, path);
    if (node == NULL) {
        errno = ENOENT;
        return -1;
    }

    node->mode = (new_permissions & 0x000001ff) | ((node->mode >> 9) << 9);
    clock_gettime(CLOCK_REALTIME, &node->ctim);
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    inode* node = get_inode(fs, path);
    if (node == NULL) {
        errno = ENOENT;
        return -1;
    }

    bool flag = false;
    if (node->uid != (uid_t) -1) { node->uid = owner; flag = true; }
    if (node->gid != (gid_t) -1) { node->gid = group; flag = true; }

    /* update the ctime field */
    if (flag) {
        clock_gettime(CLOCK_REALTIME, &node->ctim);
        return 0;
    }

    return -1;
}

uint64_t calculate_num_full_blocks(uint64_t size) {
    unsigned ct = 0;
    while (size >= 16 * KILOBYTE) {
        size -= 16 * KILOBYTE;
        ct ++;
    }
    return ct;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    /* can't create a file */
    if (path == NULL || fs == NULL) {
        return NULL;
    }

    /* inode already exists */
    inode* node = get_inode(fs, path);
    if (node != NULL) {
        return NULL;
    }

    /* create a file */
    const char* filename = NULL;
    inode* parent = parent_directory(fs, path, &filename);
    inode_number n = first_unused_inode(fs);

    if (n != -1 && parent) {
        /* step 1: init inode */
        init_inode(parent, fs->inode_root + n);

        /* step 2: create a dirent */
        minixfs_dirent dirent;
        dirent.name = (char*)filename;
        dirent.inode_num = n;

        /* step 3: write the dirent on the disk */
        bool write = false;
        if (parent->size + 256 <= MAX_SIZE) {
            int target_index = calculate_num_full_blocks(parent->size);
            unsigned offset = parent->size % (16 * KILOBYTE);

            if (target_index < NUM_DIRECT_BLOCKS) {
                target_index = parent->direct[target_index];
                if (target_index == -1)
                    target_index = add_data_block_to_inode(fs, parent);
                
                if (target_index != -1) {
                    make_string_from_dirent((target_index + fs->data_root)->data + offset, dirent);
                    write = true;
                }
            } else {
                if (add_single_indirect_block(fs, parent) != -1) {
                    target_index -= NUM_DIRECT_BLOCKS;
                    data_block_number* block_array = (data_block_number*)(parent->indirect + fs->data_root)->data;
                    target_index = block_array[target_index];
                    if (target_index == -1)
                        target_index = add_data_block_to_indirect_block(fs, &parent->indirect);

                    if (target_index != -1) {
                        make_string_from_dirent((target_index + fs->data_root)->data + offset, dirent);
                        write = true;
                    }
                }
            }
        }

        if (write) {
            parent->size += FILE_NAME_ENTRY;
            return fs->inode_root + n;
        } else {
            free_inode(fs, fs->inode_root + n);
        }
    }

    return NULL;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        ssize_t ct = 0;
        for (uint64_t i = 0; i < fs->meta->dblock_count; i++) {
            if (get_data_used(fs, i)) 
                ct ++;
        }

        char* ret = block_info_string(ct) + *off;
        ssize_t copied_size = (count > strlen(ret)) ? strlen(ret) : count;
        memcpy(buf, ret, copied_size);
        *off = *off + copied_size;
        return copied_size;
    }

    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    inode* node = get_inode(fs, path);
    if (node == NULL) {
        node = minixfs_create_inode_for_path(fs, path);
        *off = 0;
    }

    if (count > MAX_SIZE || count + node->size > MAX_SIZE) {
        errno = ENOSPC;
        return -1;
    }

    if (*off > (off_t) node->size) {
        *off = node->size;
    }
    
    unsigned num_full_blocks = calculate_num_full_blocks(*off);
    unsigned offset = *off % (16 * KILOBYTE);
    unsigned ct = count;

    int target_index = num_full_blocks;

    char* temp_buf = (char*)buf;

    while (ct != 0) {
        size_t num_bytes_copied = (ct > 16 * KILOBYTE - offset) ? 16 * KILOBYTE - offset : ct;
        if (target_index < NUM_DIRECT_BLOCKS) {  // write data to direct blocks
            target_index = node->direct[target_index];
            if (target_index == -1)
                target_index = add_data_block_to_inode(fs, node);

            if (target_index == -1) {
                errno = ENOSPC;
                return -1;
            }
            memcpy((target_index + fs->data_root)->data + offset, temp_buf, num_bytes_copied);
        } else {  // write data to indirect blocks
            if (add_single_indirect_block(fs, node) == -1) {
                errno = ENOSPC;
                return -1;
            }
            target_index -= NUM_DIRECT_BLOCKS;
            data_block_number* block_array = (data_block_number*)(node->indirect + fs->data_root)->data;
            target_index = block_array[target_index];
            if (target_index == -1)
                target_index = add_data_block_to_indirect_block(fs, &node->indirect);

            if (target_index == -1) {
                errno = ENOSPC;
                return -1;
            }
            memcpy((target_index + fs->data_root)->data + offset, temp_buf, num_bytes_copied);
        }

        /* update varibales */
        temp_buf = temp_buf + num_bytes_copied;
        offset = 0;
        num_full_blocks ++;
        target_index = num_full_blocks;
        ct -= num_bytes_copied;
    }

    /* update *off and size */
    *off = *off + count;
    node->size += count;

    /* update time */
    clock_gettime(CLOCK_REALTIME, &node->atim);
    clock_gettime(CLOCK_REALTIME, &node->mtim);

    return count;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    
    inode* node = get_inode(fs, path);
    if (node == NULL) {
        errno = ENOENT;
        return -1;
    }

    if (*off > (off_t) node->size) {
        return 0;
    }

    unsigned num_full_blocks = calculate_num_full_blocks(*off);
    unsigned offset = *off % (16 * KILOBYTE);
    unsigned long index = 0;

    int target_index = num_full_blocks;
    int target_index_cpy;

    size_t ct = (count > node->size - *off) ? node->size - *off : count;
    ssize_t ret = ct;

    while (ct != 0) {
        size_t num_bytes_copied = (ct > 16 * KILOBYTE - offset) ? 16 * KILOBYTE - offset : ct;
        if (target_index < NUM_DIRECT_BLOCKS) {
            /* copy original target_index value, i.e., copy the offset of the data_root */
            target_index_cpy = target_index;

            /* read from fs */
            target_index = node->direct[target_index];
            memcpy(buf + index, (target_index + fs->data_root)->data + offset, num_bytes_copied);

            /* change target_index back to the offse of the data_root */
            target_index = target_index_cpy;
        } else {
            /* copy original target_index value, i.e., copy the offset of the data_root */
            target_index_cpy = target_index;
            target_index -= NUM_DIRECT_BLOCKS;

            /* read from fs */
            data_block_number* block_array = (data_block_number*)(node->indirect + fs->data_root)->data;
            target_index = block_array[target_index];
            memcpy(buf + index, (target_index + fs->data_root)->data + offset, num_bytes_copied);

            /* change target_index back to the offse of the data_root */
            target_index = target_index_cpy;
        }

        offset = 0;
        num_full_blocks ++;
        target_index = num_full_blocks;
        index += num_bytes_copied;
        ct -= num_bytes_copied;
    }

    /* update *off and size */
    *off = *off + ret;

    /* update time */
    clock_gettime(CLOCK_REALTIME, &node->atim);

    return ret;
}
