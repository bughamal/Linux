#三要素:  目标,依赖,命令
#----------version_1------------
#app:add.c sub.c mul.c des.c main.c
#	gcc *.c -I../include -o app

#----------version_2------------
#Makefile的执行原理
#自上向下建立依赖
#自下向上执行命令
#Makefile只能执行一个目标
#Makefile如何节省编译时间,通过比较文件更改时间,执行命令确定更新app 新于 .o 新于 .c
#app:add.o sub.o mul.o des.o main.o
#	gcc *.o -o app
#add.o:add.c
#	gcc -c add.c
#sub.o:sub.c
#	gcc -c  sub.c
#mul.o:mul.c
#	gcc -c mul.c
#des.o:des.c
#	gcc -c des.c
#main.o:main.c
#	gcc -c -I../include main.c

#功能目标:功能目标没有任何依赖,只完成对应的命令,想要执行makefile中的功能目标,必须在make之后加入目标名
#clean:
#	rm *.o -rf
#	rm app -rf
#---------version_3-------------
#Makefile内置函数
#Makefile中的特殊变量,$@(目标),$^(所有依赖),$<(依赖中的第一个)
SRC=$(wildcard *.c)
DES=$(patsubst %.c,%.o,$(SRC))

CC=gcc#c编译器
CXX=#c++编译器
CFLAGS=$(INCLUDE) -g -Wall -c#编译时参数 -g -Wall -I -c
CPPFLAGS=#预处理参数 -E -D编译时定义宏
LDFLAGS=#链接库参数 -L../lib -l库名
CXXFLAGS=#c++编译时参数 
INCLUDE=-I../include#头文件参数 -I
RM=-rm -rf#清除参数 rm ** -rf
TARGET=app#目标   app

#1.Makefile中的修饰符 @:不显示命令本身,只显示结果  -:如果当前命令出现错误,则忽略该错误,继续执行
#2.创建伪目标,如果当前路径下有与伪目标同名的文件,则忽略比对
.PHONY:clean output
$(TARGET):$(DES)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) $<

clean:
	$(RM) $(DES)
	$(RM) $(TARGET)
output:
	@echo "test output"
#安装目标,将生成的程序安装到系统默认的程序目录例如:/bin或者/usr/bin
install:
	sudo cp ./$(TARGET) /usr/bin/
#卸载目标,将刚刚安装拷贝的程序删除,删除项目的配置文件
distclean:
	sudo $(RM) /usr/bin/app
	$(RM) config.ini
