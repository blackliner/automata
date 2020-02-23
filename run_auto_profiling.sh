#now gprof

bazel build --cxxopt='-std=c++17' --copt="-pg" --cxxopt="-pg" --linkopt="-pg" //games/automata:auto

cd bazel-bin/games/automata
./auto
cd ../../..

rm -rf profile_report
mkdir profile_report

gprof bazel-bin/games/automata/auto bazel-bin/games/automata/gmon.out >> profile_report/app_gprof_data.txt

google-chrome-stable ./profile_report/app_gprof_data.txt > /dev/null 2>&1 &