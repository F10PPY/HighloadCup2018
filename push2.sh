docker logout
docker login stor.highloadcup.ru --username=Floppy17@yandex.ru --password=6740d10b7d47


docker build -t dumb .
docker tag dumb stor.highloadcup.ru/accounts/camel_flyer
docker push stor.highloadcup.ru/accounts/camel_flyer
