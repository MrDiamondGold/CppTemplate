@echo off

echo "Starting."
for %%f in (%1\*.png) do (
    call toktx --linear --automipmap --zcmp 6 "output/%%~nf.ktx" %%f
    echo "Compressed output/%%~nf.ktx."
)
echo "Finished."