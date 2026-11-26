#pragma warning(disable: 4996) // отключаем ошибку MSVC про "небезопасный" scanf
#include <stdio.h>
#include <windows.h> // нужна для SetConsoleCP (русские буквы в консоли Windows)
#include <string.h>

#define START_DAY 1
#define START_HOUR 8
#define HOURS_IN_DAY 24
#define MAX_WORK_HOURS 1000 // ограничение, чтобы не получить переполнение int
#define INVENTORY_SIZE 10
#define EMPTY_SLOT 0
#define MAX_ITEM_ID 9 
#define MAX_NAME_LEN 64
#define DEFAULT_FARMER_NAME "Фермер"
#define MAX_ITEMS (MAX_ITEM_ID + 1)
#define LINE_LEN 128
#define NOT_FOUND -1
#define ITEMS_FILE "items.txt"

// проверяет,ввели ли число или что то другое
int readInt()
{
    int num;
    while (scanf("%d", &num) != 1)
    {
        printf("Это не число! Введите ещё раз: ");
        while (getchar() != '\n'); // чистим буфер ввода
    }
    while (getchar() != '\n'); // выбрасываем хвост строки, например "5abc"
    return num;
}

// проверяем,в диапазоне ли введеное число 
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
// вводим строчку
void readString(char* ask,char* text,int size)
{
    printf("%s", ask);
    text[0] = '\0';
    fgets(text,size,stdin);
    int len = strlen(text);
    if (len > 0 && text[len - 1] == '\n')
    {
      text[len-1] = '\0';
    }
    else if (len == size - 1)
    {
        while(getchar() != '\n');
    }
}

void fillDefaultNames(char names[][MAX_NAME_LEN])
{
    char defaults[MAX_ITEMS][MAX_NAME_LEN] = {"Пусто", "Дерево", "Камень", "Семена_пшеницы", "Лейка", "Мотыга", "Топор", "Кирка", "Морковь", "Пшеница"};
    for (int i = 0;i < MAX_ITEMS;i++)
    {
        strcpy(names[i],defaults[i]);
    }
}

int loadItemNames(char names[][MAX_NAME_LEN])
{
    FILE* file = fopen(ITEMS_FILE,"r");
    if (file == NULL)
    {
        return NOT_FOUND;
    }
    char line[LINE_LEN];
    int count = 0;
    while(count < MAX_ITEMS && fgets(line,LINE_LEN,file) != NULL)
    {
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1 ] = '\0';
        }
        strncpy(names[count], line, MAX_NAME_LEN -1);
        names[count][MAX_NAME_LEN - 1] = '\0';
        count++;
    }
    fclose(file);
    return 0;
}

// добавляет часы ,обновляет время. звездочки стоят потому что это указатели
void addHours(int* day, int* hour, int hours)
{
    *hour = *hour + hours;
    *day = *day + *hour / HOURS_IN_DAY; 
    *hour = *hour % HOURS_IN_DAY;       
}

// задание по варианту(переворачиваем массив,цикл на половину повторений из длинны массива потому что он задействует сразу 2 элемента)
void reverseInventory(int inv[], int len)
{
    for (int i = 0; i < len / 2; i++)
    {
        int temp = inv[i];
        inv[i] = inv[len - 1 - i];
        inv[len - 1 - i] = temp;
    }

}

// прост печатаем меню
void printMenu(char* farmer_name)
{
    printf("\n====== Ферма: %s =======\n",farmer_name);
    printf("[1] Посмотреть на часы\n");
    printf("[2] Поработать (промотать время)\n");
    printf("[3] Посмотреть инвентарь\n");
    printf("[4] Положить предмет в слот\n");
    printf("[5] Выбросить предмет\n");
    printf("[6] Перевернуть инвентарь\n");
    printf("[0] Выход\n");
    printf("Ваш выбор: ");
}
// печатаем время(если оно не двухзначное,добавляем 0 в начало)
void printTime(int day, int hour)
{
    printf("Текущее время: День %d, ", day);
    if (hour < 10)
    {
        printf("0"); // чтобы получилось 08:00, а не 8:00
    }
    printf("%d:00\n", hour);
}

// название предмета по его номеру
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
// выводим инвентарь на экран
void printInventory(int inv[], int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("Слот %d: [%d]", i, inv[i]);
        printItemName(inv[i]);
        printf("\n");
    }
}
// работаем н-е количество часов, используем функцию addhours
void doWork(int* day, int* hour)
{
    int hours = readInRange("Сколько часов работать", 0, MAX_WORK_HOURS);
    addHours(day, hour, hours);
    printTime(*day, *hour);
}
// ложим предмет в слот
void doPutItem(int inv[])
{
    int slot = readInRange("Номер слота", 0, INVENTORY_SIZE - 1);
    int item = readInRange("ID предмета", 0, MAX_ITEM_ID);
    inv[slot] = item;
    printf("Готово! В слоте %d теперь предмет %d\n", slot, item);
}
// выбрасываем предмет из слота
void doThrowItem(int inv[])
{
    int slot = readInRange("Номер слота", 0, INVENTORY_SIZE - 1);
    inv[slot] = EMPTY_SLOT;
    printf("Слот %d очищен\n", slot);
}
// показывает инвентарь до переворота и после 
void doReverse(int inv[])
{
    printf("--- До переворота ---\n");
    printInventory(inv, INVENTORY_SIZE);
    reverseInventory(inv, INVENTORY_SIZE);
    printf("--- После переворота ---\n");
    printInventory(inv, INVENTORY_SIZE);
}

// main (основная часть) 

int main()
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int current_day = START_DAY;
    int current_hour = START_HOUR;
    int inventory[INVENTORY_SIZE] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    int action;

    // выводим имя фермера в шапке
    char farmer_name[MAX_NAME_LEN];
    readString("Как вас зовут,фермер?",farmer_name, MAX_NAME_LEN);
    if (strlen(farmer_name) == 0)
    {
        strcpy(farmer_name,DEFAULT_FARMER_NAME);
    }

    char item_names[MAX_ITEMS][MAX_NAME_LEN];
    fillDefaultNames(item_names);
    if (loadItemNames(item_names)== NOT_FOUND)
    {
        printf("Предупреждение: файл %s не найден. Используем названия по умолчанию.\n", ITEMS_FILE);
    }

    do
    {
        printMenu(farmer_name);
        action = readInt();

        switch (action)
        {
        case 1:
            printTime(current_day, current_hour);
            break;
        case 2:
            doWork(&current_day, &current_hour);
            break;
        case 3:
            printInventory(inventory, INVENTORY_SIZE);
            break;
        case 4:
            doPutItem(inventory);
            break;
        case 5:
            doThrowItem(inventory);
            break;
        case 6:
            doReverse(inventory);
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
