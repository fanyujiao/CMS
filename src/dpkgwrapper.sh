#dpkg -l $1 | tail -n 1
result=`dpkg -s $1`
install=`echo $?`
if [ $install -ne 0 ];then
	exit $install
fi
res=`dpkg -s $1 | awk '$1=="Version:" {print $2}'`
echo $res
exit 0
#dpkg -s $1 > /dev/null && dpkg -s $1 | awk '$1=="Version:" {print $2}'
#dpkg -l imagescan
