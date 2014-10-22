source nesox.sh

readerlogfiles=nesox-reader*.log
serverlogfiles=nesox-server*.log

if [ "$1" = "" ]
then
aggregatefile="$stat/$1"
else
aggregatefile="$stat/stat.data"
fi

if [ "$2" = "" ]
then
parser=readerloop
else
parse=serverloop
fi

function parsereader()
{

if [ "$1" = "" ];
then
	echo "parsereader: log file name needed";
	exit 1;
fi;
logfilename="$1"

while read line
do
	echo $line
done < $logfilename

}

function readerloop() {
for readerlog in $readerlogfiles
do
	s=${readerlog:26:1}
	d=${readerlog:31:1}
	printf "(%s):%s->%s" $readerlog $s $d
	echo
	parsereader $readerlog
done
}

function serverloop() {
for serverlog in $serverlogfiles
do
	d=${serverlog:26:1}
	s=${serverlog:31:1}
	printf "(%s):%s->%s" $serverlog $s $d
	echo
done
}

$parser

