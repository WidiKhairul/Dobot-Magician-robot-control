#include "pesulapDobot.h"       // Memanggil Library PesulapDobot

static char posisi[5] = "RZYX"; // Array karakter untuk menyimpan label posisi (R, Z, Y, X)
int show;                       // Variabel untuk menunjukkan indeks posisi yang sedang diinput

void setup() {
  Serial.begin(115200);     // Menginisialisasi komunikasi serial pada baud rate 115200
  Serial1.begin(115200);    // Menginisialisasi komunikasi serial1 pada baud rate 115200
  Serial.println("ready");  // Menampilkan "ready" ke Serial Monitor
  Serial.println("masukan koordinat R,Z,Y,X"); // Menampilkan petunjuk ke Serial Monitor
  setHome();                // Memanggil fungsi setHome() untuk menetapkan posisi awal (home) robot

}

void loop() {
  
  if (Serial.available()) {
    String inputan = Serial.readString();     // Membaca input dari Serial Monitor sebagai string
    Serial.print("input " + String(posisi[show]) + ": " + inputan);   // Menampilkan input dan label posisi ke Serial Monitor
    float input = inputan.toFloat();          // Mengubah string input menjadi tipe data float
    setPTP(input);              // Memanggil fungsi setPTP() untuk mengatur posisi robot berdasarkan input
    show++;                     // Menambah indeks posisi untuk input berikutnya
    
    if (show == 4) {
      show = 0;                                   // Mengatur ulang indeks posisi setelah semua posisi telah diinput
      clearParams();                             // Memanggil fungsi clearParams() untuk menghapus variabel-variabel yang digunakan sebelumnya
      suctionCupOn();                            // Memanggil fungsi suctionCupOn() untuk menyalakan penyedot
      Serial.println("SuctionCup ON");           // Menampilkan pesan ke Serial Monitor
      delay(7000);                               // Menunda eksekusi program selama 7 detik (7000 milidetik)
      suctionCupOff();                           // Memanggil fungsi suctionCupOff() untuk mematikan penyedot
      Serial.println("SuctionCup Off");          // Menampilkan pesan ke Serial Monitor
      clearParams();                             // Memanggil fungsi clearParams() untuk menghapus variabel-variabel yang digunakan sebelumnya
      Serial.println("masukan koordinat R,Z,Y,X"); // Menampilkan petunjuk ke Serial Monitor
    } 
  }
}
