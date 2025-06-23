#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"

void
find(char *path, char *target) 
{

    char buf[512], *p; //buf用来储存目录， p位置
    int fd;
    struct stat st;  //获取文件元数据
    struct dirent de; //目录下的信息

    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot find %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0) { //获取文件元数据
        fprintf(2, "find: cannot stat %s/n", path);
        close(fd);
        return;
    }

    switch(st.type) {
        case T_FILE:
            if(strcmp(path + strlen(path) - strlen(target), target) == 0) {
                fprintf(1, "%s\n", path);
            }
            break;
        
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {   //strlen(path): 原始路径长     +1: 添加的 /      +DIRSIZ: 文件名最大长度        +1: 终止符 \0
                fprintf(2, "find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)) { //有偏移
                if(de.inum == 0) {
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0) {
                    fprintf(2, "find: cannot stat %s\n", buf);
                    continue;
                }
                if(strcmp(buf + strlen(buf) - 1, ".") != 0 && strcmp(buf + strlen(buf) - 2, "..") != 0) {
                    find(buf, target);
                }
            }
            break;
    }
    close(fd);
            

    

}




//3个参数 分别为 find 路径  文件名
int
main(int argc, char *argv[])
{

    if (argc < 3) {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }
    
    find(argv[1], argv[2]);
    exit(0);
   


}