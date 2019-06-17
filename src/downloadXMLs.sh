#pwd
input="temp/channelXMLList.conf"
while IFS= read -r line
do
    cd cache
    wget -q $line
    cd ..
done < "$input"
