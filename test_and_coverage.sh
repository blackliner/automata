bazel test ... --test_output=errors

bazel coverage -s \
--instrument_test_targets \
--combined_report=lcov \
--coverage_report_generator=@bazel_tools//tools/test/CoverageOutputGenerator/java/com/google/devtools/coverageoutputgenerator:Main \
//:auto