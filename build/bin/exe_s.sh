Path="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $Path
./server 8080 8081
