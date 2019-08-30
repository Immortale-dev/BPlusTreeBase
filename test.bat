make
test\test.exe

timeout /t 5

@echo "Cleaning up..."
del test\test.exe
del test\src\*.o
del src\*.o