#!/usr/bin/env bash

#bazel test ... --test_output=all

bazel coverage \
--instrument_test_targets \
--combined_report=lcov \
--coverage_report_generator=@bazel_tools//tools/test/CoverageOutputGenerator/java/com/google/devtools/coverageoutputgenerator:Main \
...

rm -rf coverage_report
mkdir coverage_report
genhtml bazel-out/_coverage/_coverage_report.dat --output-directory coverage_report/

google-chrome-stable ./coverage_report/index.html > /dev/null 2>&1 &
