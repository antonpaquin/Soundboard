PID=$(cat pidfile)
BTN=$1
echo $BTN > button_dev
kill -SIGUSR1 $PID
