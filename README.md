# SoalShift_modul4_B05
Kode dapat dilihat 
Suatu ketika King of YouTube ASEAN, Atta Halilintar, mengunjungi laboratorium AJK karena urusan yang benar-benar genting. Prestasi dan ketenarannya ternyata menarik perhatian hacker yang ingin mencuri video-video berkualitas milik Atta, bahkan ingin mencuri password akun YouTube miliknya. Menurut intel Atta, pemimpin hacker yang berusaha menyerangnya adalah:<br>

<img src="https://cdn.idntimes.com/content-images/post/20190204/img-20190203-235941-16c3a8a0cf0a39303830e8e107fd60d6.JPG" align="middle">  <br>
<p>Hacker LAPTOP_RUSAK</p><br>
<p>Hacker tersebut menyusup ke dalam file system yang ada di dalam PC Atta dengan mudah karena ternyata file system tersebut sama sekali tidak memiliki sistem pengamanan. Maka dari itu maksud Atta ke laboratorium AJK karena dia ingin kalian untuk mengembangkan file system yang aman.<br>
Atta menginginkan nama file system yang akan kalian buat adalah “AFSHiaAP”, yang nama kepanjangannya adalah AHHA’s File System: Hierarchical and Advanced Protection. Berikut ini kebutuhan yang harus kalian penuhi:  </p>

### Nomor 1
Semua nama file dan folder harus terenkripsi <br>
Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai: <br>

```bash
qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0
```

Misalkan ada file bernama “halo” di dalam folder “INI_FOLDER”, dan key yang dipakai adalah 17, maka:<br>
“INI_FOLDER/halo” saat belum di-mount maka akan bernama “n,nsbZ]wio/QBE#”, saat telah di-mount maka akan otomatis terdekripsi kembali menjadi “INI_FOLDER/halo” .<br>
Perhatian: Karakter ‘/’ adalah karakter ilegal dalam penamaan file atau folder dalam \*NIX, maka dari itu dapat diabaikan<br>

#### Jawaban :
Terlebih dahulu buat fungsi dekripsi dan enkripsi dengan character list dan key yang digunakan. <br>
Fungsi enkripsi : 
```bash
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
```

Fungsi dekripsi : 
```bash
void decrypt(char *sisop)
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

      there = ((there - key) + 94) % strlen(cipher);
      ch = cipher[there];
      sisop[i] = ch;
    }
  }
}
```
Fungsi enkripsi dan dekripsi akan digunakan untuk mengecek folder dan file dalam FUSE dengan file asli pada direktori asli. Dalam soal ini, kta akan menggunakannya pada fungsi ```readdir``` dan ```getattr``` untuk membaca dan mengecek file dalam direktori asli.  
Contoh :    
```bash
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
      off_t offset, struct fuse_file_info *fi)
{
	char fpath[300];
	char mpath[300];
	strcpy(mpath, path);

	// Nomor 1
	encrypt(mpath);                             // Fungsi enkripsi untuk path dari FUSE

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
.
.
.

  decrypt(de->d_name);                // Fungsi untuk mendekripsi nama file yang berada pada direktori asal
.
.
.  
  res = (filler(buf, de->d_name, &st, 0));   // Load file yang namanya telah diubah menjadi normal 
			if(res!=0) break;                      // atau sama dengan file di FUSE
.
.
.
```

### Nomor 2 
Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”. Urutan operasi dari kebutuhan ini adalah:
1. Tepat saat sebelum file system di-mount
     1. Secara otomatis folder “Videos” terbuat di root directory file system
     2. Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”. <br>Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file. File pecahan tersebut dijamin terletak di root folder fuse
     3. Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
     4. Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
     5. Jangan tampilkan file pecahan di direktori manapun
2. Tepat saat file system akan di-unmount
     1. Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
     2. Hapus folder “Videos” 

#### Jawaban :
--<br>

### Nomor 3
Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:
- Owner Name 	: ‘chipset’ atau ‘ic_controller’
- Group Name	: ‘rusak’
- Tidak dapat dibaca
Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.

#### Jawaban : 
Untuk mengecek masing - masing properti, gunakan struct group dan passwd dari library <pwd.h> dan <grp.h> untuk mendapatkan UID dan GID dari file, dan fungsi access dengan atribut R_OK untuk mengecek apakah file bisa dibaca atau tidak.<br>
Jika dari penggunaan struct dan fungsi tersebut, kriteria - kriterianya terpenuhi, maka data - data tentang file tersebut akan disimpan, owner ID (UID) dari st_uid, group ID (GID) dari st_gid, waktu terakhir akses dari ```struct tm```, ke dalam suatu variabel string dan diinput ke dalam file bernama filemiris.txt. <br>
Fungsi untuk nomor 3 : 
```bash
struct stat st;
memset(&st, 0, sizeof(st));
...

		struct group *grup; gid_t groupID;
		struct passwd *pwd;	uid_t userID;

		userID = st.st_uid;
		groupID = st.st_gid;
		
    // Untuk akses nama owner dan nama group dari
    // UID dan GID yang tersedia
		pwd = getpwuid(userID);
		grup = getgrgid(groupID);


    // Untuk cek jika sebuah file bisa di-read
    // atau tidak
    int deniedAccess = 0;
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
			
			sprintf(strTime, "%02d/%02d/%04d - %02d:%02d:%02d", time.tm_mday, time.tm_mon + 1, 
                            time.tm_year + 1900, time.tm_hour, time.tm_min, time.tm_sec);
			
			remove(nama);

			// Perubahan Demo
			char filemiris[20] = "filemiris.txt", fopenThis[300];
			encrypt(filemiris);
			sprintf(fopenThis, "/home/abraham/shift4/%s", filemiris);

			FILE *fo = fopen(fopenThis, "a+");
			decrypt(de->d_name);
			sprintf(data, "Filename: %s GID: %d UID: %d 
                             Last Access: %s\n", de->d_name, gID, uID, strTime);
			fprintf(fo, "%s\n", data);
			fclose(fo);
			continue;
		}
...
```

### Nomor 4
Pada folder **YOUTUBER**, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah **“.iz1”**. File berekstensi **“.iz1”** tidak bisa diubah permissionnya dan memunculkan error bertuliskan *“File ekstensi iz1 tidak boleh diubah permissionnya.”*  

#### Jawaban : 
-- <br>

### Nomor 5
Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama **Backup** kemudian hasil dari save tersebut akan disimpan pada backup dengan nama **namafile_[timestamp].ekstensi**. Dan ketika file asli dihapus, maka akan dibuat folder bernama **RecycleBin**, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama **namafile_deleted_[timestamp].zip** dan ditaruh ke dalam folder RecycleBin (file asli dan backup terhapus). Dengan format **[timestamp]** adalah **yyyy-MM-dd_HH:mm:ss**.    

#### Jawaban : 
-- <br>
