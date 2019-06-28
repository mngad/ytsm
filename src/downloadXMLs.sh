#pwd
input="../temp/channelXMLList.conf"
    cd cache
    cat $input | xargs -n1 -P100 wget -q 
    cd ..
