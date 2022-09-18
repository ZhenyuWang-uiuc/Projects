/**
 * finding_filesystems
 * CS 241 - Spring 2022
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Write tests here!
    // ./fakefs test.fs cat test.fs/goodies/hello.txt
    // cat ./goodies/hello.txt

    // ./fakefs test.fs cat test.fs/

    // file_system* fs = open_fs("test.fs");
    // char buf[2048];
    // off_t off = 0;
    // ssize_t bytes_read = minixfs_read(fs, "/goodies/hello.txt", buf, 2048, &off);
    // close_fs(&fs);
    // fprintf(stderr, "%zd\n", bytes_read);
    // fprintf(stderr, "%s\n", buf);

    file_system* fs = open_fs("test.fs");
    char buf[2048];
    off_t off = 0;
    ssize_t bytes_read = minixfs_read(fs, "/virtual/info", buf, 2048, &off);
    close_fs(&fs);
    fprintf(stderr, "%zd\n", bytes_read);
    fprintf(stderr, "%s\n", buf);

    // ./fakefs test.fs touch temp.txt

    // ./fakefs test.fs cat goodies/dog.png > newerdog.png
    // convert newerdog.png a.rgba
    // convert goodies/dog.png b.rgba
    // cmp {a,b}.rgba

    // file_system* fs = open_fs("test.fs");
    // char* buf = "hello world";
    // off_t off = 0;
    // ssize_t bytes_written = minixfs_write(fs, "/goodies/nonexistant.txt", buf, 12, &off);
    // fprintf(stderr, "bytes written: %zd\n", bytes_written);
    // fprintf(stderr, "new offset: %zd\n", off);
    // char res[13];
    // off = 0;
    // ssize_t bytes_read = minixfs_read(fs, "/goodies/nonexistant.txt", res, 11, &off);
    // fprintf(stderr, "bytes read: %zd\n", bytes_read);
    // fprintf(stderr, "new offset: %zd\n", off);
    // fprintf(stderr, "output: `%s`\n", res);
    // close_fs(&fs);
}