# minifs-ecosystem

## Описание

Файловая система без вторичных i-узлов.

В одном i-узле 6 ссылок на блоки памяти.

Максимальная длина имени файла - 7 символов.

Максимальное число i-узлов и блоков - 256 на каждый.

(Все реализовано для маскимальной укомплектации в `uint8_t`)

## Запуск

Сборка userspace с помощью `cmake`.

Сборка ядра в `./core` с помощью `Makefile`.

## minifs-console

Команды:
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

Соединение с сервером работает через видоизмененный http со своими типами запросов.

Пример запуска с обращением к серверу: `./minifs-console server localhost 3000`.

## minifs-server

Обращается к символьному устройству.

Пример запуска: `./minifs-server 3000 /dev/scull`, где `/dev/scull` - путь к символьному устройству.

## minifs-kernel

Модуль ядра.

Сборка через Makefile.

!!!ВАЖНО: необходимо поменять путь к заголовочным файлам linux в случае необходимости.

Установка: `sudo insmod minifs-kernel.ko`.

Просмотр логов: `sudo dmesg`.

Добавление символьного устройства: `sudo mknod /dev/scull c 243 0`, где 243 - major, 0 - minor (смотрятся через `dmesg`).

Изменить права доступа: `sudo chmod 777 /dev/scull`.

После этого можно запускать сервер и клиент.
