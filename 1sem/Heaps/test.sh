g++ -std=c++0x -Wall -Wextra -O2 -o main main.cpp

./main

diff binomial_heap.out leftist_heap.out
diff binomial_heap.out skew_heap.out
diff leftist_heap.out skew_heap.out

cat log

rm binomial_heap.out leftist_heap.out skew_heap.out main
