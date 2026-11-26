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
#define DIARY_FILE "diary.txt"
#define MAX_NPC 20     
#define NPC_FIELDS 4   
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"

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

int findName(char names[][MAX_NAME_LEN],int count,char* name)
{
    for(int i = 0;i < count;i++)
    {
        if (strcmp(names[i],name) == 0)
        {
            return i;
        }
    }
    return NOT_FOUND;
}

// задание по варианту
int buildSchedule(FILE* file, int hour, char names[][MAX_NAME_LEN], char places[][MAX_NAME_LEN], int busy[])
{
    char line[LINE_LEN], name[MAX_NAME_LEN], place[MAX_NAME_LEN];
    int start, end;
    int count = 0;
    while (count < MAX_NPC && fgets(line, LINE_LEN, file) != NULL)
    {
        // 63 в формате = MAX_NAME_LEN - 1 (защита от выхода за границы массива)
        if (sscanf(line, "%63[^;];%d;%d;%63[^\n]", name, &start, &end, place) == NPC_FIELDS)
        {
            int index = findName(names, count, name);
            if (index == NOT_FOUND)
            {
                index = count; // новый житель - добавляем в конец списка
                strcpy(names[index], name);
                busy[index] = 0;
                count++;
            }
            if (hour >= start && hour < end)
            {
                strcpy(places[index], place);
                busy[index] = 1;
            }
        }
    }
    return count;
}

void doSearchItem(int inv[], char names[][MAX_NAME_LEN])
{
    char name[MAX_NAME_LEN];
    readString("Введите название предмета: ",name,MAX_NAME_LEN);
    int id = findName(names,MAX_ITEMS,name);
    if (id == NOT_FOUND)
    {
        printf("Предмета \"%s\" нет в каталоге.\n",name);
        return;
    }
    int found = 0;
    for (int i = 0;i < INVENTORY_SIZE;i++)
    {
        if (inv[i] == id)
        {
            printf("Предмет\"%s\" лежит в слоте %d\n",name,i);
            found++;
        }
    }
    if (found == 0)
    {
        printf("Предмета \"%s\" нет в рюкзаке.\n",name);
    }    
    
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
    printf("[7] Найти предмет в рюкзаке по названию\n");
    printf("[8] Записать состояние в дневник фермера\n");
    printf("[9] Расписание жителей деревни\n");
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

// выводим инвентарь на экран
void printInventory(int inv[], int len,char names[][MAX_NAME_LEN])
{
    for (int i = 0; i < len; i++)
    {
        printf("Слот %d: [%d] - %s\n", i, inv[i],names[inv[i]]);
    }
}

void printDiary(FILE* out,char* farmer_name,int day,int hour,int inv[],char names[][MAX_NAME_LEN])
{
    fprintf(out, "Фермер: %s, День %d, %02d:00\n",farmer_name,day,hour);
    for (int i = 0;i < INVENTORY_SIZE; i++)
    {
        fprintf(out, " Слот %d: %s\n", i, names[inv[i]]);
    }
}

void doDiary(char* farmer_name,int day,int hour,int inv[],char names[][MAX_NAME_LEN])
{
    printDiary(stdout,farmer_name,day,hour,inv,names);
    FILE* diary = fopen(DIARY_FILE,"a");
    if(diary == NULL)
    {
        printf("Ошибка: не удалось открыть файл %s!\n",DIARY_FILE);
        return;
    }
    printDiary(diary,farmer_name,day,hour,inv,names);
    fclose(diary);
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
void doReverse(int inv[], char names[][MAX_NAME_LEN])
{
    printf("--- До переворота ---\n");
    printInventory(inv, INVENTORY_SIZE,names);
    reverseInventory(inv, INVENTORY_SIZE);
    printf("--- После переворота ---\n");
    printInventory(inv, INVENTORY_SIZE,names);
}
// задание по варианту
void printSchedule(FILE* out, int hour, char names[][MAX_NAME_LEN], char places[][MAX_NAME_LEN], int busy[], int count)
{
    fprintf(out, "Жители деревни в %02d:00:\n", hour);
    for (int i = 0; i < count; i++)
    {
        if (busy[i] == 1)
        {
            fprintf(out, "  %s - %s\n", names[i], places[i]);
        }
        else
        {
            fprintf(out, "  %s - [Спит дома]\n", names[i]);
        }
    }
}
// задание по варианту
void doSchedule(int hour)
{
    FILE* input = fopen(INPUT_FILE, "r");
    if (input == NULL)
    {
        printf("Ошибка: не удалось открыть файл %s!\n", INPUT_FILE);
        return;
    }
    char npc_names[MAX_NPC][MAX_NAME_LEN], npc_places[MAX_NPC][MAX_NAME_LEN];
    int npc_busy[MAX_NPC];
    int count = buildSchedule(input, hour, npc_names, npc_places, npc_busy);
    fclose(input);

    printSchedule(stdout, hour, npc_names, npc_places, npc_busy, count);
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
            printInventory(inventory, INVENTORY_SIZE,item_names);
            break;
        case 4:
            doPutItem(inventory);
            break;
        case 5:
            doThrowItem(inventory);
            break;
        case 6:
            doReverse(inventory,item_names);
            break;
        case 7:
            doSearchItem(inventory,item_names);
            break;
        case 8:
            doDiary(farmer_name,current_day,current_hour,inventory,item_names);
            break;
        case 9:
            doSchedule(current_hour);
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
