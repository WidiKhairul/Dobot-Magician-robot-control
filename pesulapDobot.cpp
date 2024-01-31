#include <Arduino.h>
#include "pesulapDobot.h"

byte data_payload[23];
String dataString;

int result_bin, reminder_bin;
int result_eksponen,eksponen, reminder_eksponen;
int integer_from_frac;
float result_bin_frac;
int koma,geser_koma;
int pangkat = 8,temp;
int sign;
int x;
int tampilkan;
String checksum;
byte sum;


static char hexa[17] = "0123456789ABCDEF";
String headerLen = "AAAA13";
char hasil_hex[9];
int hasil_bin[33];


String hasil_payload;
String idCtrl = "540301";



void reverse(int x) {
  // Fungsi untuk membalikkan urutan elemen dalam array hasil_bin
  int mulai = 0;
  int stop = x;

  // Melakukan penukaran nilai antara elemen awal dan akhir array
  while (mulai < stop) {
    int temp = hasil_bin[mulai];
    hasil_bin[mulai] = hasil_bin[stop];
    hasil_bin[stop] = temp;
    mulai++;
    stop--;
  }
}

void hexStringToByteArray(String input, byte* output) {
  // Fungsi untuk mengonversi string heksadesimal menjadi array byte

  // Mendapatkan panjang string input
  int strLength = input.length();

  // Loop untuk mengambil dua karakter heksadesimal sekaligus dan mengonversinya ke bentuk byte
  for (int i = 0; i < strLength; i += 2) {
    String hexByte = input.substring(i, i + 2);
    output[i / 2] = strtol(hexByte.c_str(), NULL, 16); // Konversi ke bentuk byte
  }
}


void clearArray() {
  // Fungsi untuk mengosongkan array hasil_bin dan mengatur nilai variabel sign kembali ke 0
  
  // Loop untuk mengatur setiap elemen array hasil_bin ke nilai default (0)
  for (int i = 0; i < sizeof(hasil_bin) / sizeof(hasil_bin[0]); i++) {
    hasil_bin[i] = 0;
  }

  // Mengatur variabel sign kembali ke nilai 0
  sign = 0;
}

void payload() {
  // Fungsi untuk membentuk payload dari hasil_hex
  
  // Inisialisasi variabel untuk menentukan rentang indeks dalam hasil_hex
  int mulai = 0;
  int stop = 8;

  // Loop untuk membentuk payload dengan mengambil dua karakter heksadesimal sekaligus
  while (mulai < stop) {
    // Mengambil dua karakter heksadesimal
    String temp = String(hasil_hex[mulai]) + String(hasil_hex[mulai + 1]);

    // Menambahkan dua karakter tersebut ke awal hasil_payload
    hasil_payload = temp + hasil_payload;

    // Melangkahi dua indeks berikutnya pada hasil_hex
    mulai = mulai + 2;
  }
}

String toUpperCase(String input) {
  // Fungsi untuk mengubah semua karakter dalam string menjadi huruf kapital
  
  // Inisialisasi string hasil
  String result = "";

  // Loop untuk mengubah setiap karakter dalam string menjadi huruf kapital
  for (int i = 0; i < input.length(); i++) {
    // Menambahkan karakter yang sudah diubah menjadi huruf kapital ke dalam string hasil
    result += (char)toupper(input[i]);
  }

  return result;  // Mengembalikan string hasil yang telah diubah
}

void setHome() {
  // Fungsi untuk mengirim perintah penyetelan posisi awal (home) ke Dobot
  
  // Array yang berisi perintah penyetelan posisi awal (home)
  uint8_t setHome[11] = {0xaa, 0xaa, 0x6, 0x1f, 0x03, 0x0, 0x0, 0x0, 0x0, 0xde};

  // Loop untuk mengirim setiap byte perintah ke Dobot menggunakan Serial1
  for (int a = 0; a <= 11; a++) {
    Serial1.write(setHome[a]);
  }
}

void setPTP(float input) {
  // Fungsi untuk mengatur posisi Point to Point (PTP) pada Dobot
  
  // Memisahkan antara bilangan bulat dan pecahan
  if (input < 0) {
    input = input * (-1.0);
    sign = 1;
  }
  int whole_number = floor(input);
  float decimal_number = input - whole_number;

  // Mengubah bilangan bulat ke dalam bentuk biner
  while (whole_number != 0) {
    // Konversi bilangan bulat ke biner
    result_bin = whole_number / 2;
    reminder_bin = whole_number % 2;
    hasil_bin[x] = reminder_bin;
    whole_number = result_bin;
    x++;
  }

  koma = x;
  geser_koma = x - 1;
  eksponen = 127 + geser_koma;
  if (input == 0) {
    eksponen = 0;
  }
  x = x - 1;

  // Konversi bilangan eksponen ke biner
  for (int a; a < 8; a++) {
    result_eksponen = eksponen / 2;
    reminder_eksponen = eksponen % 2;
    hasil_bin[x] = reminder_eksponen;
    eksponen = result_eksponen;
    x++;
  }

  // Membalikkan array biner hasil konversi
  reverse(x);
  x++;

  // Konversi bilangan pecahan ke biner
  while (decimal_number <= 1 && decimal_number != 0) {
    result_bin_frac = decimal_number * 2;
    integer_from_frac = floor(result_bin_frac);
    hasil_bin[x] = integer_from_frac;
    decimal_number = result_bin_frac - integer_from_frac;
    x++;
  }

  // Jika bilangan input negatif, set bit pertama (sign bit) menjadi 1
  if (sign == 1) {
    hasil_bin[0] = 1;
  }

  x = 0;

  // Ubah bilangan biner ke dalam bentuk heksadesimal
  for (int c = 0; c <= 7; c++) {
    for (int D = 0; D <= 3; D++) {
      temp = temp + (hasil_bin[x] * pangkat);
      x++;
      pangkat = pangkat / 2;
    }
    hasil_hex[c] = hexa[temp];
    temp = 0;
    pangkat = 8;
  }

  // Mengosongkan array dan variabel lainnya
  clearArray();
  x = 0;

  // Membentuk payload
  payload();
  tampilkan++;

  // Jika sudah membentuk payload sebanyak 4 kali, kirim data ke Dobot
  if (tampilkan == 4) {
    hasil_payload = idCtrl + hasil_payload;

    int len;
    byte payload_baru[19];

    // Menghitung checksum
    hexStringToByteArray(hasil_payload, payload_baru);
    len = sizeof(payload_baru);

    for (int i = 0; i < len; i++) {
      sum += payload_baru[i];
    }
    byte comp1 = ~sum;
    byte comp2 = comp1 + 1;
    checksum = String(comp2, HEX);
    checksum = toUpperCase(checksum);
    dataString = headerLen + hasil_payload + checksum;

    // Menampilkan data yang dikirim ke Dobot ke Serial Monitor
    Serial.println("data yang dikirim ke dobot : " + dataString);

    // Mengubah dataString ke dalam bentuk array byte dan mengirimkannya ke Dobot
    hexStringToByteArray(dataString, data_payload);
    for (int i; i < 23; i++) {
      Serial1.write(data_payload[i]);
    }

    // Mengosongkan variabel tampilkan untuk mengulangi proses
    tampilkan = 0;
  }
}

void clearParams() {
  // Mengosongkan variabel dan string terkait parameter komunikasi
  dataString = "";     // Mengosongkan dataString
  hasil_payload = "";  // Mengosongkan hasil_payload
  checksum = "";       // Mengosongkan checksum
  sum = 0;             // Mengatur nilai sum kembali ke 0
}


void suctionCupOn(){
  uint8_t param1 = "isCtrlEnabled"; // Karakter param1 yang mewakili isCtrlEnabled
  uint8_t param2 = "issucked";      // Karakter param2 yang mewakili issucked

  // Mendefinisikan array byte untuk perintah menyalakan SuctionCup
  uint8_t isi[5] = {0x3e, 0x3, param1, param2};
  
  // Menghitung checksum (penjumlahan semua byte perintah)
  int Len = sizeof(isi);
  for (int i = 0; i < Len; i++) {
    sum += isi[i];
  }
  
  // Menghitung nilai complement dan complement kedua dari checksum
  byte compl1 = ~sum;
  byte compl2 = compl1 + 1;
  
   // Mendefinisikan array byte lengkap untuk perintah menyalakan SuctionCup
  uint8_t isi1[9] = {0xaa, 0xaa, 0x4, 0x3e, 0x3, param1, param2, compl2};

  // Mengirim perintah penyedot dinyalakan melalui Serial1
  for(int i; i<=8; i++){
    Serial1.write(isi1[i]);
  }

  // Menampilkan pesan ke Serial Monitor tentang data yang dikirim ke Dobot
  Serial.print("data yang dikirim ke doboot : ");
  for(int i; i<=8; i++){
    Serial.print(isi1[i],HEX);  // Menampilkan data dalam format hexadecimal
  }
  Serial.println(); // Pindah ke baris baru pada Serial Monitor
  
}

void suctionCupOff() {
  // Mendefinisikan array byte untuk perintah Mematikan Suction Cup
  uint8_t isii[8] = {0xaa, 0xaa, 0x3, 0x3e, 0x3, 0x1, 0xbe};

  // Mengirim perintah untuk mematikan Suction Cup melalui Serial1
  for (int i; i < 7; i++) {
    Serial1.write(isii[i]);
  }

  // Menampilkan pesan ke Serial Monitor tentang data yang dikirim ke Dobot
  Serial.print("data yang dikirim ke doboot : ");
  for (int i; i < 7; i++) {
    Serial.print(isii[i], HEX); // Menampilkan data dalam format hexadecimal
  }
  
  Serial.println();  // Pindah ke baris baru pada Serial Monitor
}
