ID=$(cat $1 | grep '"channelId"' | cut -d\" -f4)
wget "https://www.youtube.com/feeds/videos.xml?channel_id=$ID"
mv "videos.xml?channel_id=$ID" "$1.xml"
