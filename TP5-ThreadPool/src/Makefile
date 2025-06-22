# --- Configuración ---
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -g -pthread

# --- Archivos del Proyecto ---
# Fuentes comunes a todos los tests
COMMON_SRCS = thread-pool.cc Semaphore.cc
# Convertimos los .cc a .o (archivos objeto)
COMMON_OBJS = $(COMMON_SRCS:.cc=.o)

# --- Reglas de Compilación ---

# Por defecto, compila ambos tests
all: tptest tpcustomtest main

# Regla para compilar el ejecutable 'tptest'
tptest: tptest.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o tptest $^

# Regla para compilar el ejecutable 'tpcustomtest'
tpcustomtest: tpcustomtest.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o tpcustomtest $^

# Regla para compilar el ejecutable 'main'
main: main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o main $^

# Regla genérica para compilar cualquier .cc a .o
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regla para limpiar los archivos compilados
clean:
	rm -f tptest tpcustomtest *.o

# Evita que make se confunda si existen archivos llamados 'all' o 'clean'
.PHONY: all clean
