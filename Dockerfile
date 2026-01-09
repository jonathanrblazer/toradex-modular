FROM --platform=linux/arm64 debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY src/main.cpp .

RUN g++ -std=c++17 -O2 -pthread main.cpp -o torizon-serial

CMD ["./torizon-serial"]
