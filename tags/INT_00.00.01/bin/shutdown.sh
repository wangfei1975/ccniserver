
#

echo shuting down ccni server.
ps -A|grep ccniserver| awk '{if(FNR==1)system("kill "$1""); print $1, $2, $3}'
ps aux|grep ccni|awk '{system("kill "$2""); print $0}'  

#ps -A|grep ccniserver


#ps -A|grep apn| awk '{if(FNR==1)system("kill "$1""); print $1, $2, $3}'
#ps -A|grep apn

#rm -f /tmp/*.pid
ipcs -q|grep $USER |awk '{system("ipcrm -q "$2""); print "ipcrm -q " $2}'
echo done.
ps -A|grep ccniserver

ipcs -q|grep $USER



