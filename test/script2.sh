#!/bin/bash


# This file is meant for deleting the "server_test_files"
# directory created by script1.sh. 


cd - > /dev/null

echo "Deleting directory server_test_files..."

if [[ -d "server_test_files" ]]
then
	rm -r server_test_files
	echo "Deleted directory."
else
	echo "No directory to delete."
fi
