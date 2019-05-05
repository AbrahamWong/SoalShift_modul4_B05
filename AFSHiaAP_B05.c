#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

// Nomor 3
#include <pwd.h>
#include <grp.h>
#include <time.h>

// gcc -Wall `pkg-config fuse --cflags` AFSHiaAP_B05.c -o something `pkg-config fuse --libs`
// mkdir AFSHiaAP
// ./something [namafile] -d

static const char *dirpath = "/home/abraham/shift4"; 
char cipher[]   = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV\']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
int key = 31;

// Nomor 1
void encrypt(char *sisop)
{
	char ch;
	for(int i = 0; i < strlen(sisop); i++)
	{
			int there = 0;
			ch = sisop[i];
			
			if(ch == '/')
					sisop[i] = '/';

			else
			{
					for(int j = 0; j < strlen(cipher); j++)
					{
							if(cipher[j] == ch)
									there = j;
					}

					there = (there + key) % strlen(cipher);
					ch = cipher[there];
					sisop[i] = ch;
			}
	}
}

// Nomor 1
void decrypt(char *sisop)
{		char ch;
		for(int i = 0; i < strlen(sisop); i++)
    {
        int there = 0;
        ch = sisop[i];
				
        if(ch == '/')
            sisop[i] = '/';

        else
        {
            for(int j = 0; j < strlen(cipher); j++)
            {
                if(cipher[j] == ch)
                    there = j;
            }

            there = ((there - key) + 94) % strlen(cipher);
            ch = cipher[there];
            sisop[i] = ch;
        }
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	char mpath[1000];
	strcpy(mpath, path);

	// Nomor 1
	encrypt(mpath);
	sprintf(fpath,"%s%s",dirpath,mpath);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char fpath[300];
	char mpath[300];
	strcpy(mpath, path);

	// Nomor 1
	encrypt(mpath);

	if(strcmp(mpath,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,mpath);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	// Debugging
	int count = 0;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;		
		memset(&st, 0, sizeof(st));

		// Nomor 3
		char nama[300];
		sprintf(nama, "%s/%s", fpath, de->d_name);
		int panjang = strlen(nama);
		nama[panjang] = '\0';
		stat(nama, &st);
		printf("%s\n", nama);

		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		
		struct group *grup; gid_t groupID;
		struct passwd *pwd;	uid_t userID;

		userID = st.st_uid;
		groupID = st.st_gid;
		
		pwd = getpwuid(userID);
		grup = getgrgid(groupID);

		// Gak gini caranya ngecek readnya
		// FILE *fnew = fopen(nama, "r");
		int deniedAccess = 0;
		// if(fnew == NULL)
		// {
		// 	if(errno == EACCES){
		// 			deniedAccess = 1;
		// 	}
		// 	else if(errno = ENOENT)
		// 			printf("Why???\n\n");
		// 			// continue;
		// }

		int akses = access(nama, R_OK);
		if (akses)
		{
			if (errno = EACCES)
				deniedAccess = 1;
		}

		if ( (!(strcmp(pwd->pw_name, "chipset")) || !(strcmp(pwd->pw_name, "ic_controller")) )
					&& !(strcmp(grup->gr_name, "rusak"))
					&& deniedAccess)
    {
			char data[500];
			int gID = (int) groupID;
			int uID = (int) userID;
			char strTime[100];
			time_t za_warudo = time(NULL);
			struct tm time = *localtime(&za_warudo);
			
			sprintf(strTime, "%02d/%02d/%04d - %02d:%02d:%02d", time.tm_mday, time.tm_mon + 1, time.tm_year + 1900, time.tm_hour, time.tm_min, time.tm_sec);
			
			remove(nama);

			// Perubahan Demo
			char filemiris[20] = "filemiris.txt", fopenThis[300];
			encrypt(filemiris);
			sprintf(fopenThis, "/home/abraham/shift4/%s", filemiris);

			FILE *fo = fopen(fopenThis, "a+");
			decrypt(de->d_name);
			sprintf(data, "Filename: %s GID: %d UID: %d Last Access: %s\n", de->d_name, gID, uID, strTime);
			fprintf(fo, "%s\n", data);
			fclose(fo);
			continue;
		}

		// Nomor 1
		decrypt(de->d_name);
		printf("%s --> ", de->d_name);

		// Perubahan demo
		char root[300];
		strcpy(root, de->d_name);
		encrypt(root);

		/*
			Secara default akan ada hidden folder . dan ..
			yang jika di-decrypt akan menjadi c dan cc, yang akan
			muncul. Agar mereka tidak tampil, jangan masukkan
			ke dalam filler.
		*/
		
		if(strcmp(root, ".") == 0 || strcmp(root, "..") == 0)
			continue;
		
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
		
		count++;
		printf("%d\n", count);
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[300];
	char mpath[300];
	strcpy(mpath, path);

	// Nomor 1 
	encrypt(mpath);

	if(strcmp(mpath,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,mpath);
	
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

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[300];
	char mpath[300];
	strcpy(mpath, path);

	encrypt(mpath);
	if(strcmp(mpath, "/") == 0)
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else sprintf(fpath, "%s%s", dirpath, mpath);

	res = mkdir(fpath, mode);
	if(res)
		return errno;
	
	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[300];
	char mpath[300];
	strcpy(mpath, path);

	encrypt(mpath);
	if(strcmp(mpath, "/") == 0)
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else sprintf(fpath, "%s%s", dirpath, mpath);

	res = rmdir(fpath);
	if(res)
		return errno;
	
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[300];
	char mpath[300];
	strcpy(mpath, path);

	encrypt(mpath);
	if(strcmp(mpath, "/") == 0)
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else sprintf(fpath, "%s%s", dirpath, mpath);

	res = chmod(fpath, mode);
	if(res)
		return errno;
	
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char fpath[300], mpath[300];
	strcpy(mpath, path);

	encrypt(mpath);
	if(strcmp(mpath, "/") == 0)
	{
		path = dirpath;
		sprintf(fpath, "%s", path);
	}
	else sprintf(fpath, "%s%s", dirpath, mpath);

	res = lchown(fpath, uid, gid);
	if(res)
		return errno;
	
	return 0;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi) 
{
  int res; // , a;
	char fpath[300], mpath[300]; // , newpath[400];
	strcpy(mpath, path);
	
	encrypt(mpath);
	if(strcmp(mpath,"/") == 0) 
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s", dirpath, mpath);

	// Nope
	// if(strstr(path, "YOUTUBER"))
	// {
	// 	for(a = 0; a < strlen(path); a++)
	// 	{
	// 		if(a + 8 < strlen(path))
	// 		{
	// 			if(path[a] == 'Y' && path[a + 1] == 'O' && path[a + 2] == 'U' &&
	// 			 path[a + 3] == 'T' && path[a + 4] == 'U' && path[a + 5] == 'B' &&
	// 			 path[a + 6] == 'E' && path [a + 7] == 'R' && path [a + 8] == '/')
	// 			 {
	// 				 	res = creat(fpath, 640);
	// 					sprintf(newpath, "%soD)4", fpath);
	// 					char *argv[] = {"mv", fpath, newpath, NULL};
	// 					execv("/bin/mv", argv);
	// 			 }
	// 			 else res = creat(fpath, mode);
	// 		}
	// 	}
	// }
	// else res = creat(fpath, mode);

	res = creat(fpath, mode);

	if (res == -1)
			return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.mkdir = xmp_mkdir,
	.rmdir = xmp_rmdir,
	.chmod = xmp_chmod,
	.chown = xmp_chown,
	.create		= xmp_create,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}