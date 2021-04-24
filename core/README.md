# miniFS

## Описание

Файловая система без вторичных i-узлов.

В одном i-узле 6 ссылок на блоки памяти.

Максимальная длина имени файла - 7 символов.

Максимальное число i-узлов и блоков - 256 на каждый.

(Все реализовано для маскимальной укомплектации в `uint8_t`)

## Запуск

Сборка с помощью `cmake`.

Можно запустить `build.sh`. Исполняемый файл будет в `./bin`.

В процессе запуска в запускаепой директории будет создан файл `filesystem`, если
он не существует.

## Команды

```
file create <full_filename>
file get <full_filename>
file write <full_filename> <data>
file delete <full_filename>
file copy <full_source> <full_destination>

dir create <full_dirname>
dir delete <full_dirname>
dir get <full_dirname>
```
