#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/martin/Documents";
static const char *key = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

void dekrip1(char* res, char* name) {
    strcpy(res, "");
    int resmod = 87;
    int dekrip = 1;
    char* new = name;
    int slash = 0;
    if (name[0] == '/') {
        name++;
        slash = 1;
    }
    int first = 1;
    char* tok;
    tok = strtok(new, "/");
    while (tok != NULL) {
        char rev[1000];
        int index = 0;
        for (int i = 0; i < strlen(tok); i++) {
            if (tok[i]=='.') {
                dekrip = 0;
            }
            if (dekrip) {
                int pos = 0, z = 0;
                while(1) {
                    if (key[z]==tok[i])
                        break;
                    z++;
                }
                pos = (z + 87 - 10) % resmod;
                rev[index] = key[pos];
            }
            else {
                rev[index] = tok[i];
            }
            index++;
        }
        rev[index] = '\0';
        if (!first || slash) strcat(res, "/");
        first = 0;
        strcat(res, rev);
        tok = strtok(NULL, "/");
    }
}

void enkrip1(char* res, char* name) {
    strcpy(res, "");
    int resmod = 87;
    int enkrip = 1;
    char* new = name;
    int slash = 0;
    if (name[0] == '/') {
        name++;
        slash = 1;
    }
    int first = 1;
    char* tok;
    tok = strtok(new, "/");
    while (tok != NULL) {
        char rev[1000];
        int index = 0;
        for (int i = 0; i < strlen(tok); i++) {
            if (tok[i]=='.') {
                enkrip = 0;
            }
            if (enkrip) {
                int pos = 0, z = 0;
                while(1) {
                    if (key[z]==tok[i])
                        break;
                    z++;
                }
                pos = (z + 10) % resmod;
                rev[index] = key[pos];
            }
            else {
                rev[index] = tok[i];
            }
            index++;
        }
        rev[index] = '\0';
        if (!first || slash) strcat(res, "/");
        first = 0;
        strcat(res, rev);
        tok = strtok(NULL, "/");
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int res,enable=0;
    char fpath[1000];
    char befencv[1000] = "", afencv[1000] = "";
    char s[1000];

    char *tok = strtok(path, "/");
    while (tok!=NULL) {
        if (enable==0) {
            strcat(befencv, "/");
            strcat(befencv, tok);
        }
        else if (enable==1) {
            strcat(afencv, "/");
            strcat(afencv, tok);
        }
        if (strncmp(tok, "encv1_", 6) == 0)
            enable=1;
        tok = strtok(NULL, "/");
    }
    dekrip1(s, afencv);
    sprintf(fpath, "%s%s%s", dirpath, befencv, s);
    res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    int enable=0;
    if(strcmp(path,"/") == 0) {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else {
        char s[1000];
        char befencv[1000] = "", afencv[1000] = "";

        char *tok = strtok(path, "/");
        while (tok!=NULL) {
            if (enable==0) {
                strcat(befencv, "/");
                strcat(befencv, tok);
            }
            else if (enable==1) {
                strcat(afencv, "/");
                strcat(afencv, tok);
            }

            if (strncmp(tok, "encv1_", 6) == 0)
                enable=1;
            tok = strtok(NULL, "/");
        }
        
        dekrip1(s, afencv);
        sprintf(fpath, "%s%s%s", dirpath, befencv, s);

        printf("%s %s %s %s\n",dirpath, befencv, s, fpath);
    }
    int res = 0;
    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;
    printf("%s\n", fpath);
    dp = opendir(fpath);
    if (dp == NULL) {
        return -errno;
    }
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (enable==1) {
            char sa[1000];
            enkrip1(sa, de->d_name);
            res = (filler(buf, sa, &st, 0));
        }
        else {
            res = (filler(buf, de->d_name, &st, 0));
        }

        if(res!=0) break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    if(strcmp(path,"/") == 0) {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else {
        char s[1000];
        int enable=0;
        char fpath[1000];
        char befencv[1000] = "", afencv[1000] = "";

        char *tok = strtok(path, "/");
        while (tok!=NULL) {
            if (enable==0) {
                strcat(befencv, "/");
                strcat(befencv, tok);
            }
            else if (enable==1) {
                strcat(afencv, "/");
                strcat(afencv, tok);
            }

            if (strncmp(tok, "encv1_", 6) == 0)
                enable=1;
            tok = strtok(NULL, "/");
        }
        
        dekrip1(s, afencv);
        sprintf(fpath, "%s%s%s", dirpath, befencv, s);
    }
    int res = 0;
    int fd = 0 ;
    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;
    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
    close(fd);
    return res;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
};  

int main(int  argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}