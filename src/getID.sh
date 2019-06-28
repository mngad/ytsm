#pwd
#rm temp/channelXMLList.conf
input="channelUrlList.conf"
url="https://www.youtube.com/feeds/videos.xml?channel_id="
cat $input | xargs -n 1 -P 100 -I % sh -c ' echo 'https://www.youtube.com/feeds/videos.xml?channel_id='"$(wget % -q -O - | grep '"channelId"' | cut -d\" -f4 | grep -v '"c4-flexible-player-box"')" >> temp/channelXMLList.conf'


