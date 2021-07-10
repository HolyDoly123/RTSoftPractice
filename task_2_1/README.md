
```
$ sudo xhost+
$ sudo docker build -t ff_dock
$ sudo docker run -e DISPLAY=unix$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix ff_dock
