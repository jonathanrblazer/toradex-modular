# toradex-modular

docker build --platform linux/arm64 -t jonathanrblazer/torizon-serial:dev .



docker run -it \
  --device=/dev/ttyACM0:/dev/ttyACM0 \
  --restart=unless-stopped \
  --name torizon-serial \
  torizon-serial:dev

MORE FLEXIBLE WITH USB:

docker run -it \
  --device-cgroup-rule='c 166:* rmw' \
  --restart=unless-stopped \
  --name torizon-serial \
  jonathanrblazer/torizon-serial:dev

docker run -it \
  --device=/dev/ttyACM0 \
  --device=/dev/ttyACM1 \
  --restart=unless-stopped \
  --name torizon-serial \
  jonathanrblazer/torizon-serial:dev

THIS IS THE GIRL:

docker run -it \
  --mount type=bind,source=/dev,target=/dev \
  --group-add dialout \
  --restart=unless-stopped \
  --name torizon-serial \
  jonathanrblazer/torizon-serial:dev

docker run -it \
  --privileged \
  --restart=unless-stopped \
  --name ts \
  jonathanrblazer/torizon-serial:dev

