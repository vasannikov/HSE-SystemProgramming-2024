#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
struct UserData {
char name[50];
char surname[50];
char patronymic[50];
int identifier;
};
union SerializedData {
struct UserData user;
unsigned char bytes[sizeof(struct UserData)];
};
unsigned char modifyByte(unsigned char byte) {
return byte ^ 0b01010101;
}
void applyCallback(unsigned char* inputBuffer, unsigned char* outputBuffer, int size,
unsigned char (*callback)(unsigned char)) {
for (int i = 0; i < size; i++) {
outputBuffer[i] = callback(inputBuffer[i]);
}
}
void simpleEncryption(unsigned char* inputBuffer, unsigned char* outputBuffer, int size) {
applyCallback(inputBuffer, outputBuffer, size, modifyByte);
}
int isAlphaWithSpace(const char *str) {
while (*str) {
if (!isalpha(*str) && !isspace(*str)) {
return 0; // Не только буквы и пробелы
}
str++;
}
return 1; // Все символы - буквы и пробелы
}
int main(int argc, char *argv[]) {
if (argc != 2) {
printf("Usage: %s <output_filename>\n", argv[0]);
return 1;
}
char outputFilename[100];
strcpy(outputFilename, argv[1]);
struct UserData userData;
char input[150]; // Буфер для ввода пользователя
printf("Maximum 50 characters for first name, last name, and middle name.\n");
printf("Enter last name: ");
fgets(input, sizeof(input), stdin);
input[strcspn(input, "\n")] = 0; // Удаляем символ новой строки
if (!isAlphaWithSpace(input)) {
printf("Error: Invalid input for last name!\n");
return 1;
}
strncpy(userData.name, input, sizeof(userData.name));
printf("Enter first name: ");
fgets(input, sizeof(input), stdin);
input[strcspn(input, "\n")] = 0; // Удаляем символ новой строки
if (!isAlphaWithSpace(input)) {
printf("Error: Invalid input for first name!\n");
return 1;
}
strncpy(userData.surname, input, sizeof(userData.surname));
printf("Enter middle name: ");
fgets(input, sizeof(input), stdin);
input[strcspn(input, "\n")] = 0; // Удаляем символ новой строки
if (!isAlphaWithSpace(input)) {
printf("Error: Invalid input for middle name!\n");
return 1;
}
strncpy(userData.patronymic, input, sizeof(userData.patronymic));
printf("Maximum 10 characters for ID.\n");
char idInput[20];
int validInput = 0;
while (!validInput) {
printf("Enter ID: ");
fgets(idInput, sizeof(idInput), stdin);
idInput[strcspn(idInput, "\n")] = 0; // Удаление символа новой строки
int i;
for (i = 0; idInput[i] != '\0'; i++) {
if (!isdigit(idInput[i])) {
printf("Error: ID must be a number!\n");
break;
}
}
if (idInput[i] == '\0') {
validInput = 1;
userData.identifier = atoi(idInput);
}
}
union SerializedData serializedData;
serializedData.user = userData;
unsigned char *encryptedBytes = (unsigned char*)malloc(sizeof(struct UserData));
simpleEncryption(serializedData.bytes, encryptedBytes, sizeof(struct UserData));
FILE *file = fopen(outputFilename, "wb");
if (file != NULL) {
fwrite(encryptedBytes, sizeof(unsigned char), sizeof(struct UserData), file);
fclose(file);
} else {
fprintf(stderr, "Error! File does not exist!\n");
return 1;
}
free(encryptedBytes);
return 0;
}
