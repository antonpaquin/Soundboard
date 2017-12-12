pushd ..
for x in $(seq 1 31); do
	./button.sh $x;
	sleep 0.1;
done
popd
