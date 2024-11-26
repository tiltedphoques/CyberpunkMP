FROM mcr.microsoft.com/dotnet/sdk:8.0.404-bookworm-slim-amd64 AS build
WORKDIR /app
ENV XMAKE_ROOT y

RUN echo 'deb http://deb.debian.org/debian bookworm-backports main' >> /etc/apt/sources.list
RUN apt update \
  && apt install -y xmake g++ \
  && apt clean

COPY . .

RUN --mount=type=cache,target=/root/.xmake xmake -y

# RELEASE STAGE
FROM mcr.microsoft.com/dotnet/runtime:8.0.11-bookworm-slim-amd64 AS release
WORKDIR /app

COPY --from=build /app/build/linux/x86_64/release/ /app/


EXPOSE 11778
CMD [ "/app/Server.Loader" ]
