set history filename ~/.gdb_history
set history save
file ./kernel
target remote localhost:9998
