# Ring-Buffer
Ring Buffer with multithread support
Implementacion en C de ring buffer con soporte multithread.

Para compilar:
gcc main.c ringbuffer.c -lpthread (opcional -D SAFEWRITE para que el buffer NO permita sobreescritura)



La funcion que inicializa el buffer toma como argumento el tamaño del mismo. 
Este debe siempre ser una potencia de 2 (4,8,..1024..) o resultara en comportamiento indefinido.
Esto se debe a lo explicado en ref (1).

El usuario debería solo utilizar las funciones rb_init, rb_enque y rb_deque, rb_remaining y rb_destroy. 
Las demás funciones son para usos internos de la estructura.

El tipo de datos a utilizar se define en el archivo datatype.c,
en caso de querer utilizar otro tipo de datos, se debería modificar dicho archivo.
Otra opción a dicha implementación es la de almacenar los datos como (void*),
y que sea el proceso main quien se encargue de alocar la memoria a cada dato.
Con esto se ganaría generalidad, pero resulta menos explícito el comportamiento de las funciones.

Ref (1):
Las sentencias de la forma 
    inPointer++ & (buff->size-1) 
son equivalentes a:
    1- inPointer % size (para acceder al elemento deseado)
    2- inPointer + 1    (el puntero "adelanta" una posición)
Ya que x % n   ==   x & (n-1)       (si n es potencia de dos)
De esta manera se reducen las operaciones a realizar, haciendo así uso del mutex por el menor tiempo posible,
con el propósito de agilizar el proceso de escritura/lectura.
OBSERVACION IMPORTANTE:
    La igualdad antes mencionada solo es cierta para valores de n que sean potencias de dos.
    Por lo tanto, el tamaño del buffer deberá siempre ser una potencia de dos.
