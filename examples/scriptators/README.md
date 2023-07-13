# Scriptator examples

These examples run with the default image by changing the descriptor (to add types) and the script.

They mark spans of multiple lines separated by empty lines on a few Wikipedia pages (in text format).

## Pythonnator

```bash
sudo docker run -v $PWD/pythonnator/scripts:/usr/local/uimacpp/scripts -v $PWD/data:/data -v $PWD/out:/out -v $PWD/pythonnator/desc:/usr/local/uimacpp/desc apache:uimacpp /usr/local/uimacpp/desc/Pythonnator.xml /data /out
```

## Perltator

```bash
docker run -v $PWD/perltator/scripts:/usr/local/uimacpp/scripts -v $PWD/data:/data -v $PWD/out:/out -v $PWD/perltator/desc:/usr/local/uimacpp/desc apache:uimacpp /usr/local/uimacpp/desc/Perltator.xml /data /out
```
