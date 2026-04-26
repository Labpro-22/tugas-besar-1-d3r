# Nimonspoli
## Tugas Besar 1 IF2010 Pemrograman Berorientasi Objek

Oleh D3R - Completing a Tubes that doesn't exist:
- 13524003 - Faiq Azzam Nafidz
- 13524013 - Anindya Naufal Pinasthika
- 13524048 - Josh Reinhart Zidik
- 13524050 - Raysha Erviandika Putra
- 13524084 - Dzaki Ahmad Alhusainy

## Deskripsi
Nimonspoli adalah gim dalam bahasa C++ yang memanfaatkan konsep-konsep pemrograman berbasis objek (_object oriented programming_). Gim ini merupakan gim strategi dengan pemain di dalamnya dapat membeli properti, membayar sewa, mengikuti lelang, hingga mengelola kekayaan sambil berkeliling papan permainan.

## Fitur Utama
- Sistem papan permainan Monopoly dengan properti, pajak, utilitas, stasiun, dan tile spesial.
- Mekanisme jual beli properti, bangun rumah/hotel, gadai, dan tebus.
- Kartu kemampuan yang dapat digunakan pemain pada momen tertentu.
- Dukungan penyimpanan permainan melalui command `SIMPAN`.
- Visualisasi permainan berbasis GUI (raylib) dengan konsol command interaktif.

## Cara Menjalankan
### Prasyarat
- CMake versi 3.20 atau lebih baru.
- Compiler C++ dengan dukungan C++17:
	- Linux: `g++` / `clang++`
	- Windows: MSVC (Visual Studio 2022) atau MinGW
	- macOS: AppleClang (Xcode Command Line Tools)
- Library `raylib`.
- (Linux) `pkg-config` untuk membantu pencarian `raylib`.

Instalasi:

Linux (Ubuntu/Debian):
```bash
sudo apt-get update
sudo apt-get install cmake build-essential libraylib-dev pkg-config
```

macOS (Homebrew):
```bash
brew install cmake raylib
```

Windows (vcpkg):
```powershell
vcpkg install raylib:x64-windows
```

### _Compile_ dan _Run_
Dari root project:

```bash
cmake -S . -B build
cmake --build build
```
(Jika tidak memiliki hal yang ada di prasyarat, bisa langsung jalankan _executable_-nya saja)

Lalu jalankan _executable_:

Linux/macOS:
```bash
./bin/MonopolyGame
```

Windows:
```powershell
.\bin\MonopolyGame
```

### Menjalankan via VS Code Task
Workspace ini sudah menyediakan task berikut:
- `configure` untuk generate build system CMake.
- `build` untuk kompilasi project.
- `clean` untuk membersihkan artefak build.

## Cara Memainkan
Saat game dimulai, pemain akan diminta memilih:
1. `New Game` untuk memulai permainan baru.
2. `Load Game` untuk memuat permainan tersimpan.

Untuk `New Game`, masukkan jumlah pemain (2-4) dan username masing-masing pemain.

Setelah inisialisasi selesai, permainan berjalan berbasis giliran. Input command pada konsol game sesuai giliran pemain aktif.

### Command Utama
- `BANTUAN`: menampilkan daftar command yang tersedia.
- `CETAK_PAPAN`: menampilkan papan permainan.
- `STATUS`: menampilkan status pemain aktif (uang, posisi, efek).
- `LEMPAR_DADU`: melempar dadu untuk bergerak.
- `ATUR_DADU X Y`: mengatur nilai dadu manual (untuk pengujian/debug).
- `CETAK_PROPERTI`: menampilkan properti milik pemain aktif.
- `BANGUN`: membangun rumah/hotel pada properti yang memenuhi syarat.
- `GADAI`: menggadaikan properti.
- `TEBUS`: menebus properti yang sedang digadaikan.
- `CETAK_KARTU`: menampilkan kartu kemampuan pemain.
- `GUNAKAN_KEMAMPUAN`: menggunakan kartu kemampuan.
- `CETAK_LOG [N]`: menampilkan log aktivitas permainan (opsional N baris).
- `CETAK_AKTA`: menampilkan detail akta suatu petak berdasarkan kode.
- `SIMPAN <nama_file>.txt`: menyimpan state permainan.
- `KELUAR`: keluar dari permainan.

### Catatan
- File konfigurasi board dan properti berada di folder `config/`.
- File status gim yang disimpan berada di folder `data/`.

## Struktur Direktori
- `src/`: implementasi source code.
- `include/`: header file.
- `config/`: data konfigurasi tile/properti/peraturan.
- `data/`: data pendukung dan aset.
- `build/`: output dan cache CMake.
- `bin/`: executable hasil build.
- `doc/`: laporan pengerjaan tugas besar