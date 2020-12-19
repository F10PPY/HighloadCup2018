sudo docker logout
sudo docker login stor.highloadcup.ru --username=poker-pro@yandex.ru --password=43bee4bfdd68

sudo docker build -t dumb .
sudo docker tag dumb stor.highloadcup.ru/accounts/ocelot_shooter
sudo docker push stor.highloadcup.ru/accounts/ocelot_shooter
