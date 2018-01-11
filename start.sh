function ctrl_c() {
    kill $(jobs -p) 2>/dev/null
    exit
}

trap ctrl_c INT

BASEDIR=$(dirname "$0")
cd $BASEDIR
BASEDIR=`pwd`

cd server && echo -ne "prepare nodejs... " && npm install 1>/dev/null 2>&1 && echo "done"
cd $BASEDIR
cd ccprog && echo -ne "prepare cpp... " && make 1>/dev/null 2>&1 && echo "done"
cd $BASEDIR

node server/server.js 1>/dev/null 2>&1 &
echo -e "Web Ok , please open  http://localhost:8001"
sleep 5
echo "Simulation Start... Ctrl-C to stop" && ccprog/main 1>/dev/null 2>&1 &

while read line
do
    echo "Ctrl-C to stop"
done
