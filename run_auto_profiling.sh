#now gprof

bazel build --cxxopt='-std=c++17' --copt=-march=native --copt="-pg" --cxxopt="-pg" --linkopt="-pg" //games/automata

cd bazel-bin/games/automata
./automata
cd ../../..

rm -rf profile_report
mkdir profile_report

gprof bazel-bin/games/automata/automata bazel-bin/games/automata/gmon.out >> profile_report/app_gprof_data.txt

google-chrome-stable --no-sandbox ./profile_report/app_gprof_data.txt > /dev/null 2>&1 &