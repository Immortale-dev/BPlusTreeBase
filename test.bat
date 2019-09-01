make
test\test.exe

@echo "Cleaning up..."
@del test\test.exe
@del test\src\*.o
@del src\*.o