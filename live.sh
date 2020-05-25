cd content
git pull
cd ..
git pull
make clean
make
./generate
cp -r output/* /var/www/danieljon.es
setfacl -Rm u::rwX,g::r-X,o::--- /var/www/danieljon.es/
setfacl -Rdm u::rwX,g::r-X,o::--- /var/www/danieljon.es/
restorecon -vRF /var/www/danieljon.es
