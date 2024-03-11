#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UserData { // Обновленная структура
char name[50];
char surname[50];
char patronymic[50];
int identifier;
};
union SerializedData {
struct UserData user; // Используем ту же структуру, что и в первой программе
unsigned char bytes[sizeof(struct UserData)];
};
unsigned char modifyByte(unsigned char byte) {
return byte ^ 0b01010101; // Модификация байта (для дешифрования)
}
void applyCallback(unsigned char* inputBuffer, unsigned char* outputBuffer, int size,
unsigned char (*callback)(unsigned char)) {
for (int i = 0; i < size; i++) {
outputBuffer[i] = callback(inputBuffer[i]);
}
}
void simpleDecryption(unsigned char* inputBuffer, unsigned char* outputBuffer, int size) {
applyCallback(inputBuffer, outputBuffer, size, modifyByte);
}
int main(int argc, char *argv[]) {
if (argc != 2) {
printf("Usage: %s <input_filename>\n", argv[0]);
return 1;
}
char inputFilename[100];
strcpy(inputFilename, argv[1]);
FILE *file = fopen(inputFilename, "rb");
if (file != NULL) {
fseek(file, 0, SEEK_END);
int fileSize = ftell(file);
rewind(file);
unsigned char *encryptedBytes = (unsigned char*)malloc(fileSize);
fread(encryptedBytes, sizeof(unsigned char), fileSize, file);
fclose(file);
unsigned char *decryptedBytes = (unsigned char*)malloc(sizeof(struct UserData)); //
Обновленная структура
simpleDecryption(encryptedBytes, decryptedBytes, sizeof(struct UserData)); //
Используем обновленную структуру
union SerializedData serializedData;
serializedData.user = *(struct UserData*)decryptedBytes; // Используем обновленную
структуру
printf("Last name: %s\n", serializedData.user.name);
printf("First name: %s\n", serializedData.user.surname);
printf("Middle name: %s\n", serializedData.user.patronymic);
printf("ID: %d\n", serializedData.user.identifier);
free(encryptedBytes);
free(decryptedBytes);
} else {
fprintf(stderr, "Failed to open the file for reading\n");
}
return 0;
}
