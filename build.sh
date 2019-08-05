mkdir .build
cd .build
cmake -DBoost_DEBUG=ON -DFLEXBIN_ENABLE_TEST=1 -Dgtest_force_shared_crt=ON ..
cmake --build . &> build.log
cd ..
