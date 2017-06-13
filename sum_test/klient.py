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

sums = ((1,1), (3,3), (4,4))

sum_dict = {}
sum_id = 0
init_linda()
for number_set in sums:
    number_sum_local = 0
    for number in number_set:
        number_sum_local += number
    print "policzona lokalnie suma: " + str(number_sum_local)
    
    tuple_to_send = '"+" "'+ str(sum_id)+ '"'
    sum_dict[sum_id] = number_sum_local
    sum_id = sum_id + 1
    for number in number_set:
        tuple_to_send += " " + str(number)
    print "tuple do wyslania: " + tuple_to_send
    output_linda(tuple_to_send)
    
x= 0 
while(x!=len(sum_dict)):
    x = x+1
    tuple_from_linda = input_linda('string:"=", integer:*, integer:*', 10)
    if(tuple_from_linda != ''):
        continue
    print "------------------------------------------\ngot:" + tuple_from_linda
    print "sum id: " + tuple_from_linda.split(" ")[1] +" sum = " + tuple_from_linda.split(" ")[2]
    
exit_linda()
        
