#/bin/sh

counter=0

while sleep 0.5
do
    truncate -s 1MB database/foo$counter
    echo "Created dummy data"
    counter=$[$counter +1]

done



