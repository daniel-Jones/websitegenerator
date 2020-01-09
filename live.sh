cd content
git pull
cd ..
git pull
make clean
make
./generate
cp -r output/* /var/www/html
chmod -R +r /var/www/html/blog
chmod -R +r /var/www/html/posts
