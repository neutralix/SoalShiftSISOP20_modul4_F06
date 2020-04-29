# SoalShiftSISOP20_modul3_F06
## Enkripsi Versi 1
Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1. Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file. Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key :
``9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO``
Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10
“encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg
Note : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.
#### fungsi enkrip1
~~~
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
~~~
Penjelasan:
- Fungsi menerima pointer string yang akan diencrypt dan pointer untuk mengembalikan hasil encrypt.
- Terdapat loop di dalam fungsi yang menelusuri string 'name' yang dikirimkan.
- Masing-masing karakter dicari posisinya di dalam key. Setelah ditemukan, key tersebut ditambah dengan 10 lalu di modulo agar kembali memutar '(key + 10) % 87' untuk diencrypt dan hasilnya dimasukkan ke dalam variable string hasil 'res'
- Fungsi selesai ketika seluruh string 'name' telah ditelusuri
#### fungsi dekrip1
~~~
void dekrip1(char* res, char* name) {
...
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
...
~~~
Penjelasan:
- Fungsi menerima pointer string yang akan didecrypt dan pointer untuk mengembalikan hasil decrypt.
- Terdapat loop di dalam fungsi yang menelusuri string 'name' yang dikirimkan.
- Masing-masing karakter dicari posisinya di dalam key. Setelah ditemukan, key tersebut ditambah 87 dikurang dengan 10 lalu di modulo agar kembali memutar '(key + 87 - 10) % 87' untuk didecrypt dan hasilnya dimasukkan ke dalam variable string hasil 'res'
- Fungsi selesai ketika seluruh string 'name' telah ditelusuri

Untuk melakukan encrypt pada filesystem buatan, harus diubah fungsi xmp_getattr, xmp_readdir, dan xmp_read
#### xmp_getattr
~~~
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
~~~
Penjelasan :
- Fungsi tersebut pertama akan memisahkan string yang akan didecrypt dan tidak akan didecrypt menggunakan variabel 'befencv' dan 'afencv'
- Variable 'afencv' akan diperlakukan fungsi decrypt
- Hasil string tersebut digabungkan agar menjadi path untuk membaca directory sesungguhnya. 'sprintf(fpath, "%s%s%s", dirpath, befencv, s);'
#### xmp_readdir
~~~
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    ...
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
~~~
Penjelasan :
- Fungsi tersebut pertama akan memisahkan string yang akan didecrypt dan tidak akan didecrypt menggunakan variabel 'befencv' dan 'afencv' (sama seperti xmp_getattr)
- Variable 'afencv' akan diperlakukan fungsi decrypt
- Hasil string tersebut digabungkan agar menjadi path untuk membaca directory sesungguhnya. 'sprintf(fpath, "%s%s%s", dirpath, befencv, s);'
- Apabila directory tersebut merupakan directory yang perlu diencrypt, maka akan dilakukan fungsi encrypt sebelum filler. 'enkrip1(sa, de->d_name); res = (filler(buf, sa, &st, 0));'
#### xmp_readdir
~~~

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    ...
    else {
        char s[1000];
        int enable=0;
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
    ...
}
~~~
Penjelasan :
- Fungsi tersebut pertama akan memisahkan string yang akan didecrypt dan tidak akan didecrypt menggunakan variabel 'befencv' dan 'afencv' (sama seperti xmp_getattr)
- Variable 'afencv' akan diperlakukan fungsi decrypt
- Hasil string tersebut digabungkan agar menjadi path untuk membaca directory sesungguhnya. 'sprintf(fpath, "%s%s%s", dirpath, befencv, s);'

Untuk membuat log folder baru yang diencrypt, mengubah fungsi xmp_mkdir dan xmp_rename
#### xmp_readdir
~~~
static int xmp_mkdir(const char *path, mode_t mode)
{
    ...
    char pathcopy[1000];
    strcpy(pathcopy, path);
    char *tok = strtok(pathcopy, "/");
    char log2[1000];
    while (tok!=NULL) {
        if (strncmp(tok, "encv1_", 6) == 0) {
            sprintf(log2, "ENCRYPT::%s\n", path);
            get_log(log2);
            break;
        }
        tok = strtok(NULL, "/");
    }
	return 0;
}
~~~
Penjelasan : 
- Nama directory yang baru dibuat akan dicek terlebih dahulu apakah berawalan string encv1_
- Akan menulis log encrypt suatu folder apabila folder tersebut berawalan encv1
#### xmp_rename
~~~
static int xmp_rename(const char *from, const char *to)
{
    ...
    char pathcopy[1000];
    strcpy(pathcopy, to);
    char *tok = strtok(pathcopy, "/");
    char log2[1000];
    while (tok!=NULL) {
        if (strncmp(tok, "encv1_", 6) == 0) {
            sprintf(log2, "ENCRYPT::%s\n", to);
            get_log(log2);
            break;
        }
        tok = strtok(NULL, "/");
    }
	return 0;
}
~~~
Penjelasan : 
- Nama directory yang baru dibuat akan dicek terlebih dahulu apakah berawalan string encv1_
- Akan menulis log encrypt suatu folder apabila folder tersebut berawalan encv1
