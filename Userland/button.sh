PID=$(cat pidfile)
DEVF=$1
BTN=$2
echo $BTN > $DEVF
kill -SIGUSR1 $PID
