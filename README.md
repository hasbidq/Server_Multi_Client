# Server Multi-Client dalam C menggunakan Fork

Repositori ini berisi implementasi server multi-client sederhana menggunakan bahasa C dengan sistem call `fork`. Server ini dapat menangani beberapa klien secara bersamaan, menyediakan autentikasi dasar dan fungsi echo. Proyek ini dirancang untuk tujuan pembelajaran tentang komunikasi server-klien menggunakan socket dalam C.

## Fitur

- **Dukungan Multi-klien**: Menangani hingga banyak klien secara bersamaan.
- **Autentikasi**: Klien harus memasukkan kata sandi untuk mengakses server.
- **Layanan Echo**: Server mengembalikan pesan yang diterima dari klien.
- **Pencatatan Log**: Mencatat semua koneksi, pemutusan koneksi, dan pesan klien ke file (`server.log`).

## Persyaratan

- Kompiler GCC
- Lingkungan Linux

## Cara Menjalankan

1. **Clone repositori**

   ```bash
   git clone https://github.com/hasbidq/Server_Multi_Client.git
   cd Server_Multi_Client
   ```

2. **Kompilasi server**

   ```bash
   gcc server.c -o server
   ```

3. **Jalankan server**

   ```bash
   ./server
   ```

   Server akan mulai berjalan dan mendengarkan koneksi masuk di `127.0.0.1:4444`.

4. **Hubungkan klien**

   Gunakan alat seperti `telnet` untuk menghubungkan ke server:

   ```bash
   telnet 127.0.0.1 4444
   ```

   Ikuti instruksi di layar untuk melakukan autentikasi dan mulai mengirim pesan.

## Perilaku Server

- **Mulai**: Server mendengarkan di `127.0.0.1:4444`.
- **Koneksi klien**: Ketika klien terhubung, server membuat proses baru menggunakan `fork` untuk menangani klien tersebut.
- **Autentikasi**: Klien harus memasukkan kata sandi yang benar (`mypassword`) untuk melanjutkan.
- **Pengiriman pesan**: Server mengembalikan semua pesan yang dikirim oleh klien. Mengirimkan `:exit` akan memutuskan koneksi klien.
- **Pencatatan log**: Semua aktivitas dicatat dalam `server.log`.

## Deskripsi File

- `server.c`: Implementasi utama server.
- `server.log`: File log untuk melacak aktivitas klien (dihasilkan saat runtime).

## Catatan

- Kata sandi default adalah `mypassword`. Anda dapat mengubahnya di kode sumber.
- Pastikan `server.log` dapat ditulis oleh pengguna yang menjalankan server.
