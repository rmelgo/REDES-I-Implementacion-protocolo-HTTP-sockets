# lanzaServidor.sh
# Lanza el servidor que es un daemon y varios clientes
# las ordenes están en un fichero que se pasa como tercer parámetro
make
./servidor
#PARA NOGAL
#./cliente nogal TCP ordenes.txt &
#./cliente nogal TCP ordenes1.txt &
#./cliente nogal TCP ordenes2.txt &
#./cliente nogal UDP ordenes.txt &
#./cliente nogal UDP ordenes1.txt &
#./cliente nogal UDP ordenes2.txt &


#PARA LINUX NORMAL
./cliente localhost TCP ordenes.txt &
./cliente localhost TCP ordenes1.txt &
./cliente localhost TCP ordenes2.txt &
./cliente localhost UDP ordenes.txt &
./cliente localhost UDP ordenes1.txt &
./cliente localhost UDP ordenes2.txt &
