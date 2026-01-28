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

----------- FOR DEBUGGING ---------------

docker build \
  -f Dockerfile.debug \
  -t torizon-serial:debug .

docker run -it \
  --user 0 \
  --mount type=bind,source=/dev,target=/dev \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  -p 2345:2345 \
  --name ts-debug \
  torizon-serial:debug

[CAN REMOVE -P 2345:2345]
  OR WITH LIVE_MOUNT:

  docker run -it \
  --user 0 \
  --mount type=bind,source=/home/torizon/projects/toradex/toradex-modular/src,target=/app \
  --mount type=bind,source=/dev,target=/dev \
  --cap-add=SYS_PTRACE \
  --security-opt seccomp=unconfined \
  -p 2345:2345 \
  --name ts-debug \
  torizon-serial:debug


gdbserver :23450 ./torizon-serial
  SHOULD RETURN SOMETHING LIKE:
> Listening on port 2345
ACTUAL:
> Process ./torizon-serial created; pid = 9
> Listening on port 2345

RSYNC MADNESS

find . \( -name "*.cpp" -o -name "*.h" \) | entr torasync

