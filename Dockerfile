FROM gcc:latest

WORKDIR /app
COPY . .

RUN gcc -o my_shell my_shell.c

CMD ["./my_shell"]