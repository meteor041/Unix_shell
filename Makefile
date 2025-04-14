target:=my_shell
all:${target}.c
	gcc ${target}.c -o ${target}.o

run:
	./${target}.o

clean:
	rm ./${target}.o
