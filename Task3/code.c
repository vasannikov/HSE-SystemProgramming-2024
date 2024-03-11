#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
extern void f(int32_t shift, int32_t len, int32_t* mass);
int main() {
SetConsoleOutputCP(1251);
int32_t n;
int32_t shift;
printf("Введите размер массива\n");
#pragma warning(suppress : 4996)
scanf("%d", &n);
if (n <= 0) {
printf("длина меньше 0");
return 1;
}
printf("Введите количество поцизий для циклического сдвига\n");
#pragma warning(suppress : 4996)
scanf("%d", &shift);
int* mass = (int32_t*)malloc(n * sizeof(int32_t));
for (int i = 0; i < n; i++) {
mass[i] = rand() % 1000;
}
printf("Сгенерированный массив\n");
for (int i = 0; i < n; i++) {
printf("%d ", mass[i]);
}
f(shift, n, mass);
printf("\n");
printf("Итоговый массив\n");
for (int i = 0; i < n; i++) {
printf("%d ", mass[i]);
}
free(mass);
}
