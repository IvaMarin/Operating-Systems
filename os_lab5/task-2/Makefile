# -fpic -- Position Independent Code\
		   использовать относительную адресацию в переходах подпрограмм\
		   во избежание конфликтов при динамическом связывании
# -shared -- предписывает создать динамическую библиотеку\
			 именуйте динамические библиотеки следующим способом: libNAME.so
# -L. -- путь, по которму нужно искать файл библиотеки
# -ldl -- для получения адреса функции нам потребуется dlsym — это функция из библиотеки libdl\
		  которая найдет нашу функцию в стеке динамических библиотек
# -lm -- для библиотеки math
# export LD_LIBRARY_PATH=. -- добавляем текущий каталог в $LD_LIBRARY_PATH

all: libimp.so libimp_2.so  
	gcc programm-2.c -ldl -Wall -o programm-2

libimp.so: libimp.o
	gcc libimp.o -lm -shared -o libimp.so

libimp.o:
	gcc ../lib/implementation-1.c -c -Wall -Werror -fpic -o libimp.o

libimp_2.so: libimp_2.o
	gcc libimp_2.o -shared -o libimp_2.so

libimp_2.o:
	gcc ../lib/implementation-2.c -c -Wall -Werror -fpic -o libimp_2.o

clean:
	rm -rf *.so *.o programm-2