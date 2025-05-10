FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Instalar las herramientas necesarias
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    time \
    valgrind \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Establecer el directorio de trabajo
WORKDIR /workspace

# Copiar todos los archivos del proyecto al contenedor
COPY . .

# Compilar todos los archivos .cpp en el directorio src
RUN g++ -o myapp src/*.cpp

# Comando para ejecutar el programal programa
CMD ["./myapp"]