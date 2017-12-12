#! /bin/bash

if [[ "$1" == "clean" ]]; then
	rm pidfile
	rm button_dev
	rmdir ./media
else
	touch pidfile
	mkdir -p ./media
	touch button_dev
fi
