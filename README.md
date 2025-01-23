# Сборка
```
git clone https://gitlab.akhcheck.ru/lenar.khabibullin/battleship.git
cd battleship
cmake -B build
make -C build
```
# Запуск
Есть два варианта запуска:

 1. "Создать" игру и ждать подключения по второму способу на заданном порте
```
./bin/game_client host -p <port>
```
2. Подключиться к игре запущенной первым способом
```
./bin/game_client guest -a <IPv4 address> -p <port>
```

