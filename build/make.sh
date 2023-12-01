Path="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
#sourceshttps://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
cd $Path
make clean
sleep 1
make
