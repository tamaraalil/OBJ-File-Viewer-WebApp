OBJ File Viewer WebApp README
Program: a1.c, a1.h, readppm.py, webgl.css, webgl-demo.js, loaddata.js, index.php, exit.html, Makefile
Author: Tamara Alilovic

Compile:
    chmod +x readppm.py
    gcc -ansi -Wall a1.c -o a1.o -c -I/usr/include/python3.7m -I/usr/include/python3.7m  -Wno-unused-result -Wsign-compare -g -fdebug-prefix-
map=/build/python3.7-3.7.3=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -Wformat -Werror=format-security -DNDEBUG -g -fwrapv -O3 -Wall -fPIC 
    gcc -ansi -Wall a1.o -o a1 -L/usr/lib/python3.7/config-3.7m-x86_64-linux-gnu -L/usr/lib -lpython3.7m -lcrypt -lpthread -ldl -lutil -lm -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions

Steps to Compile:
    - Type chmod u=rwx,g=rwx,o=rwx loaddata.js into terminal
    - chmod u=rwx,g=rwx,o=rwx arrays.txt into terminal
    - chmod u=rwx,g=rwx,o=rwx webgl-demo.js into terminal
    - Type make.

Extra Notes:
    - Transfering the data to the database takes a minute or two. A message will appear under the buttons when it is complete.
    - Once more than one object is uploaded, after pressing the "Display #" button, you may have to refresh the page
      in order to see the object drawn. Once you refresh you should be able to switch between the displays of all of
      the objects uploaded to the database.
