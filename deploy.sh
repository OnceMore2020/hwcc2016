cp ./source/flash.* ./deploy/future_net/
scp -P 8888 ./deploy/future_net/flash.* wgh@202.120.37.117:/home/wgh/deploy/future_net/
echo "Sync Complete. Start Remote Build:"
ssh -p 8888 wgh@202.120.37.117 "/home/wgh/deploy.sh"
echo -e "\n\nBuild Complete. Copy to Desktop"
scp -P 8888 wgh@202.120.37.117:/home/wgh/deploy/future_net.tar.gz ~/Desktop/
