# Godot WebGL Build - Docker

This docker setup runs a unity webgl build locally.

## Testing Locally

First, create a Godot build - make sure that it's placed in `/build/webgl`

Next, just run `./test.sh`. This will copy the contents of `/build/webgl` and run
docker-compose.

## Testing Locally - manual process

First, run:

```
docker-compose up
```

Then, open up the following url in your browser:

```
localhost://8080
```

To stop an instance:

```
docker-compose down
```

## Config

The `webgl.conf` file is an nginx config file.

## More info

See: https://dev.to/tomowatt/running-an-unity-webgl-game-within-docker-5039

