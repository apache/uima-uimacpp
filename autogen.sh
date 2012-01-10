#touch NEWS README AUTHORS ChangeLog
autoreconf -I m4 -I config  --force --install
rm -rf autom4te.cache
