#!/bin/bash

# This script's purpose is to create a root directory
# that can be utilised for testing the server. You should
# delete this directory by using script2.sh later on.


echo "Creating a root directory..."

cd - > /dev/null
mkdir -p server_test_files/r00t

echo "Root directory created."
echo "Creating test files..."

touch server_test_files/r00t/index.html
touch server_test_files/r00t/random.txt

echo "Created testing files."

text=$(cat <<EOF 


<!DOCTYPE html> 
<html> 
	<head> 
		<meta charset='utf-8'></meta> 
		<title>Euler Server Test Page</title> 
	</head> 
	 
	<body> 
		<div align='center'> 
			<h1>Euler Server</h1> 
			<p>This page is for testing.</p> 
		</div> 
	</body> 
</html> 

EOF
)

echo $text > server_test_files/r00t/index.html
python3 -c "print('random.txt' * 120000)" >> server_test_files/r00t/random.txt

