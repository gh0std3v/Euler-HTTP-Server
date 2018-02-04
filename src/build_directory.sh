#!/bin/bash

cd - > /dev/null

if [[ $1 == "build" ]]
then
	bash test/script1.sh

elif [[ $1 == "destroy" ]]
then
	bash test/script2.sh

fi
