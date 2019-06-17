rm channelXMLList.conf
input="channelUrlList.conf"
while IFS= read -r line
do
    ID=$(wget -q -O - $line | grep '"channelId"' | cut -d\" -f4)
    echo "https://www.youtube.com/feeds/videos.xml?channel_id=$ID" >> channelXMLList.conf
    

done < "$input"
