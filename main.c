#pragma warning(disable: 4996) // отключаем ошибку
#include <stdio.h>
#include <windows.h> // нужна для SetConsoleCP 

#define START_DAY 1
#define START_HOUR 8
#define HOURS_IN_DAY 24
#define MAX_WORK_HOURS 1000 // ограничение, чтобы не получить переполнение int
#define INVENTORY_SIZE 10
#define EMPTY_SLOT 0
#define MAX_ITEM_ID 9

// Читает целое число. Если ввели не число - просим ввести заново.
int readInt()
{
    int num;
    while (scanf("%d", &num) != 1)
    {
        printf("Это не число! Введите ещё раз: ");
        while (getchar() != '\n'); // чистим буфер ввода
    }
    while (getchar() != '\n'); // выбрасываем хвост строки
    return num;
}

// Читает целое число и проверяет, что оно в диапазоне 
int readInRange(char* ask, int min, int max)
{
    printf("%s (от %d до %d): ", ask, min, max);
    int num = readInt();
    while (num < min || num > max)
    {
        printf("Вне диапазона! Введите от %d до %d: ", min, max);
        num = readInt();
    }
    return num;
}

void printTime(int day, int hour)
{
    printf("Текущее время: День %d, ", day);
    if (hour < 10)
    {
        printf("0"); // чтобы получилось 08:00, а не 8:00
    }
    printf("%d:00\n", hour);
}

// Печатает название предмета по его ID
void printItemName(int id)
{
    switch (id)
    {
    case 1: printf(" (Дерево)"); break;
    case 2: printf(" (Камень)"); break;
    case 3: printf(" (Семена)"); break;
    case 4: printf(" (Лейка)"); break;
    case 5: printf(" (Мотыга)"); break;
    case 6: printf(" (Топор)"); break;
    case 7: printf(" (Кирка)"); break;
    case 8: printf(" (Морковь)"); break;
    case 9: printf(" (Пшеница)"); break;
    }
}

void printInventory(int inv[], int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("Слот %d: [%d]", i, inv[i]);
        printItemName(inv[i]);
        printf("\n");
    }
}

// Вариант 3: переворачиваем массив 
void reverseInventory(int inv[], int len)
{
    for (int i = 0; i < len / 2; i++)
    {
        int temp = inv[i];
        inv[i] = inv[len - 1 - i];
        inv[len - 1 - i] = temp;
    }
}

int main()
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int current_day = START_DAY;
    int current_hour = START_HOUR;
    int inventory[INVENTORY_SIZE] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 }; 
    int action, hours, slot, item;

    do
    {
        printf("\n[1] Посмотреть на часы\n");
        printf("[2] Поработать (промотать время)\n");
        printf("[3] Посмотреть инвентарь\n");
        printf("[4] Положить предмет в слот\n");
        printf("[5] Выбросить предмет\n");
        printf("[6] Задание по варианту\n");
        printf("[0] Выход\n");
        printf("Ваш выбор: ");
        action = readInt();

        switch (action)
        {
        case 1:
            printTime(current_day, current_hour);
            break;
        case 2:
            hours = readInRange("Сколько часов работать", 0, MAX_WORK_HOURS);
            current_hour = current_hour + hours;
            current_day = current_day + current_hour / HOURS_IN_DAY; // сколько целых суток прошло
            current_hour = current_hour % HOURS_IN_DAY;              // сколько часов осталось
            printTime(current_day, current_hour);
            break;
        case 3:
            printInventory(inventory, INVENTORY_SIZE);
            break;
        case 4:
            slot = readInRange("Номер слота", 0, INVENTORY_SIZE - 1);
            item = readInRange("ID предмета", 0, MAX_ITEM_ID);
            inventory[slot] = item;
            printf("Готово! В слоте %d теперь предмет %d\n", slot, item);
            break;
        case 5:
            slot = readInRange("Номер слота", 0, INVENTORY_SIZE - 1);
            inventory[slot] = EMPTY_SLOT;
            printf("Слот %d очищен\n", slot);
            break;
        case 6:
            printf("--- До переворота ---\n");
            printInventory(inventory, INVENTORY_SIZE);
            reverseInventory(inventory, INVENTORY_SIZE);
            printf("--- После переворота ---\n");
            printInventory(inventory, INVENTORY_SIZE);
            break;
        case 0:
            printf("Пока-пока!\n");
            break;
        default:
            printf("Нет такого пункта меню!\n");
        }
    } while (action != 0);

    return 0;
}
