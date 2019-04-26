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
char cipher[]   = {"qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV\']jcp5JZ&Xl|\\8s;g<{3.u*W-0"};
int key = 17;

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
	char fpath[1000];
	char mpath[1000];
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

	while ((de = readdir(dp)) != NULL) {
		struct stat st;		
		memset(&st, 0, sizeof(st));

		// Nomor 3
		char nama[2000];
		snprintf(nama, 2000, "%s/%s", fpath, de->d_name);
		stat(nama, &st);

		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		
		struct group *grup; gid_t groupID;
		struct passwd *pwd;	uid_t userID;

		userID = st.st_uid;
		groupID = st.st_gid;
		
		pwd = getpwuid(userID);
		grup = getgrgid(groupID);

		FILE *fnew = fopen(nama, "r");
		int deniedAccess = 0;
		if(fnew == NULL)
		{
			if(errno == EACCES){
					deniedAccess = 1;
			}
		}

		if (!(strcmp(pwd->pw_name, "chipset")) 
					|| !(strcmp(pwd->pw_name, "ic_controller"))
					|| !(strcmp(grup->gr_name, "rusak"))
					|| deniedAccess)
    {
			char data[2500];
			int gID = (int) groupID;
			int uID = (int) userID;
			char strTime[1000];
			time_t za_warudo = time(NULL);
			struct tm time = *localtime(&za_warudo);
			
			sprintf(strTime, "%02d/%02d/%04d - %02d:%02d:%02d", time.tm_mday, time.tm_mon + 1, time.tm_year + 1900, time.tm_hour, time.tm_min, time.tm_sec);
			
			remove(nama);

			FILE *fo = fopen("/home/abraham/shift4/V[EOr[c[Y`HDH", "a+");
			decrypt(de->d_name);
			sprintf(data, "Filename: %s GID: %d UID: %d Last Access: %s\n", de->d_name, gID, uID, strTime);
			fprintf(fo, "%s\n", data);
			fclose(fo);
			continue;
		}

		// Nomor 1
		decrypt(de->d_name);

		/*
			Secara default akan ada hidden folder . dan ..
			yang jika di-decrypt akan menjadi c dan cc, yang akan
			muncul. Agar mereka tidak tampil, jangan masukkan
			ke dalam filler.
		*/
		
		if(strcmp(de->d_name, "c") == 0 || strcmp(de->d_name, "cc") == 0)
			continue;
		
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	char mpath[1000];
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

//static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi);
/*
{
  char fpath[1000];
	char mpath[1000];
	strcpy(mpath, path);

	// Nomor 1 
	encrypt(mpath);

	if(strcmp(mpath,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,mpath);

	int res;
	(void) fi;
	res = 
}
*/

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	// .create		= xmp_create,
};

int main(int argc, char *argv[])
{
	umask(0);
	fuse_main(argc, argv, &xmp_oper, NULL);
	
	return 0;
}