# -fpic -- Position Independent Code\
		   использовать относительную адресацию в переходах подпрограмм\
		   во избежание конфликтов при динамическом связывании
# -shared -- предписывает создать динамическую библиотеку\
			 именуйте динамические библиотеки следующим способом: libNAME.so
# -L. -- путь, по которму нужно искать файл библиотеки
# -lNAME_OF_LIBRARY -- линковка
# -lm -- для библиотеки math
# export LD_LIBRARY_PATH=. -- добавляем текущий каталог в $LD_LIBRARY_PATH

all: libimp.so 
	gcc programm-1.c -L. -limp -Wall -o programm-1

libimp.so: libimp.o 
	gcc libimp.o -lm -shared -o libimp.so

libimp.o:
	gcc ../lib/implementation-1.c -c -Wall -Werror -fpic -o libimp.o

clean:
	rm -rf *.so *.o programm-1