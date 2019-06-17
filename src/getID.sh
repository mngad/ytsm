#pwd
#rm temp/channelXMLList.conf
input="channelUrlList.conf"
while IFS= read -r line
do
    #echo $line
    ID=$(wget -q -O - $line | grep '"channelId"' | cut -d\" -f4)
    echo "https://www.youtube.com/feeds/videos.xml?channel_id=$ID" >> temp/channelXMLList.conf
    

done < "$input"
