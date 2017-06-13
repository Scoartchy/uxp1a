from ctypes import cdll
from ctypes import c_char_p

linda_lib = cdll.LoadLibrary("linda.so")

input_linda = linda_lib.input_linda #??
input_linda.restype = c_char_p

read_linda = linda_lib.read_linda #??
read_linda.restype = c_char_p

output_linda = linda_lib.output_linda #??
output_linda.restype = c_char_p

init_linda = linda_lib.init_linda
exit_linda = linda_lib.exit_linda

init_linda()
while(True):
    tuple_from_linda = input_linda('string:"+", integer:*, integer:*, integer:*', 10)
    if(tuple_from_linda == ''):
        continue
    suma = 0
    print "got: " + tuple_from_linda
    print tuple_from_linda.split(" ")[2:-1]
    tuple_to_send = '"=" '+ str(tuple_from_linda.split(" ")[1])+' '
    for number in tuple_from_linda.split(" ")[2:-1]:
        suma = suma + int(number)
    tuple_to_send += str(suma)
    print "SUMA = " + str(suma) 
    print "tuple to send back: " + tuple_to_send
    output_linda(tuple_to_send)
        
exit_linda()
